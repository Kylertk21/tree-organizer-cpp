#include "tree.hpp"
#include <functional>
#include <iostream>
#include <raylib.h>
#include <string>

template <typename T>

std::vector<Node<T> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";

int leafindex = 0;

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
      treeLayout(child, depth + 1); // recurse into children first
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
  DrawCircleV(node->screenPos, 20, DARKBLUE);
  DrawText(node->data.c_str(), node->screenPos.x - 20, node->screenPos.y - 8,
           10, WHITE);
}

int main(void) {

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    leafindex = 0;
    treeLayout(&node, 0);
    drawTree(&node);

    EndDrawing();
  }

  return 0;
}
