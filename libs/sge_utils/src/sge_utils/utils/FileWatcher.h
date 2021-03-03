#pragma once

#include "FileStream.h"

namespace sge {

struct FileWatcher
{
	FileWatcher() = default;
	FileWatcher(const char* const filenameToWatch) {
		*this = FileWatcher();
		initilize(filenameToWatch);
	}

	// Returns true if the file exists.
	bool initilize(const char* const filenameToWatch) {
		*this = FileWatcher(); // reset to defaults.
		if(filenameToWatch) {
			this->filename = filenameToWatch;
			return update();
		}
		
		return false;
	}

	// Returns true if the file was updated.
	bool update() {
		lastUpdateModtime = modtime;
		modtime = FileReadStream::getFileModTime(filename.c_str());
		isFileExisting = modtime != 0;
		return lastUpdateModtime != modtime;
	}

	bool doesFileExists() const {
		return isFileExisting;
	}

	bool hasChangedSinceLastUpdate() {
		if(isFileExisting == false) {
			return false;
		}
		
		sgeAssert(modtime >= lastUpdateModtime);
		return modtime > lastUpdateModtime;
	}
	
	const std::string& getFilename() const {
		return filename;
	}
	
private:

	bool isFileExisting = false;
	sint64 modtime = 0;
	sint64 lastUpdateModtime = 0;
	std::string filename;
};

}