#include "tree.hpp"
#include <functional>
#include <iostream>
#include <string>

template <typename T>

std::vector<Node<T> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";

int main(void) {

  Node<std::string> root("root node");
  Node<std::string> child1("child 1");
  Node<std::string> child2("child 2");
  Node<std::string> gchild("grand child");

  root.addChild(&child1);
  root.addChild(&child2);
  child2.addChild(&gchild);

  createFile(root, "test", path + "test.txt");
  createFile(child1, "test2", path + "test2.txt");
  createFile(gchild, "test3", path + "test3.txt");

  std::string output = loadNode(root);
  std::cout << output << std::endl;

  output = loadNode(child1);
  std::cout << output << std::endl;

  output = loadNode(gchild);
  std::cout << output << std::endl;

  return 0;
}
