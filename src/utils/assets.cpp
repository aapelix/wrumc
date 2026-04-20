#include "assets.hpp"
#include <filesystem>
#include <string>
#include <vector>

std::vector<std::string> list_pngs(const char *path) {
  std::vector<std::string> files;

  for (const auto &entry : std::filesystem::directory_iterator(path)) {
    if (entry.is_regular_file()) {
      if (entry.path().extension() == ".png") {
        files.push_back(entry.path().string());
      }
    }
  }

  return files;
}
