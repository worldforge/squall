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

#include "squall/core/Repository.h"
#include "squall/curl/CurlProvider.h"
#include "squall/core/Resolver.h"
#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_vector.hpp>
#include <utility>
#include <algorithm>
#include <iostream>

using namespace Squall;

TEST_CASE("Curl resolver copies files", "[resolver]") {
	setupEncodings();

	std::filesystem::path repoPath = TESTDATADIR "/repo/data";
	Repository repositoryDestination("CurlResolverTestDirectory");

	Resolver resolver(repositoryDestination,
					  std::make_unique<CurlProvider>("file://" + repoPath.generic_string()),
					  "678ad9fb8345c7677a1057b4fc9b4d8a26b2616256e1c296cd27b1b5e81b2c");

	int i = 0;
	while (true) {
		auto pollResult = resolver.poll(1);
		REQUIRE(pollResult.status != Squall::ResolveStatus::ERROR);
		REQUIRE(i++ < 20);
		if (pollResult.status == Squall::ResolveStatus::COMPLETE) {
			break;
		}
	}

	REQUIRE(repositoryDestination.fetch("2c26b46b68ffc68ff99b453c1d30413413422d706483bfa0f98a5e886266e7ae").status == Squall::FetchStatus::SUCCESS);


}