#include "tree.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <string>
template <typename T>

std::vector<Node<T> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";
Node<std::string> root("root", "Root");
Node<std::string> *selected = nullptr;

void clickNode(Node<std::string> *node) {
  if (selected == nullptr) {
    selected = node; // first click, select as parent
  } else if (selected == node) {
    selected = nullptr; // deselect
  } else {
    detachFromParent(&root, node);
    selected->addChild(node); // second click, attach node as child of selected
    selected = nullptr;
  }
}

Node<std::string> *getClicked(Node<std::string> *node, Vector2 mouse) {
  if (!node)
    return nullptr;

  float dx = mouse.x - node->screenPos.x; // Check if within node square
  float dy = mouse.y - node->screenPos.y;
  if ((dx * dx + dy * dy) <
      20 * 20) // distance = sqrt (dx*dx + dy*dy), 20*20 = sqrt
    return node;

  for (auto *child : node->children) { // Recurse through tree to determine who
                                       // was clicked, depth first
    Node<std::string> *hit = getClicked(child, mouse);
    if (hit)
      return hit;
  }
  return nullptr;
}

int main(void) {

  Node<std::string> child1("child 1", "Child 1");
  Node<std::string> child2("child 2", "Child 2");
  Node<std::string> gchild("gchild", "Child 3");

  createFile(root, "testing file testing file", path + "test.txt");
  std::string content = *loadNode(root);

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

    if (IsMouseButtonPressed(
            MOUSE_LEFT_BUTTON)) { // Click a node, recurse through tree to
                                  // determine who was clicked
      Node<std::string> *hit = getClicked(&root, GetMousePosition());
      if (hit)
        clickNode(hit);
    }

    EndDrawing();
  }

  return 0;
}
