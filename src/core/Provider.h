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

#ifndef SQUALL_PROVIDER_H
#define SQUALL_PROVIDER_H

#include "Record.h"
#include "Repository.h"
#include <future>
#include <filesystem>

namespace Squall {
enum class ProviderResultStatus {
	SUCCESS,
	FAILURE
};
struct ProviderResult {
	ProviderResultStatus status;
};

struct Provider {
	virtual ~Provider() = default;

	virtual std::future<ProviderResult> fetch(Signature signature,
											  std::filesystem::path destination) = 0;
};

struct RepositoryProvider : public Provider {
	const Repository& mRepo;

	explicit RepositoryProvider(const Repository& repo) : mRepo(repo) {}


	std::future<ProviderResult> fetch(Signature signature,
									  std::filesystem::path destination) override;

};
}


#endif //SQUALL_PROVIDER_H
