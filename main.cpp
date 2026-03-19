#include "tree.hpp"
#include <functional>
#include <iostream>
#include <raylib.h>
#include <string>
template <typename T>

std::vector<Node<T> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";

int main(void) {

  Node<std::string> root("root");
  Node<std::string> child1("child 1");
  Node<std::string> child2("child 2");
  Node<std::string> gchild("gchild");

  createFile(root, "testing file testing file", path + "test.txt");
  root = *loadNode(root);

  root.addChild(&child1);
  root.addChild(&child2);
  child2.addChild(&gchild);

  InitWindow(800, 600, "Tree Organizer");

  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    leafindex = 0;
    treeLayout(&root, 0);
    drawTree(&root);

    EndDrawing();
  }

  return 0;
}
