//
// Created by Kehinde Adeoso on 9/4/26.
//

#include "test_utils.h"


std::vector<fs::path> get_utils(const fs::path& path, const std::string& extension) {
	std::vector<fs::path> utils;
	if (!fs::exists(path)) return utils;
	for (const auto& entry : fs::recursive_directory_iterator(path)) {
		if (entry.is_regular_file() && entry.path().extension() == extension) utils.push_back(entry.path());
	}
	return utils;
}