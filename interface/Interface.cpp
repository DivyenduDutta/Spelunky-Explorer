#include "Interface.h";

using namespace std;

Interface::Interface() {
	wixFileName = "";
	wadFileName = "";
}

Interface::~Interface() {
}

void Interface::setWixFileName(string wixFileName) {
	if (stringEndsWith(wixFileName, ".wix")) {
		//compare if filename ends with .wix
		this->wixFileName = wixFileName;
	}
}

void Interface::setWadFileName(string wadFileName) {
	if (stringEndsWith(wadFileName, ".wad")) {
		//compare if filename ends with .wad
		this->wadFileName = wadFileName;
	}
}

string Interface::getWixFileName() const {
	return wixFileName;
}

string Interface::getWadFileName() const {
	return wadFileName;
}

void Interface::setAssetsDirectoryName(string assetsDirectoryName) {
	this->assetsDirectoryName = assetsDirectoryName;
}

string Interface::getAssetsDirectoryName() const {
	return assetsDirectoryName;
}

bool Interface::prepareDirectoryForUnpack() const {
	createWixWadBackup();

	//create a directory called 'unpacked'
	if (!fs::directory_entry(fs::path("unpacked")).exists()) {
		fs::create_directory(fs::path("unpacked"));
	}
	else {
		return false;
	}
	return true;
}


bool Interface::prepareDirectoryForRepack() const {
	createWixWadBackup();
	//create a directory called 'unpacked'
	if (!fs::directory_entry(fs::path("repacked")).exists()) {
		fs::create_directory(fs::path("repacked"));
	}
	else {
		return false;
	}
	return true;
}

void Interface::createWixWadBackup() const {
	//creates .orig files of wad and wix files for backup
	fs::path wixFile(getWixFileName());
	fs::path wadFile(getWadFileName());

	fs::path wixOrigFile(getWixFileName() + ".orig");
	fs::path wadOrigFile(getWadFileName() + ".orig");

	fs::directory_entry wixOrigFileEntry(wixOrigFile);
	fs::directory_entry wadOrigFileEntry(wadOrigFile);

	if (!wixOrigFileEntry.exists()) {
		fs::copy(wixFile, wixOrigFile);
	}

	if (!wadOrigFileEntry.exists()) {
		fs::copy(wadFile, wadOrigFile);
	}
}

void Interface::unpackTextures() const {
	/*ifstream indexFile(getWixFileName(), ifstream::in);
	ifstream dataFile(getWadFileName(), ifstream::in | ifstream::binary);*/

	ifstream indexFile(getWixFileName());
	ifstream dataFile(getWadFileName(), ifstream::binary);

	filebuf* dataFileBuffer = dataFile.rdbuf();

	string line;
	string folderName;

	while (getline(indexFile, line)) { //reads a line
		vector<string> values;
		int offset;
		uintmax_t size;
		string fileName;
		if (stringStartsWith(line, "!group")) {
			values = splitString(line.c_str(), ' ');
			folderName = string(values[1]);
		}
		else {
			values = splitString(line.c_str(), ' ');
			fileName = values[0];
			offset = stoi(values[1]);
			size = stoi(values[2]);

			//read data from the wad file
			char* buffer = new char[size];
			dataFileBuffer->pubseekpos(offset);
			dataFileBuffer->sgetn(buffer, size);

			//write the read data to its own file in the respective folder
			if (!fs::directory_entry(fs::path("unpacked/" + folderName)).exists()) {
				fs::create_directory(fs::path("unpacked/" + folderName));
			}
			ofstream createdDataFile;
			createdDataFile.open("unpacked\\" + folderName + "\\" + fileName, fstream::binary);

			createdDataFile.write(buffer, size);
			createdDataFile.close();

			delete[] buffer;
		}
	}

	dataFile.close();
	indexFile.close();
}


void Interface::repackTextures() const {
	fs::remove(fs::path(getWixFileName()));
	fs::remove(fs::path(getWadFileName()));

	ifstream origWixFile(getWixFileName() + ".orig");
	ifstream origWadFile(getWadFileName() + ".orig", ifstream::binary);
	//filebuf* origWadFileBuffer = origWadFile.rdbuf();

	ofstream newWixFile("repacked\\" + getWixFileName(), ios::trunc);
	ofstream newWadFile("repacked\\" + getWadFileName(), ios::trunc | ios::binary);
	//filebuf* newWadFileBuffer = newWadFile.rdbuf();

	string line;
	string folderName;
	bool isOffsetNeeded = false;
	int nextFileOffset;

	while (getline(origWixFile, line)) {
		vector<string> values;
		int offset;
		uintmax_t oldSize;
		string fileName;

		uintmax_t newSize;

		if (stringStartsWith(line, "!group")) {
			values = splitString(line.c_str(), ' ');
			folderName = string(values[1]);
			newWixFile << line << "\n";
		}
		else {
			values = splitString(line.c_str(), ' ');
			fileName = values[0];
			offset = stoi(values[1]);
			oldSize = stoi(values[2]);

			if (doesFileExist(getAssetsDirectoryName() + "\\" + folderName + "\\" + fileName)) {
				newSize = getPhysicalFileSize(folderName, fileName);
				if (oldSize == newSize) {
					if (isOffsetNeeded) {
						line = values[0] + " " + to_string(nextFileOffset) + " " + values[2];

						//the below is a little cringe but we had to get the new offset by subtracting (nextFileOffset-oldSize) since
						//we have already calculated the next file's offset in nextFileOffset
						copyWadData(origWadFile, offset, newWadFile, nextFileOffset, oldSize);
						nextFileOffset += oldSize;
					}
					else {
						copyWadData(origWadFile, offset, newWadFile, offset, oldSize);
					}
					newWixFile << line << "\n";
						
				}
				else {
					line = values[0] + " " + (isOffsetNeeded ? to_string(nextFileOffset) : values[1]) + " " + to_string(newSize);
					newWixFile << line << "\n";
					ifstream fileToRead(getAssetsDirectoryName() + "\\" + folderName + "\\" + fileName, fstream::binary);
					readAndWriteWadData(newWadFile, (isOffsetNeeded ? nextFileOffset : stoi(values[1])), newSize, fileToRead);
					isOffsetNeeded = true;
					nextFileOffset = newSize + offset;
				}
			}
			else {
				cout << folderName << "/" << fileName << " not found! Repacking data from original WAD file\n";
				if (isOffsetNeeded) {
					line = values[0] + " " + to_string(nextFileOffset) + " " + values[2];
					copyWadData(origWadFile, offset, newWadFile, nextFileOffset, oldSize);
					nextFileOffset += oldSize;
				}
				else {
					copyWadData(origWadFile, offset, newWadFile, offset, oldSize);
				}
				newWixFile << line << "\n";
			}
		}
	}

	newWadFile.close();
	newWixFile.close();
	origWadFile.close();
	origWadFile.close();
}

void Interface::copyWadData(ifstream& origWadFile, int oldOffset, ofstream& newWadFile, int newOffset, uintmax_t size) const {
	char* buffer = new char[size];
	/*origWadFileBuffer->pubseekpos(oldOffset);
	origWadFileBuffer->sgetn(buffer, size);*/
	origWadFile.seekg(oldOffset);
	origWadFile.read(buffer, size);
	newWadFile.seekp(newOffset);
	newWadFile.write(buffer, size);

	delete[] buffer;
}

void Interface::readAndWriteWadData(ofstream& newWadFile, int offset, uintmax_t size, ifstream& fileToRead) const {
	char* buffer = new char[size];

	fileToRead.read(buffer, size);
	newWadFile.seekp(offset);
	newWadFile.write(buffer, size);

	delete[] buffer;
}

intmax_t Interface::getPhysicalFileSize(string& folderName, string& filename) const{
	fs::path filePath = fs::path(getAssetsDirectoryName() + "\\" + folderName + "\\" + filename);
	return getPhysicalFileSizeUtil(filePath);
}
