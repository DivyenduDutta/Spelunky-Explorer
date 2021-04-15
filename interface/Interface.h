#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include "../utils/FileUtils.h"
#include "../utils/StringUtils.h"

using namespace std;

/*
Common to CLI and GUI - core code
*/

class Interface {
public:
	Interface();
	~Interface();

protected:
	void createWixWadBackup() const;
	bool prepareDirectoryForUnpack() const;
	bool prepareDirectoryForRepack() const;
	void unpackTextures() const;
	void repackTextures() const;

	void setWixFileName(string wixFileName);
	void setWadFileName(string wadFileName);
	string getWixFileName() const;
	string getWadFileName() const;

	void setAssetsDirectoryName(string assetsDirectoryName);
	string getAssetsDirectoryName() const;
	intmax_t getPhysicalFileSize(string& folderName, string& filename) const;
	void copyWadData(ifstream& origWadFile, int oldOffset, ofstream& newWadFile, int newOffset, uintmax_t size) const;
	void readAndWriteWadData(ofstream& newWadFile, int offset, uintmax_t size, ifstream& fileToRead) const;

private:
	string wixFileName;
	string wadFileName;
	string assetsDirectoryName;
};