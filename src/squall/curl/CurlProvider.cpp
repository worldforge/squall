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

#include "CurlProvider.h"
#include <curl/curl.h>

#include <utility>
#include <fstream>
#include <spdlog/spdlog.h>

namespace Squall {

CurlProvider::CurlProvider(std::string baseUrl)
		: mBaseUrl(std::move(baseUrl)) {
	curl_global_init(CURL_GLOBAL_ALL);
}

struct CurlFileEntry {
	CURL* easy;
	std::fstream& file;
	size_t bytesCopied;
	bool hasProcessedHeaders;
};

static size_t curlCallback(void* data, size_t, size_t numberOfBytes, void* userData) {
	auto* curlFileEntry = static_cast<CurlFileEntry*>(userData);
	if (!curlFileEntry->hasProcessedHeaders) {
		long responseCode;
		curl_easy_getinfo(curlFileEntry->easy, CURLINFO_RESPONSE_CODE, &responseCode);
		//0 is ok if we're on a non-http protocol
		if (responseCode != 200 && responseCode != 0) {
			return CURL_WRITEFUNC_ERROR;
		}
		curlFileEntry->hasProcessedHeaders = true;
	}
	curlFileEntry->file.write(static_cast<const char*>(data), static_cast<std::streamsize>(numberOfBytes));
	curlFileEntry->bytesCopied += numberOfBytes;
	return numberOfBytes;
}


std::future<ProviderResult> CurlProvider::fetch(Signature signature, std::filesystem::path destination) {
	auto curl = curl_easy_init();

	if (curl) {
		auto destinationPartialPath = destination;
		destinationPartialPath += ".partial";
		create_directories(destinationPartialPath.parent_path());
		std::fstream outputFile(destinationPartialPath, std::ios::out | std::ios::binary);
		if (!outputFile.good()) {
			std::promise<ProviderResult> promise;
			promise.set_value(ProviderResult{.status=ProviderResultStatus::FAILURE});
			return promise.get_future();
		}

		CurlFileEntry curlFileEntry{.easy = curl, .file = outputFile, .bytesCopied=0, .hasProcessedHeaders=false};


		auto first = signature.str_view().substr(0, 2);
		auto second = signature.str_view().substr(2);

		//Note: the "operator+" is (sillily) missing for std::string and std::string_view. Perhaps will be added in C++26?
		auto sourceFile = mBaseUrl + "/" + std::string(first) + "/" + std::string(second);
		curl_easy_setopt(curl, CURLOPT_URL, sourceFile.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &curlFileEntry);
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, curlCallback);

		auto res = curl_easy_perform(curl);

		curl_easy_cleanup(curl);
		outputFile.close();
		// something failed
		if (res != CURLE_OK) {
			spdlog::error("Curl download of file '{}' failed with error message: {}.", sourceFile, curl_easy_strerror(res));
			std::promise<ProviderResult> promise;
			promise.set_value(ProviderResult{.status=ProviderResultStatus::FAILURE});
			return promise.get_future();
		} else {
			std::filesystem::rename(destinationPartialPath, destination);
			std::promise<ProviderResult> promise;
			promise.set_value(ProviderResult{.status=ProviderResultStatus::SUCCESS, .bytesCopied=curlFileEntry.bytesCopied});
			return promise.get_future();
		}
	}
	std::promise<ProviderResult> promise;
	promise.set_value(ProviderResult{.status=ProviderResultStatus::FAILURE});
	return promise.get_future();
}

}