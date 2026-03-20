#include "tree.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <sstream>
#include <string>
Node<std::string> *selected = nullptr;
namespace fs = std::filesystem;
int leafindex = 0;
int fontSize = 20;
int textPaddingX = 10;
int textPaddingY = 5;
int nodePaddingX = 20;
int nodePaddingY = 0;

void detachFromParent(Node<std::string> *root,
                      Node<std::string> *target) { // Detach node from parent
  for (auto *child : root->children) {
    auto &c = child->children;
    c.erase(std::remove(c.begin(), c.end(), target), c.end());
    detachFromParent(child, target); // Recurse through tree to remove parent
  }
  auto &c = root->children;
  c.erase(std::remove(c.begin(), c.end(), target), c.end());
}

Node<std::string> *loadNode(Node<std::string> &node) {
  std::ifstream file(node.filepath);
  if (!file.is_open())
    return nullptr;
  std::stringstream buffer;
  buffer << file.rdbuf();
  node.data = buffer.str();
  return &node;
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

void treeLayout(Node<std::string> *node, int depth) {
  // Graphical Tree Layout on Raylib

  if (!node)
    return;

  if (node->children.empty()) {
    node->screenPos.x =
        leafindex * 120 + 60; // screen position, space by 120 pixels
    leafindex++;
  } else {
    for (auto *child : node->children) {
      treeLayout(child,
                 depth +
                     1); // recurse into children first, add 1 layer of depth
    }

    float first =
        node->children.front()->screenPos.x; // put parent first (first element)
    float last = node->children.back()->screenPos.x;
    node->screenPos.x = (first + last) / 2.0f;
  }
  node->screenPos.y = depth * 100 + 60;
}

void drawTree(Node<std::string> *node) {

  if (!node)
    return;

  // Draw lines to children first so they appear behind nodes
  for (auto *child : node->children) {
    DrawLineV(node->screenPos, child->screenPos, GRAY);
    drawTree(child);
  }

  // Draw node
  float textWidth = MeasureText(node->data.c_str(), fontSize);
  float w = textWidth + nodePaddingX;
  float h = 30 + nodePaddingY;

  Color fill =
      (node == selected) ? ORANGE : DARKBLUE; // Highlight if node is selected

  DrawRectangleV(node->screenPos, {w, h}, fill);
  DrawText(node->data.c_str(), node->screenPos.x + textPaddingX,
           node->screenPos.y + textPaddingY, fontSize, WHITE);
}

// Return true if ancestor is above node in tree
bool isAncestor(Node<std::string> *ancestor, Node<std::string> *node) {
  if (!node)
    return false;
  if (node == ancestor)
    return true;
  for (auto *child : ancestor->children) {
    if (isAncestor(child, node)) {
      return true;
    }
  }
  return false;
}
