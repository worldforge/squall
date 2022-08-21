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

#ifndef SQUALL_DIGEST_H
#define SQUALL_DIGEST_H

#include <string>
#include <filesystem>
#include <vector>

namespace Squall {

static const std::string SignatureVersion = "1";

typedef std::string Signature;

enum class FileEntryType {
	FILE,
	DIRECTORY
};

struct FileEntry {
	std::string fileName;
	Signature signature;
	FileEntryType type;
	size_t size;
};

struct Digest {
	std::string version;
	std::vector<FileEntry> entries;
};

}

std::ostream& operator<<(std::ostream& out, const Squall::Digest& digest);

bool operator==(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs);
bool operator!=(const Squall::FileEntry& lhs, const Squall::FileEntry& rhs);


#endif //SQUALL_DIGEST_H
