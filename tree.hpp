#ifndef TREE_HPP
#define TREE_HPP
#include <algorithm>
#include <pstl/glue_algorithm_defs.h>
#pragma once
#include "raylib.h"
#include <string>
#include <vector>
#define BOYGRAY (Color){50, 50, 59, 255}

extern int leafindex;
extern int fontSize;
extern int textPaddingX;
extern int textPaddingY;
extern int nodePaddingX;
extern int nodePaddingY;

extern int colorR;
extern int colorG;
extern int colorB;
extern int colorT;

extern const int screenWidth;
extern const int screenHeight;
extern const std::string path;

template <typename T>

struct Node {
  std::string filepath;
  std::string name;
  Node<T> *parent = nullptr;
  T data;
  Vector2 screenPos;
  std::vector<Node<T> *> children;
  bool pinned = false;

  Node(std::string name) : name(name) {} // Constructor initializer

  void addChild(Node<T> *child) {
    child->parent = this;      // This node is the parent
    children.push_back(child); // Adds a child
  };

  void detachChild() { // Detaches child from parent
    if (!parent)
      return;
    auto &c = parent->children;
    c.erase(std::remove(c.begin(), c.end(), this),
            c.end()); // Delete current node
    parent = nullptr;
  }
  void delChild(Node<T> &child) {
    children.erase(*child);
  } // erase at child's addr
};

extern std::vector<Node<std::string> *> nodes;

struct TextBox {
  Rectangle rect;
  std::string text;
  bool active = false;
  int fontSize = 20;
};

struct ContextMenu {
  bool open = false;
  Vector2 pos;
  Node<std::string> *target = nullptr;
};

extern Node<std::string> *selected;
extern Node<std::string> root;
Node<std::string> *loadNode(Node<std::string> &node);
extern Node<std::string> *drag;
extern Vector2 dragOffset;
extern bool isDragging;
void createFile(Node<std::string> &node, std::string input,
                const std::string path);
void deleteFile(Node<std::string> &node);
void treeLayout(Node<std::string> *node, int depth);
void drawTree(Node<std::string> *node);
void detachFromParent(Node<std::string> *root, Node<std::string> *target);
bool isAncestor(Node<std::string> *ancestor, Node<std::string> *node);

void clickNode(Node<std::string> *node);
Node<std::string> *getClicked(Node<std::string> *node, Vector2 mouse);

void drawTextBox(const TextBox &box);
void updateContextMenu(ContextMenu &menu, TextBox &dataInput);
void drawContextMenu(ContextMenu &menu, TextBox &dataInput);

#endif
