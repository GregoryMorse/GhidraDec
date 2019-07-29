/**
 * @file idaplugin/plugin_config.cpp
 * @brief Module deals with GhidraDec plugin configuration.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <fstream>
#include <iostream>

#include <json/json.h>
#define JSONCPP_STRING std::string

#include "retdec/utils/file_io.h"
#include "retdec/utils/string.h"
#include "plugin_config.h"
#include "decompiler.h"
#include "sleighinterface.h"
#include "idaplugin.h"

namespace {

const std::string JSON_ghidraPath = "ghidraPath";
const std::string JSON_cacheSize = "cacheSize";
const std::string JSON_maxPayload = "maxPayload";
const std::string JSON_timeout = "timeout";
const std::string JSON_cmtLevel = "cmtLevel";
const std::string JSON_alysChecks = "alysChecks";
const std::string JSON_dispChecks = "dispChecks";
const std::string JSON_maxChars = "maxChars";
const std::string JSON_numChars = "numChars";
const std::string JSON_comStyle = "comStyle";
const std::string JSON_intFormat = "intFormat";


} // anonymous namespace


namespace idaplugin {

/**
 * Get root value from the provided JSON string.
 * @param[in]  json String containing entire JSON file.
 * @param[out] root JSON root value to get from config.
 * @param[in]  silent Should the function throw warning at user if
 *                    something goes wrong?
 * @return @c False is @a root value read ok, @c true otherwise.
 */
bool getConfigRootFromString(
		const std::string& json,
		Json::Value& root,
		bool silent = true)
{
	std::istringstream input(json);
	Json::CharReaderBuilder builder;
	JSONCPP_STRING errors;

	bool success = Json::parseFromStream(builder, input, &root, &errors);
	if (!success || root.isNull() || !root.isObject())
	{
		if ((!silent) && (errors.size() != 0))
		{
			WARNING_GUI("Failed to parse JSON content.\n" << errors << "\n");
		}
		return true;
	}

	return false;
}

/**
 * Get root value from the provided JSON file.
 * @param[in]  file   JSON file.
 * @param[out] root   JSON root value to get from config.
 * @return @c False is @a root value read ok, @c true otherwise.
 */
bool getConfigRootFromFile(
		const std::string& file,
		Json::Value& root)
{
	std::ifstream jsonFile(file, std::ios::in | std::ios::binary);
	if (!jsonFile)
	{
		return true;
	}

	std::string jsonContent;
	jsonFile.seekg(0, std::ios::end);
	jsonContent.resize(jsonFile.tellg());
	jsonFile.seekg(0, std::ios::beg);
	jsonFile.read(&jsonContent[0], jsonContent.size());
	jsonFile.close();

	return getConfigRootFromString(jsonContent, root);
}

/**
 * Read provided JSON file into plugins's global information.
 * @param rdgi Plugin's global information.
 * @return @c False is @a config read ok, @c true otherwise.
 */
bool readConfigFile(RdGlobalInfo& rdgi)
{
	Json::Value root;

	if (getConfigRootFromFile(rdgi.pluginConfigFile.getPath(), root))
	{
		return true;
	}

	rdgi.ghidraPath = root.get(JSON_ghidraPath, "").asString();
	rdgi.cacheSize = root.get(JSON_cacheSize, 10).asUInt64();
	rdgi.maxPayload = root.get(JSON_maxPayload, 50).asUInt64();
	rdgi.timeout = root.get(JSON_timeout, 30).asUInt64();
	rdgi.cmtLevel = root.get(JSON_cmtLevel, 20).asUInt64();
	rdgi.alysChecks = root.get(JSON_alysChecks, 1 | 4 | 8 | 16 | 32).asUInt();
	rdgi.dispChecks = root.get(JSON_dispChecks, 4 | 8 | 128 | 256 | 1024).asUInt();
	rdgi.maxChars = root.get(JSON_maxChars, 100).asUInt();
	rdgi.numChars = root.get(JSON_numChars, 2).asUInt();
	rdgi.comStyle = root.get(JSON_comStyle, 0).asInt();
	rdgi.intFormat = root.get(JSON_intFormat, 2).asInt();

	netnode nn("GhidraConfig");
	qstring qs;
	;
	if (netnode_qvalstr(nn, &qs) != -1) {
		int idx = 0;
		std::vector<std::string> split({ std::string() });
		const uchar delim = ';';
		std::for_each(qs.begin(), qs.end(), [&idx, &split, delim](uchar ch) { if (ch == delim) { idx++; split.push_back(std::string()); } else if (ch != 0) split[idx].push_back(ch); });
		rdgi.customPspec = split[0];
		rdgi.customCspec = split[1];
		rdgi.customSlafile = split[2];
		rdgi.customCallStyle = split[3];
	}

	return false;
}

/**
 * Save plugin's configuration into provided JSON file.
 * File content is rewritten.
 * @param rdgi Plugin's global information.
 */
void saveConfigTofile(RdGlobalInfo& rdgi)
{
	Json::Value root;

	if (getConfigRootFromFile(rdgi.pluginConfigFile.getPath(), root))
	{
		// Problem when reading config -- does not matter, we use empty root.
	}

	root[JSON_ghidraPath] = rdgi.ghidraPath;
	root[JSON_cacheSize] = rdgi.cacheSize;
	root[JSON_maxPayload] = rdgi.maxPayload;
	root[JSON_timeout] = rdgi.timeout;
	root[JSON_cmtLevel] = rdgi.cmtLevel;
	root[JSON_alysChecks] = rdgi.alysChecks;
	root[JSON_dispChecks] = rdgi.dispChecks;
	root[JSON_maxChars] = rdgi.maxChars;
	root[JSON_numChars] = rdgi.numChars;
	root[JSON_comStyle] = rdgi.comStyle;
	root[JSON_intFormat] = rdgi.intFormat;

	netnode nn("GhidraConfig", 0, true);
	nn.set((rdgi.customPspec + ";" + rdgi.customCspec + ";" + rdgi.customSlafile + ";" + rdgi.customCallStyle).c_str());

	Json::StreamWriterBuilder writer;
	writer.settings_["commentStyle"] = "All";
	std::ofstream jsonFile(rdgi.pluginConfigFile.getPath().c_str());
	jsonFile << Json::writeString(writer, root);
}

extern RdGlobalInfo* decompInfo;
int idaapi sleighCB(int button_code, form_actions_t& fa)
{
	qstring formGhidraDecPluginSettings =
		"BUTTON YES Compile\n"
		"GhidraDec Sleigh Pcode Snippet Compiler\n"
		"\n"
		"\n"
		"Enter Pcode snippet here and use the compile button to see the output.\n"
		"\n"
		"<Editor:t1:0:40::>\n"
		"<Output:t2:0:40::>\n"
		"\n";
	textctrl_info_t tinfo, toutinfo;
	tinfo.flags |= TXTF_AUTOINDENT | TXTF_ACCEPTTABS | TXTF_LINENUMBERS | TXTF_FIXEDFONT;
	tinfo.tabsize = 2;
	tinfo.text = "";
	toutinfo.flags |= TXTF_READONLY | TXTF_FIXEDFONT;
	toutinfo.tabsize = 0;
	toutinfo.text = "";
	int ok;
	std::string pspec, cspec, sleighfilename;
	if (decompInfo->customSlafile.empty() &&
		canDecompileInput() &&
		detectProcCompiler(decompInfo, pspec, cspec, sleighfilename)) {
	}
	if (!decompInfo->customSlafile.empty()) sleighfilename = decompInfo->customSlafile;
	do {
		if (tinfo.text.size() != 0) {
			try {
				toutinfo.text = DecompInterface::compilePcodeSnippet(sleighfilename,
					tinfo.text.c_str(), std::vector<std::pair<std::string, int>>(), std::vector<std::pair<std::string, int>>()).c_str();
			} catch (DecompError& err) {
				toutinfo.text = err.explain.c_str();
			}
		}
		ok = ask_form(formGhidraDecPluginSettings.c_str(), &tinfo, &toutinfo);
	} while (ok == ASKBTN_YES);

	if (ok == 0) {
		// ESC or CANCEL
		return true;
	}
	return false;
}

int idaapi displayCB(int button_code, form_actions_t& fa)
{
	sval_t cmtLevel = decompInfo->cmtLevel, maxChars = decompInfo->maxChars, numChars = decompInfo->numChars;
	int comStyle = decompInfo->comStyle, intFormat = decompInfo->intFormat;
	ushort alysChecks = decompInfo->alysChecks, dispChecks = decompInfo->dispChecks;
	qstring formGhidraDecPluginSettings =
		"GhidraDec Plugin Settings\n"
		"\n"
		"\n"
		"Settings will be permanently stored and you will not have to fill them each time you run decompilation.\n"
		"\n"
		"<Display printing of type casts:C>\n"
		"<Display EOL comments:C>\n"
		"<Display Header comment:C>\n"
		"<Display Line Numbers (future):C>\n"
		"<Display Namespaces (future):C>\n"
		"<Display PLATE comments:C>\n"
		"<Display POST comments:C>\n"
		"<Display PRE comments:C>\n"
		"<Display Warning comments:C>\n"
		"<Print 'NULL' for null pointers:C>\n"
		"<Print calling convention name:C>>\n"
		"<Comment line indent level:D::::>\n"
		"<Maximum characters in a code line:D::::>\n"
		"<Number of characters per indent level:D::::>\n"
		"<Comment style:b::::>\n"
		"<Integer format:b::::>\n"
		"\n";
	qstrvec_t cstyles, iformats;
	cstyles.push_back("/* C-style comments */");
	cstyles.push_back("// C++-style comments");
	iformats.push_back("Force Hexadecimal");
	iformats.push_back("Force decimal");
	iformats.push_back("Best Fit");

	int ok = ask_form(formGhidraDecPluginSettings.c_str(),
		&dispChecks,
		&cmtLevel, &maxChars, &numChars,
		&cstyles, &comStyle, &iformats, &intFormat
	);

	if (ok != ASKBTN_YES) {
		// ESC or CANCEL
		return true;
	} else {
		decompInfo->cmtLevel = cmtLevel;
		decompInfo->dispChecks = dispChecks;
		decompInfo->maxChars = maxChars;
		decompInfo->numChars = numChars;
		decompInfo->comStyle = comStyle;
		decompInfo->intFormat = intFormat;
	}
	return false;
}

/**
 * Present plugin configuration form to developer.
 * @param rdgi Plugin's global information.
 * @return @c True if cancelled, @c false otherwise.
 */
bool askUserToConfigurePlugin(RdGlobalInfo& rdgi)
{
	char cGhidraPath[QMAXPATH], cPspecPath[QMAXPATH],
		cCspecPath[QMAXPATH], cSlaPath[QMAXPATH];

	if (rdgi.ghidraPath.empty())
	{
		std::string pattern = rdgi.ghidraPath;
		qstrncpy(cGhidraPath, pattern.c_str(), QMAXPATH);
	}
	else
	{
		qstrncpy(cGhidraPath, rdgi.ghidraPath.c_str(), QMAXPATH);
	}

	qstring formGhidraDecPluginSettings =
		//"BUTTON NO Refresh Prototype (must if Compiler Spec changed)\n"
		"GhidraDec Plugin Settings\n"
		"\n"
		"\n"
		"Settings will be permanently stored and you will not have to fill them each time you run decompilation.\n"
		"\n"
		"Path to %A (unnecessary if it is in the system PATH):\n"
		"<Ghidra Folder:F1::60::>\n"
		"<Cache Size (Functions):D::::>\n"
		"<Decompiler Max-Payload (MBytes):D::::>\n"
		"<Decompiler Timeout (seconds):D::::>\n"
		"<Eliminate unreachable code:C>\n"
		"<Ignore unimplemented instructions:C>\n"
		"<Infer constant pointers:C>\n"
		"<Respect readonly flags:C>\n"
		"<Simplify extended integer operations:C>\n"
		"<Simplify predication:C>\n"
		"<Use inplace assignment operators:C>>\n"
		"<Display Options:B::::>\n"
		"<Sleigh Pcode Snippet Compiler:B::::>\n"
		"\n"
		"Following Settings are per database and stored in the .idb/.i64 file.\n"
		"<Processor Spec:f2::60::>\n"
		"<Compiler Spec:f3::60::>\n"
		"<Sleigh file:f4::60::>\n"
		"<Prototype Evaluation:b::::>\n"
		"\n";
	int curProto = -1;
	std::string cspec, pspec, sleighfilename, protoeval;
	if ((rdgi.customCspec.empty() || rdgi.customPspec.empty() || rdgi.customSlafile.empty()) &&
		canDecompileInput() &&
		detectProcCompiler(&rdgi, pspec, cspec, sleighfilename)) {
	}
	if (!rdgi.customCspec.empty()) cspec = rdgi.customCspec;
	if (!rdgi.customPspec.empty()) pspec = rdgi.customPspec;
	if (!rdgi.customSlafile.empty()) sleighfilename = rdgi.customSlafile;
	qstrncpy(cPspecPath, pspec.c_str(), QMAXPATH);
	qstrncpy(cCspecPath, cspec.c_str(), QMAXPATH);
	qstrncpy(cSlaPath, sleighfilename.c_str(), QMAXPATH);
	int ok;
	sval_t cacheSize = rdgi.cacheSize, maxPayload = rdgi.maxPayload, timeout = rdgi.timeout;
	ushort alysChecks = rdgi.alysChecks;
	std::vector<std::string> vec;
	do {
		qstrvec_t protoTypes;
		vec.clear();
		int defIdx = -1;
		if (!cspec.empty()) DecompInterface::getProtoEvals(cspec, vec, &defIdx);
		if (!rdgi.customCallStyle.empty()) protoeval = rdgi.customCallStyle;
		else protoeval = ccToStr(inf.cc.cm, 0, true);
		vec.insert(vec.begin(), "default");
		vec.insert(vec.begin(), "unknown");
		for (int i = 0; i < vec.size(); i++) {
			if (strcmp(protoeval.c_str(), vec[i].c_str()) == 0) curProto = i;
			protoTypes.push_back(vec[i].c_str());
		}
		if (curProto == -1) curProto = 0;

		ok = ask_form(formGhidraDecPluginSettings.c_str(),
			rdgi.ghidraPath.c_str(),
			cGhidraPath, 
			&cacheSize, &maxPayload, &timeout,
			&alysChecks, &displayCB, &sleighCB,
			cPspecPath, cCspecPath, cSlaPath,
			&protoTypes, &curProto
		);
		if (ok == ASKBTN_YES) {
			cspec = cCspecPath;
			if (cspec != rdgi.customCspec) {
				warning("Since you changed the compiler specification file, please confirm the prototype in use");
			}
		}
	} while (ok == ASKBTN_YES && cspec != rdgi.customCspec);
	if (ok != ASKBTN_YES)
	{
		// ESC or CANCEL
		return true;
	}
	else
	{
		rdgi.ghidraPath = cGhidraPath;
		rdgi.cacheSize = cacheSize;
		rdgi.maxPayload = maxPayload;
		rdgi.timeout = timeout;
		rdgi.alysChecks = alysChecks;
		
		//these need to be saved and loaded through the IDB somehow
		rdgi.customPspec = cPspecPath;
		rdgi.customCspec = cCspecPath;
		rdgi.customSlafile = cSlaPath;
		rdgi.customCallStyle = curProto == -1 ? "" : vec[curProto];
	}
	return false;
}

/**
 * @return @c True if cancelled, @c false otherwise.
 */
bool pluginConfigurationMenu(RdGlobalInfo& rdgi)
{
	bool canceled = askUserToConfigurePlugin(rdgi);
	if (!canceled)
	{
		saveConfigTofile(rdgi);
	}
	return canceled;
}

/**
 * @return @c False if success, @c true otherwise.
 */
bool addConfigurationMenuOption(RdGlobalInfo& rdgi)
{
	char optionsActionName[] = "ghidradec:ShowOptions";
	char optionsActionLabel[] = "GhidraDec plugin options...";

	static show_options_ah_t show_options_ah(&rdgi);

	static const action_desc_t desc = ACTION_DESC_LITERAL(
			optionsActionName,
			optionsActionLabel,
			&show_options_ah,
			nullptr,
			NULL,
			-1);

	if (!register_action(desc)
			|| !attach_action_to_menu(
					"Options/SourcePaths",
					optionsActionName,
					SETMENU_APP)
			|| !attach_action_to_menu(
					"Options/SourcePaths",
					"-",
					SETMENU_APP))
	{
		ERROR_MSG("Failed to register Options menu item for GhidraDec plugin!\n");
		return true;
	}

	return false;
}

} // namespace idaplugin
