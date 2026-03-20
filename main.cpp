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

void clickNode(Node<std::string> *node) {
  if (selected == nullptr) {
    selected = node; // first click, select as parent
  } else if (selected == node) {
    selected = nullptr; // deselect
  } else {

    if (isAncestor(node, selected)) {
      std::cout << "Cant move a node under its children!" << std::endl;
      selected = nullptr;
      return;
    }

    if (node == &root) {
      std::cout << "Can't reparent root!" << std::endl;
      selected = nullptr;
      return;
    }
    detachFromParent(&root, node);
    selected->addChild(node); // second click, attach node as child of selected
    selected = nullptr;
  }
}

Node<std::string> *getClicked(Node<std::string> *node, Vector2 mouse) {
  if (!node)
    return nullptr;

  float textWidth = MeasureText(node->data.c_str(), fontSize);
  float w = textWidth + nodePaddingX;
  float h = 30 + nodePaddingY;

  if (mouse.x >=
          node->screenPos.x && // Can also use raylib's CheckCollisionPointRec
      mouse.x <=
          node->screenPos.x +
              w && // Check that mouse is within screenpos + width and height
      mouse.y >= node->screenPos.y &&
      mouse.y <= node->screenPos.y + h) {
    return node;
  }
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
  root = *loadNode(root);

  root.addChild(&child1);
  root.addChild(&child2);
  child2.addChild(&gchild);
  InitWindow(1920, 800, "Tree Organizer");

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
