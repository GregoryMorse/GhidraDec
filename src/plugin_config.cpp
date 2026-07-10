/**
 * @file idaplugin/plugin_config.cpp
 * @brief Module deals with GhidraDec plugin configuration.
 * @copyright (c) 2017 Avast Software, licensed under the MIT license
 */

#include <cstdint>
#include <algorithm>
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
const std::string JSON_viewFeatures = "viewFeatures";
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
const std::string JSON_behaviorChecks = "behaviorChecks";
const std::string JSON_splitDatatypeChecks = "splitDatatypeChecks";
const std::string JSON_maxJumpTableSize = "maxJumpTableSize";
const std::string JSON_maxInstructions = "maxInstructions";
const std::string JSON_aliasBlock = "aliasBlock";
const std::string JSON_namespaceStrategy = "namespaceStrategy";
const std::string JSON_nanIgnore = "nanIgnore";
const std::string JSON_braceFormatFunction = "braceFormatFunction";
const std::string JSON_braceFormatIfElse = "braceFormatIfElse";
const std::string JSON_braceFormatLoop = "braceFormatLoop";
const std::string JSON_braceFormatSwitch = "braceFormatSwitch";

Json::Value jsonSval(sval_t value)
{
	return Json::Value(static_cast<Json::LargestInt>(value));
}

void applyEnvironmentConfig(idaplugin::RdGlobalInfo& rdgi)
{
	if (rdgi.ghidraPath.empty())
	{
		qstring ghidraDir;
		if (qgetenv("GHIDRADEC_GHIDRA_DIR", &ghidraDir))
		{
			rdgi.ghidraPath = ghidraDir.c_str();
		}
		else if (qgetenv("GHIDRA_INSTALL_DIR", &ghidraDir))
		{
			rdgi.ghidraPath = ghidraDir.c_str();
		}
	}

	qstring value;
	if (qgetenv("GHIDRADEC_PSPEC", &value))
	{
		rdgi.customPspec = value.c_str();
	}
	if (qgetenv("GHIDRADEC_CSPEC", &value))
	{
		rdgi.customCspec = value.c_str();
	}
	if (qgetenv("GHIDRADEC_SLEIGH", &value))
	{
		rdgi.customSlafile = value.c_str();
	}
	if (qgetenv("GHIDRADEC_CALL_STYLE", &value))
	{
		rdgi.customCallStyle = value.c_str();
	}
}

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
		applyEnvironmentConfig(rdgi);
		return true;
	}

	rdgi.ghidraPath = root.get(JSON_ghidraPath, "").asString();
	rdgi.viewFeatures = root.get(JSON_viewFeatures, ghidradec_default_view_features()).asUInt();
	rdgi.cacheSize = (sval_t)root.get(JSON_cacheSize, 10).asUInt64();
	rdgi.maxPayload = (sval_t)root.get(JSON_maxPayload, 50).asUInt64();
	rdgi.timeout = (sval_t)root.get(JSON_timeout, 30).asUInt64();
	rdgi.cmtLevel = (sval_t)root.get(JSON_cmtLevel, 20).asUInt64();
	rdgi.alysChecks = root.get(JSON_alysChecks, GHIDRADEC_DEFAULT_ANALYSIS_CHECKS).asUInt();
	rdgi.dispChecks = root.get(JSON_dispChecks, GHIDRADEC_DEFAULT_DISPLAY_CHECKS).asUInt();
	rdgi.maxChars = root.get(JSON_maxChars, 100).asUInt();
	rdgi.numChars = root.get(JSON_numChars, 2).asUInt();
	rdgi.comStyle = root.get(JSON_comStyle, 0).asInt();
	rdgi.intFormat = root.get(JSON_intFormat, 2).asInt();
	rdgi.behaviorChecks = root.get(JSON_behaviorChecks, GHIDRADEC_DEFAULT_BEHAVIOR_CHECKS).asUInt();
	rdgi.splitDatatypeChecks = root.get(JSON_splitDatatypeChecks, GHIDRADEC_DEFAULT_SPLIT_DATATYPE_CHECKS).asUInt();
	rdgi.maxJumpTableSize = (sval_t)root.get(JSON_maxJumpTableSize, 4096).asUInt64();
	rdgi.maxInstructions = (sval_t)root.get(JSON_maxInstructions, 100000).asUInt64();
	rdgi.aliasBlock = root.get(JSON_aliasBlock, 2).asInt();
	rdgi.namespaceStrategy = root.get(JSON_namespaceStrategy, 0).asInt();
	rdgi.nanIgnore = root.get(JSON_nanIgnore, 1).asInt();
	rdgi.braceFormatFunction = root.get(JSON_braceFormatFunction, 2).asInt();
	rdgi.braceFormatIfElse = root.get(JSON_braceFormatIfElse, 0).asInt();
	rdgi.braceFormatLoop = root.get(JSON_braceFormatLoop, 0).asInt();
	rdgi.braceFormatSwitch = root.get(JSON_braceFormatSwitch, 0).asInt();

	applyEnvironmentConfig(rdgi);

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

	applyEnvironmentConfig(rdgi);

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
	root[JSON_viewFeatures] = rdgi.viewFeatures;
	root[JSON_cacheSize] = jsonSval(rdgi.cacheSize);
	root[JSON_maxPayload] = jsonSval(rdgi.maxPayload);
	root[JSON_timeout] = jsonSval(rdgi.timeout);
	root[JSON_cmtLevel] = jsonSval(rdgi.cmtLevel);
	root[JSON_alysChecks] = rdgi.alysChecks;
	root[JSON_dispChecks] = rdgi.dispChecks;
	root[JSON_maxChars] = jsonSval(rdgi.maxChars);
	root[JSON_numChars] = jsonSval(rdgi.numChars);
	root[JSON_comStyle] = rdgi.comStyle;
	root[JSON_intFormat] = rdgi.intFormat;
	root[JSON_behaviorChecks] = rdgi.behaviorChecks;
	root[JSON_splitDatatypeChecks] = rdgi.splitDatatypeChecks;
	root[JSON_maxJumpTableSize] = jsonSval(rdgi.maxJumpTableSize);
	root[JSON_maxInstructions] = jsonSval(rdgi.maxInstructions);
	root[JSON_aliasBlock] = rdgi.aliasBlock;
	root[JSON_namespaceStrategy] = rdgi.namespaceStrategy;
	root[JSON_nanIgnore] = rdgi.nanIgnore;
	root[JSON_braceFormatFunction] = rdgi.braceFormatFunction;
	root[JSON_braceFormatIfElse] = rdgi.braceFormatIfElse;
	root[JSON_braceFormatLoop] = rdgi.braceFormatLoop;
	root[JSON_braceFormatSwitch] = rdgi.braceFormatSwitch;

	netnode nn("GhidraConfig", 0, true);
	nn.set((rdgi.customPspec + ";" + rdgi.customCspec + ";" + rdgi.customSlafile + ";" + rdgi.customCallStyle).c_str());

	Json::StreamWriterBuilder writer;
	writer.settings_["commentStyle"] = "All";
	std::ofstream jsonFile(rdgi.pluginConfigFile.getPath().c_str());
	jsonFile << Json::writeString(writer, root);
}

int idaapi sleighCB(int button_code, form_actions_t& fa)
{
	RdGlobalInfo* decompInfo = (RdGlobalInfo*)fa.get_ud();
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
		decompInfo->pm->canDecompileInput() &&
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
	RdGlobalInfo* decompInfo = (RdGlobalInfo*)fa.get_ud();
	sval_t cmtLevel = decompInfo->cmtLevel, maxChars = decompInfo->maxChars, numChars = decompInfo->numChars;
	int comStyle = decompInfo->comStyle, intFormat = decompInfo->intFormat;
	int namespaceStrategy = decompInfo->namespaceStrategy;
	int braceFormatFunction = decompInfo->braceFormatFunction;
	int braceFormatIfElse = decompInfo->braceFormatIfElse;
	int braceFormatLoop = decompInfo->braceFormatLoop;
	int braceFormatSwitch = decompInfo->braceFormatSwitch;
	ushort dispChecks = decompInfo->dispChecks;
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
		"<Legacy namespace checkbox (unused):C>\n"
		"<Display PLATE comments:C>\n"
		"<Display POST comments:C>\n"
		"<Display PRE comments:C>\n"
		"<Display Warning comments:C>\n"
		"<Print 'NULL' for null pointers:C>\n"
		"<Print calling convention name:C>\n"
		"<Hide implied extension casts:C>>\n"
		"<Comment line indent level:D::::>\n"
		"<Maximum characters in a code line:D::::>\n"
		"<Number of characters per indent level:D::::>\n"
		"<Comment style:b:0:18::>\n"
		"<Integer format:b:0:12::>\n"
		"<Namespace display:b:0:12::>\n"
		"<Function brace style:b:0:12::>\n"
		"<If/else brace style:b:0:12::>\n"
		"<Loop brace style:b:0:12::>\n"
		"<Switch brace style:b:0:12::>\n"
		"\n";
	qstrvec_t cstyles, iformats, namespaceStyles;
	qstrvec_t braceFunctionStyles, braceIfElseStyles, braceLoopStyles, braceSwitchStyles;
	cstyles.push_back("C /* */");
	cstyles.push_back("C++ //");
	iformats.push_back("Hex");
	iformats.push_back("Dec");
	iformats.push_back("Best");
	namespaceStyles.push_back("Minimal");
	namespaceStyles.push_back("None");
	namespaceStyles.push_back("All");
	for (qstrvec_t* styles : { &braceFunctionStyles, &braceIfElseStyles, &braceLoopStyles, &braceSwitchStyles }) {
		styles->push_back("Same");
		styles->push_back("Next");
		styles->push_back("Blank");
	}

	int ok = ask_form(formGhidraDecPluginSettings.c_str(),
		&dispChecks,
		&cmtLevel, &maxChars, &numChars,
		&cstyles, &comStyle, &iformats, &intFormat,
		&namespaceStyles, &namespaceStrategy,
		&braceFunctionStyles, &braceFormatFunction,
		&braceIfElseStyles, &braceFormatIfElse,
		&braceLoopStyles, &braceFormatLoop,
		&braceSwitchStyles, &braceFormatSwitch
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
		decompInfo->namespaceStrategy = namespaceStrategy;
		decompInfo->braceFormatFunction = braceFormatFunction;
		decompInfo->braceFormatIfElse = braceFormatIfElse;
		decompInfo->braceFormatLoop = braceFormatLoop;
		decompInfo->braceFormatSwitch = braceFormatSwitch;
	}
	return false;
}

int idaapi behaviorCB(int button_code, form_actions_t& fa)
{
	RdGlobalInfo* decompInfo = (RdGlobalInfo*)fa.get_ud();
	ushort alysChecks = decompInfo->alysChecks;
	ushort behaviorChecks = decompInfo->behaviorChecks;
	ushort splitDatatypeChecks = decompInfo->splitDatatypeChecks;
	sval_t maxJumpTableSize = decompInfo->maxJumpTableSize;
	sval_t maxInstructions = decompInfo->maxInstructions;
	int aliasBlock = decompInfo->aliasBlock;
	int nanIgnore = decompInfo->nanIgnore;

	qstring formGhidraDecPluginSettings =
		"GhidraDec Decompiler Behavior Options\n"
		"\n"
		"\n"
		"Settings will be permanently stored and you will not have to fill them each time you run decompilation.\n"
		"\n"
		"<Eliminate unreachable code:C>\n"
		"<Ignore unimplemented instructions:C>\n"
		"<Infer constant pointers:C>\n"
		"<Respect readonly flags:C>\n"
		"<Simplify extended integer operations:C>\n"
		"<Simplify predication:C>\n"
		"<Use inplace assignment operators:C>>\n"
		"<Analyze for loops:C>\n"
		"<Allow processor context changes during disassembly:C>\n"
		"<Treat unimplemented instructions as fatal:C>\n"
		"<Treat reinterpreted bytes as fatal:C>\n"
		"<Treat too many instructions as fatal:C>\n"
		"<Record jump-table load data:C>>\n"
		"<Maximum jump table entries:D::::>\n"
		"<Maximum instructions per function:D::::>\n"
		"<Alias blocking:b:0:12::>\n"
		"<Split structure assignments:C>\n"
		"<Split array assignments:C>\n"
		"<Split pointer load/store assignments:C>>\n"
		"<NaN handling:b:0:12::>\n"
		"\n";

	qstrvec_t aliasBlocks, nanIgnores;
	aliasBlocks.push_back("None");
	aliasBlocks.push_back("Struct");
	aliasBlocks.push_back("Array");
	aliasBlocks.push_back("All");
	nanIgnores.push_back("None");
	nanIgnores.push_back("Compare");
	nanIgnores.push_back("All");

	int ok = ask_form(formGhidraDecPluginSettings.c_str(),
		&alysChecks,
		&behaviorChecks,
		&maxJumpTableSize, &maxInstructions,
		&aliasBlocks, &aliasBlock,
		&splitDatatypeChecks,
		&nanIgnores, &nanIgnore
	);

	if (ok != ASKBTN_YES) {
		return true;
	} else {
		decompInfo->alysChecks = alysChecks;
		decompInfo->behaviorChecks = behaviorChecks;
		decompInfo->splitDatatypeChecks = splitDatatypeChecks;
		decompInfo->maxJumpTableSize = maxJumpTableSize;
		decompInfo->maxInstructions = maxInstructions;
		decompInfo->aliasBlock = aliasBlock;
		decompInfo->nanIgnore = nanIgnore;
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
		"%*\n"
		"\n"
		"Settings will be permanently stored and you will not have to fill them each time you run decompilation.\n"
		"\n"
		"Path to %A (unnecessary if it is in the system PATH):\n"
		"<Ghidra Folder:F1::60::>\n"
		"<Show Control-Flow Graph View:C>\n"
		"<Use Extra Parameter Identification Phase:C>\n"
		"<Back up IDA database before decompilation:C>>\n"
		"<Cache Size (Functions):D::::>\n"
		"<Decompiler Max-Payload (MBytes):D::::>\n"
		"<Decompiler Timeout (seconds):D::::>\n"
		"<Display Options:B::::>\n"
		"\n"
		"<Decompiler Behavior Options:B::::>\n"
		"\n"
		"<Sleigh Pcode Snippet Compiler:B::::>\n"
		"\n"
		"Following Settings are per database and stored in the .idb/.i64 file (IDA reports %A-bit processor %A).\n"
		"<Processor Spec:f2::60::>\n"
		"<Compiler Spec:f3::60::>\n"
		"<Sleigh file:f4::60::>\n"
		"<Prototype Evaluation:b:0:24::>\n"
		"\n";
	int curProto = -1;
	std::string cspec, pspec, sleighfilename, protoeval;
	if ((rdgi.customCspec.empty() || rdgi.customPspec.empty() || rdgi.customSlafile.empty()) &&
		rdgi.pm->canDecompileInput() &&
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
	ushort viewFeatures = rdgi.viewFeatures;
	std::vector<std::string> vec;
	do {
		qstrvec_t protoTypes;
		vec.clear();
		int defIdx = -1;
		if (!cspec.empty()) DecompInterface::getProtoEvals(cspec, vec, &defIdx);
		std::vector<std::string> protoValues;
		protoValues.push_back("default");
		std::string defaultLabel = "default";
		if (defIdx >= 0 && defIdx < (int)vec.size())
			defaultLabel += " (" + vec[defIdx] + ")";
		protoTypes.push_back(defaultLabel.c_str());
		for (const std::string& proto : vec) {
			if (proto.empty() || std::find(protoValues.begin(), protoValues.end(), proto) != protoValues.end())
				continue;
			protoValues.push_back(proto);
			protoTypes.push_back(proto.c_str());
		}
		if (!rdgi.customCallStyle.empty() && rdgi.customCallStyle != "unknown")
			protoeval = rdgi.customCallStyle;
		else
			protoeval = ccToStr(inf_cc_cm, 0, true);
		curProto = 0;
		for (size_t i = 0; i < protoValues.size(); i++)
			if (protoeval == protoValues[i])
				curProto = (int)i;
		if (curProto == -1) curProto = 0;

		ok = ask_form(formGhidraDecPluginSettings.c_str(),
			&rdgi,
			rdgi.ghidraPath.c_str(),
			cGhidraPath, &viewFeatures,
			&cacheSize, &maxPayload, &timeout,
			&displayCB, &behaviorCB, &sleighCB,
			inf_is_32bit() ? (inf_is_64bit() ? "32/64" : "32") : (inf_is_64bit() ? "64" : "?"),
			inf_procname.c_str(),
			cPspecPath, cCspecPath, cSlaPath,
			&protoTypes, &curProto
		);
		if (ok == ASKBTN_YES) {
			if (cspec != cCspecPath) {
				cspec = cCspecPath;
				warning("Since the compiler specification file was changed, please confirm the prototype in use");
			continue;
			}
		}
		vec = protoValues;
	} while (false);
	if (ok != ASKBTN_YES)
	{
		// ESC or CANCEL
		return true;
	}
	else
	{
		rdgi.ghidraPath = cGhidraPath;
		rdgi.viewFeatures = viewFeatures;
		rdgi.cacheSize = cacheSize;
		rdgi.maxPayload = maxPayload;
		rdgi.timeout = timeout;
		
		//these are saved and loaded through the IDB
		rdgi.customPspec = cPspecPath;
		rdgi.customCspec = cCspecPath;
		rdgi.customSlafile = cSlaPath;
		rdgi.customCallStyle = curProto <= 0 ? "" : vec[curProto];
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
	static const char optionsActionName[] = "ghidradec:ShowOptions";
	static const char optionsActionLabel[] = "GhidraDec plugin options...";

	static show_options_ah_t show_options_ah(&rdgi);

	action_desc_t desc = ACTION_DESC_LITERAL_PLUGMOD(
			optionsActionName,
			optionsActionLabel,
			&show_options_ah,
			rdgi.pm,
			nullptr,
			NULL,
			-1);
	if (rdgi.pluginIconId >= 0)
	{
		desc.icon = rdgi.pluginIconId;
	}

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
