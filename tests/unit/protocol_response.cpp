#include "sleighinterface.h"
#include "function_mapping.h"
#include "xml.hh"
#include <algorithm>
#include <cstring>
#include <iostream>
#include <limits>
#include <future>
#include <stdexcept>
#include <thread>
#include <sstream>
#include <memory>

ghidra::Document* xml_tree_whitespace(std::istream& stream);

class MemoryCallback : public DecompileCallback
{
public:
    std::vector<uchar> input;
    size_t position = 0;
    std::vector<std::string> warnings;
    std::thread::id reader;
    std::thread::id interrupter;
    bool blockRead = false;
    bool terminated = false;
    std::promise<void> interrupted;
    std::shared_future<void> ready = interrupted.get_future().share();
    int getBytes(unsigned char*, int, AddrInfo) override { return 0; }
    void getMappedSymbol(AddrInfo, MappedSymbolInfo&) override {}
    void getExternInfo(AddrInfo, std::string&, std::string&, FuncProtoInfo&) override {}
    void getMetaType(std::string, std::vector<TypeInfo>&) override {}
    void getComments(AddrInfo, std::vector<CommentInfo>&) override {}
    std::string getSymbol(AddrInfo) override { return {}; }
    std::vector<uchar> getStringData(AddrInfo) override { return {}; }
    std::string getPcodeInject(int, std::string, AddrInfo, std::string, unsigned long long) override { return {}; }
    void getCPoolRef(const std::vector<unsigned long long>&, CPoolRecord&) override {}
    std::string emit(std::string, std::string, std::string text) override { return text; }
    void getInits(std::vector<InitStateItem>&) override {}
    void launchDecompiler() override {}
    size_t readDec(void* buffer, size_t size) override
    {
        reader = std::this_thread::get_id();
        if (blockRead) {
            ready.wait();
            return 0;
        }
        size = std::min(size, input.size() - position);
        std::memcpy(buffer, input.data() + position, size);
        position += size;
        return size;
    }
    size_t writeDec(const void*, size_t size) override { return size; }
    void terminate() override { terminated = true; }
    void interrupt() override
    {
        interrupter = std::this_thread::get_id();
        interrupted.set_value();
    }
    void decompilerWarning(const std::string& message) override { warnings.push_back(message); }
    void burst(uchar type) { input.insert(input.end(), {0, 0, 1, type}); }
    void text(const std::string& value) { input.insert(input.end(), value.begin(), value.end()); }
};

struct DecompInterfaceTestAccess
{
    static std::string render(const std::string& markup, std::string& display)
    {
        MemoryCallback callback;
        DecompInterface bridge{};
        bridge.callback = &callback;
        std::istringstream input(markup);
        std::unique_ptr<ghidra::Document> document(xml_tree_whitespace(input));
        std::string prototype, colorPrototype;
        std::vector<std::tuple<std::vector<unsigned int>, std::string, unsigned int>> graph;
        return bridge.convertSourceDoc(document->getRoot(), display, prototype, colorPrototype, graph);
    }
    static std::string command(MemoryCallback& callback, int timeout)
    {
        DecompInterface bridge{};
        bridge.callback = &callback;
        bridge.statusGood = true;
        bridge.archId = 0;
        auto response = bridge.sendCommand1ParamTimeout("test", "argument", timeout);
        return {response.begin(), response.end()};
    }
    static std::string read(MemoryCallback& callback)
    {
        DecompInterface bridge{};
        bridge.callback = &callback;
        auto response = bridge.readResponse();
        return {response.begin(), response.end()};
    }
};

static void require(bool condition, const char* message)
{
    if (!condition)
        throw std::runtime_error(message);
}

static void framingTests()
{
    MemoryCallback interleaved;
    interleaved.burst(6);
    interleaved.burst(18); interleaved.text("before"); interleaved.burst(19);
    interleaved.burst(14); interleaved.text("left");
    interleaved.burst(18); interleaved.text("during"); interleaved.burst(19);
    interleaved.text("right"); interleaved.burst(15);
    interleaved.burst(18); interleaved.text("after"); interleaved.burst(19);
    interleaved.burst(16); interleaved.burst(17); interleaved.burst(7);
    require(DecompInterfaceTestAccess::read(interleaved) == "leftright", "warnings corrupted main output");
    require(interleaved.warnings == std::vector<std::string>{"before", "during", "after"}, "warnings lost");
    require(interleaved.position == interleaved.input.size(), "response framing not consumed");

    MemoryCallback error;
    error.burst(6); error.burst(14); error.text("partial output");
    error.burst(16); error.text("Low-level Error: bad <type> & data"); error.burst(17); error.burst(7);
    bool rejected = false;
    try {
        DecompInterfaceTestAccess::read(error);
    } catch (const DecompError& failure) {
        rejected = failure.explain == "Low-level Error: bad <type> & data";
    }
    require(rejected, "native error accepted as decompiled output");
    require(error.position == error.input.size(), "native error left an unread response");

    for (bool oversized : {false, true}) {
        MemoryCallback malformed;
        malformed.burst(6); malformed.burst(18);
        malformed.text(oversized ? std::string(65537, 'w') : "wrong terminator");
        malformed.burst(17); malformed.burst(7);
        rejected = false;
        try {
            DecompInterfaceTestAccess::read(malformed);
        } catch (const DecompError&) {
            rejected = true;
        }
        require(rejected, "malformed informational frame accepted");
    }
}

static void mappingTests()
{
    MappedSymbolInfo info{};
    info.entryPoint = 0x1000;
    info.ranges = {{"ram", 0xff0, 0x1040}, {"ram", 0x2000, 0x2020}};
    for (unsigned long long query : {0x1000ull, 0x1001ull, 0x1007ull}) {
        auto mapping = functionQueryMapping(info, {"ram", query});
        require(mapping.symbol && mapping.size == 8, "entry offcut window differs from 12.2");
    }
    auto before = functionQueryMapping(info, {"ram", 0xff8});
    require(!before.symbol && before.holeEnd == 0xfff, "pre-entry hole masks function entry");
    auto interior = functionQueryMapping(info, {"ram", 0x1008});
    require(!interior.symbol && interior.holeEnd == 0x1040, "interior hole bounds differ from 12.2");
    auto tail = functionQueryMapping(info, {"ram", 0x2000});
    require(!tail.symbol && tail.holeEnd == 0x2020, "tail chunk exposed as function entry");
    info.ranges = {{"ram", 0x1000, 0x1002}};
    require(functionQueryMapping(info, {"ram", 0x1001}).size == 3, "short function mapping overruns its body");
    info.entryPoint = std::numeric_limits<unsigned long long>::max() - 7;
    info.ranges = {{"ram", info.entryPoint, std::numeric_limits<unsigned long long>::max()}};
    require(functionQueryMapping(info, {"ram", info.entryPoint + 7}).size == 8, "entry window arithmetic overflow");
}

static void commandThreadTests()
{
    const auto caller = std::this_thread::get_id();
    MemoryCallback normal;
    normal.burst(6); normal.burst(14); normal.text("ok"); normal.burst(15);
    normal.burst(16); normal.burst(17); normal.burst(7);
    require(DecompInterfaceTestAccess::command(normal, 5) == "ok", "command response lost");
    require(normal.reader == caller, "protocol callbacks moved off the calling thread");
    MemoryCallback stalled;
    stalled.blockRead = true;
    bool timedOut = false;
    try {
        DecompInterfaceTestAccess::command(stalled, 0);
    } catch (const DecompError& failure) {
        timedOut = failure.explain == "process timeout";
    }
    require(timedOut && stalled.terminated, "deadline did not terminate stalled command");
    require(stalled.reader == caller && stalled.interrupter != caller, "watchdog performed protocol callbacks");
}

int main()
{
    try {
        framingTests();
        mappingTests();
        commandThreadTests();
        std::string display;
        const std::string labels = DecompInterfaceTestAccess::render(
            "<function><syntax>case </syntax><value color=\"const\">7</value>"
            "<syntax>:</syntax><break indent=\"0\"/><value color=\"keyword\">default</value>"
            "<syntax>:</syntax></function>", display);
        require(labels == "case 7:\ndefault:", "case/default token content was discarded");
        require(display == labels, "case/default tokens missing from displayed code");
    } catch (const std::exception& failure) {
        std::cerr << failure.what() << '\n';
        return 1;
    } catch (const DecompError& failure) {
        std::cerr << failure.explain << '\n';
        return 1;
    }
    return 0;
}
