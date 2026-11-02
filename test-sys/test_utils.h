//
// Created by Kehinde Adeoso on 9/4/26.
//

#pragma once
#include <filesystem>
#include <vector>
namespace fs = std::filesystem;


std::vector<fs::path> get_utils(const fs::path& path, const std::string& extension);