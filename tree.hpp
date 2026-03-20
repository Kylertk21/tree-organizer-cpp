#ifndef TREE_HPP
#define TREE_HPP
#include <algorithm>
#include <pstl/glue_algorithm_defs.h>
#pragma once
#include "raylib.h"
#include <string>
#include <vector>

extern int leafindex;
extern int fontSize;
extern int textPaddingX;
extern int textPaddingY;
extern int nodePaddingX;
extern int nodePaddingY;

template <typename T>

struct Node {
  std::string filepath;
  std::string name;
  Node<T> *parent = nullptr;
  T data;
  Vector2 screenPos;
  std::vector<Node<T> *> children;

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

struct TextBox {
  Rectangle rect;
  std::string text;
  bool active = false;
  int fontSize = 20;
};

extern Node<std::string> *selected;
Node<std::string> *loadNode(Node<std::string> &node);
void createFile(Node<std::string> &node, std::string input,
                const std::string path);
void deleteFile(Node<std::string> &node);
void treeLayout(Node<std::string> *node, int depth);
void drawTree(Node<std::string> *node);
void detachFromParent(Node<std::string> *root, Node<std::string> *target);
bool isAncestor(Node<std::string> *ancestor, Node<std::string> *node);
#endif
