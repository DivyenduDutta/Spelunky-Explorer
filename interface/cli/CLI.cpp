#include <iostream>
#include "CLI.h"


using namespace std;
namespace fs = std::filesystem;

CLI::CLI(int argc, char **argv) {
	this->argc = argc;
	this->argv = argv;
}

CLI::~CLI() { //I guess its a good practise to provide an empty destructor. Didnt find any other info online
}

void CLI::processCommand(CLI_COMMAND command) {
	switch (command) {
		case UNPACK:
			cliUnpack();
			break;
		case REPACK:
			cliRepack();
			break;
		case RESTORE:
			cliRestore();
			break;
		default: 
			cliRestore();
	}
}

void CLI::cliUnpack() {
	showMessage("Unpacking files...\n");
	if (!getFileNames(UNPACK)) {
		showError(".wad or .wix file not found/provided\n");
		return;
	}
	if (prepareDirectoryForUnpack()) {
		unpackTextures();
		showMessage("Unpacking files completed\n");
	}
	else {
		showWarning("The directory called unpacked already exists. Please check!!\n");
	}
	
}

void CLI::cliRepack() {
	showMessage("Repacking files...\n");
	if (!getFileNames(REPACK)) {
		showError(".wad or .wix file not found/provided!!");
		return;

		/*setWixFileName("alltex.wad.wix");
		ofstream wixFile;
		wixFile.open(getWixFileName(), fstream::binary);
		wixFile.close();

		setWadFileName("alltex.wad");
		ofstream wadFile;
		wadFile.open(getWadFileName(), fstream::binary);
		wadFile.close();*/
	}

	if (!extractAssetsDirectory()) {
		showError("Either the assets directory wasn't provided /" 
			      " assets directory provided isnt found /" 
				  " default assets directory - \"unpacked\" not found /"
			      "-dir= wasnt added before directory. Please check!!");
		return;
	}

	

	if (prepareDirectoryForRepack()) {
		repackTextures();
		showMessage("Repacking files completed\n");
	}
	else {
		showError("The directory called \"repacked\" already exists. Please check!!\n");
	}
}

void CLI::cliRestore() {
	cout << "Restore\n";
}

void CLI::run(string programName, string version) {
	string startup = "Running " + programName + " v " + version + "\n";
	showMessage(startup.c_str());
	if (!checkInput()) {
		showError("The argument count or command isnt correct\n");
		return;
	}

	CLI_COMMAND command = argToCommand(argv[1]);
	processCommand(command);
}

bool CLI::checkInput() {
	if (argc < 2) {
		printUsage();
		return false;
	}

	if (!isCommand(argv[1])) {
		printUsage();
		return false;
	}

	argcRange range = getArgCount(argToCommand(argv[1]));

	if (argc < range.low || argc > range.high) {
		printUsage();
		return false;
	}

	return true;
}

CLI_COMMAND CLI::argToCommand(const char* arg) {
	if (!strcmp(arg, "-unpack") || !strcmp(arg, "-u")) {
		return UNPACK;
	}
	if (!strcmp(arg, "-repack") || !strcmp(arg, "-r")) {
		return REPACK;
	}
	if (!strcmp(arg, "-restore") || !strcmp(arg, "-rs")) {
		return RESTORE;
	}
	else {
		return RESTORE;
	}
}

argcRange CLI::getArgCount(CLI_COMMAND command) {
	switch (command) {
	case UNPACK:
		return { 2, 4 };
	case REPACK:
		return { 2, 5 };
	case RESTORE:
		return { 2, 2 };
	default:
		return { 2, 2 };
	}
}

bool CLI::isCommand(const char* arg) {
	return string(arg).length() > 1 && arg[0] == 0x2D && arg[1] != 0x2D; //hex for - since all commands start with '-' 
}

bool CLI::getFileNames(CLI_COMMAND command) {
	bool isError = false;
	if ((command == UNPACK && argc == 4) || (command == REPACK && argc == 5) ||
		(command == REPACK && argc == 4 && !stringStartsWith(argv[3], "-dir="))) {
		if (stringEndsWith(argv[2], ".wix")) {
			checkAndSetWixWadFiles(argv[2], argv[3], isError);
		}
		else if (stringEndsWith(argv[2], ".wad")) {
			checkAndSetWadWixFiles(argv[2], argv[3], isError);
		}
		else {
			isError = true;
		}
	}
	else if ((command == UNPACK && argc == 3) || (command == REPACK && argc == 4 && stringStartsWith(argv[3], "-dir=")) ||
		(command == REPACK && argc == 3 && !stringStartsWith(argv[2], "-dir="))) {
		if (stringEndsWith(argv[2], ".wix")) {
			fs::path filePath(argv[2]);
			//getting filename without extension ie without .wix
			string wadFileName = filePath.stem().string();
			checkAndSetWixWadFiles(argv[2], wadFileName, isError);
		}else if (stringEndsWith(argv[2], ".wad")) {
			string wixFileName = string(argv[2]).append(".wix");
			checkAndSetWadWixFiles(argv[2], wixFileName, isError);
		}
		else {
			isError = true;
		}
	}
	else if((command == UNPACK && argc == 2) || (command == REPACK && ((argc == 3 && stringStartsWith(argv[2], "-dir="))||argc==2))){
		//iterate over all the files in the drectory and set wad wix files as the first ones found
		bool isFileFound = false;
		for (auto& p: fs::directory_iterator(".")) {
			string fileName = p.path().filename().string();
			if (stringEndsWith(fileName, ".wix")) {
				fs::path filePath(fileName);
				//getting filename without extension ie without .wix
				string wadFileName = filePath.stem().string();
				checkAndSetWixWadFiles(fileName, wadFileName, isError);
				isFileFound = true;
				break;
			}
			else if (stringEndsWith(fileName, ".wad")) {
				string wixFileName = string(fileName).append(".wix");
				checkAndSetWadWixFiles(fileName, wixFileName, isError);
				isFileFound = true;
				break;
			}
		}

		if (!isFileFound) {
			isError = true;
		}
	}

	/*if (isError) {
		showError(".wad or .wix file not found/provided\n");
	}*/

	return !isError;
}

bool CLI::extractAssetsDirectory() {
	bool isError = false;
	if (argc == 5) {
		if (stringStartsWith(argv[4], "-dir=")) {
			extractDirNameAndSet(argv[4], isError);
		}
		else {
			isError = true;
		}
	}
	else if (argc == 4) {
		if (stringStartsWith(argv[3], "-dir=")) {
			extractDirNameAndSet(argv[3], isError);
		}
		else {
			extractDefaultDirNameAndSet(isError);
		}
	}
	else if (argc == 3) {
		if (stringStartsWith(argv[2], "-dir=")) {
			extractDirNameAndSet(argv[2], isError);
		}
		else {
			extractDefaultDirNameAndSet(isError);
		}
	}
	else if(argc == 2){
		extractDefaultDirNameAndSet(isError);
	}

	return !isError;
}

void CLI::extractDirNameAndSet(string dirArgument, bool& isError) {
	string assetsDirName = "";
	string dirArg = string(dirArgument);
	assetsDirName = dirArg.substr(5, dirArg.length() - 5);
	if (doesFileExist(assetsDirName)) {
		setAssetsDirectoryName(assetsDirName);
	}
	else {
		isError = true;
	}
}

void CLI::extractDefaultDirNameAndSet(bool& isError) {
	string assetsDirName = "unpacked";
	showMessage("Assets directory not provided. Using \"unpacked\" as default directory to use.");
	if (doesFileExist(assetsDirName)) {
		setAssetsDirectoryName(assetsDirName);
	}
	else {
		isError = true;
	}
}

void CLI::checkAndSetWixWadFiles(string wixFileName, string wadFileName, bool& isError) {
	if (doesFileExist(wixFileName)) {
		setWixFileName(wixFileName);
		if (stringEndsWith(wadFileName, ".wad") && doesFileExist(wadFileName)) {
			setWadFileName(wadFileName);
		}
		else {
			isError = true;
		}
	}
	else {
		isError = true;
	}
}

void CLI::checkAndSetWadWixFiles(string wadFileName, string wixFileName, bool& isError) {
	if (doesFileExist(wadFileName)) {
		setWadFileName(wadFileName);
		if (stringEndsWith(wixFileName, ".wix") && doesFileExist(wixFileName)) {
			setWixFileName(wixFileName);
		}
		else {
			isError = true;
		}
	}
	else {
		isError = true;
	}
}

void CLI::printUsage() {
	showMessage(USAGE_MESSAGE);
}

void CLI::showMessage(const char * message) {
	cout << message << endl;
}

void CLI::showError(const char* error) {
	cout << "Error : "  << error << endl;
}

void CLI::showWarning(const char* warning) {
	cout << "Warning : " << warning << endl;
}