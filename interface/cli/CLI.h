#pragma once
#include <string>
#include "../Interface.h"

using namespace std;

/*
CLI specific code
*/

typedef enum CLI_COMMAND {
	UNPACK,
	REPACK,
	RESTORE
} CLI_COMMAND;

struct argcRange {
	uint8_t low;
	uint8_t high;
};

class CLI: public Interface{
public:
	CLI(int argc, char **argv);
	~CLI();

	void run(string programName, string version);
	void exit();

private:
	int argc;
	char** argv;

	const char* USAGE_MESSAGE = "Usage: \t SpelunkyExplorer.exe [-u/-unpack]  wix_file \twad_file \n"
								"\t SpelunkyExplorer.exe [-u/-unpack]  wix/wad_file \n"
								"\t SpelunkyExplorer.exe [-u/-unpack] \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  wix_file \twad_file \t-dir=directory_with_assets \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  wix/wad_file \t-dir=directory_with_assets \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  wix_file \twad_file \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  wix/wad_file \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  -dir=directory_with_assets \n"
								"\t SpelunkyExplorer.exe [-r/-repack]  \n"
								"\t SpelunkyExplorer.exe [-rs/-restore]\n"
								"Available options: \n"
								"\tUnpack:		\t-u, -unpack\n"
								"\tRepack:		\t-r, -repack\n"
								"\tRestore:		-rs, -restore\n";

	const char * EXITING = "Exiting\n";

	//command methods
	void cliUnpack();
	void cliRepack();
	void cliRestore();

	void showError(const char* error);
	void showMessage(const char* message);
	void showWarning(const char* warning);


	argcRange getArgCount(CLI_COMMAND command);
	CLI_COMMAND argToCommand(const char * arg);
	bool isCommand(const char * arg);
	void printUsage();
	bool checkInput();
	void processCommand(CLI_COMMAND command);
	bool getFileNames(CLI_COMMAND command);
	bool extractAssetsDirectory();
	void extractDirNameAndSet(string dirArgument, bool& isError);
	void extractDefaultDirNameAndSet(bool& isError);
	void checkAndSetWixWadFiles(string wixFileName, string wadFileName, bool& isError);
	void checkAndSetWadWixFiles(string wadFileName, string  wixFileName, bool& isError);

};