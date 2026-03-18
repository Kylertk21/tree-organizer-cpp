#ifndef TREE_HPP
#define TREE_HPP
#pragma once
#include <string>
#include <vector>

template <typename T>

struct Node {
  std::string filepath;
  T data;
  std::vector<Node<T> *> children;

  Node(T data) : data(data) {} // Constructor initializer

  void addChild(Node<T> *child) { children.push_back(child); }
  void delChild(Node<T> &child) {
    children.erase(*child);
  } // erase at child's addr
};

std::string loadNode(const Node<std::string> &node);
void createFile(Node<std::string> &node, std::string input,
                const std::string path);
void deleteFile(Node<std::string> &node);

#endif
