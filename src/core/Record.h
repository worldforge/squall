/*
 Copyright (C) 2022 Erik Ogenvik

 This program is free software; you can redistribute it and/or modify
 it under the terms of the GNU General Public License as published by
 the Free Software Foundation; either version 2 of the License, or
 (at your option) any later version.

 This program is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.

 You should have received a copy of the GNU General Public License
 along with this program; if not, write to the Free Software
 Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef SQUALL_RECORD_H
#define SQUALL_RECORD_H

#include "Signature.h"
#include <string>
#include <filesystem>
#include <vector>

namespace Squall {

static const std::string SignatureVersion = "1";


enum class FileEntryType {
	FILE,
	DIRECTORY
};

struct FileEntry {
	/**
	 * The file name of this entry (or directory name if a directory).
	 */
	std::string fileName;
	Signature signature;
	FileEntryType type;
	std::int64_t size;

	bool operator==(const FileEntry& rhs) const;

	bool operator!=(const FileEntry& rhs) const;
};

struct Record {
	std::string version;
	std::vector<FileEntry> entries;

	bool operator==(const Record& rhs) const;

	bool operator!=(const Record& rhs) const;
};

}

std::ostream& operator<<(std::ostream& out, const Squall::Record& record);
Squall::Record& operator<<(Squall::Record& record, std::istream& in);

#endif //SQUALL_RECORD_H
