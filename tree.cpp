#include "tree.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

namespace fs = std::filesystem;

std::string loadNode(const Node<std::string> &node) {
  std::ifstream file(node.filepath);
  if (!file.is_open())
    return "file not found";
  std::stringstream buffer;
  buffer << file.rdbuf();
  return buffer.str();
}

void createFile(Node<std::string> &node, std::string input,
                const std::string path) {
  if (node.filepath.empty()) {
    node.filepath = path;
  } else {
    std::cout << "File Exists in Node!" << std::endl;
    return;
  }
  std::ofstream file(node.filepath);
  if (!file.is_open()) {
    std::cerr << "Failed to Create File!" << std::endl;
  }
  file << input;
  file.close();
}

void deleteFile(Node<std::string> &node) {
  fs::path filepath = node.filepath;
  try {
    if (fs::remove(filepath)) {
      std::cout << "File Removed From Node: " << filepath << std::endl;
    } else {
      std::cout << "File Could Not Be Deleted: " << filepath << std::endl;
    }
  } catch (const fs::filesystem_error &ex) {
    std::cerr << "Filesystem Error: " << ex.what() << std::endl;
  }
}
