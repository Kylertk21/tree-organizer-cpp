#include "tree.hpp"
#include <cerrno>
#include <cstring>
#include <filesystem>
#include <fstream>
#include <iostream>
#include <raylib.h>
#include <sstream>
#include <string>
#include <unordered_set>

Node<std::string> *selected = nullptr;
Node<std::string> root("root");
Node<std::string> *colorTarget = nullptr;
bool colorPickerOpen = false;
Vector2 colorPickerPos = {0, 0};

std::vector<Node<std::string> *> nodes;
namespace fs = std::filesystem;
int leafindex = 0;
int fontSize = 20;
int textPaddingX = 10;
int textPaddingY = 5;
int nodePaddingX = 20;
int nodePaddingY = 0;

float menuWidth = 220;
float menuHeight = 220;

const int screenWidth = 1920;
const int screenHeight = 800;

const std::string path = "/home/kyler/tree-organizer-cpp/files/";

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
  std::cout << "Attempting to create file at: " << path << std::endl;

  node.filepath = path;

  std::ofstream file(node.filepath);
  if (!file.is_open()) {
    std::cerr << "Failed to create file: " << path << std::endl;
    std::cerr << "Error: " << strerror(errno) << std::endl; // exact OS error
    return;
  }

  file << input;
  file.close();
  std::cout << "File created successfully: " << path << std::endl;
}

void deleteNode(Node<std::string> *target) {
  if (!target)
    return;
  if (target == &root) {
    std::cout << "Can't delete root!" << std::endl;
    return;
  }

  if (!target->filepath.empty()) {
    deleteFile(*target);
  }

  if (target->parent) {
    for (auto *child : target->children) {
      child->parent = target->parent;
      target->parent->children.push_back(child);
    }
  } else {
    for (auto *child : target->children) {
      root.addChild(child);
    }
  }

  detachFromParent(&root, target);

  nodes.erase(std::remove(nodes.begin(), nodes.end(), target), nodes.end());

  if (selected == target)
    selected = nullptr;

  delete target;
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

void treeLayout(Node<std::string> *node, int depth,
                std::unordered_set<Node<std::string> *> &visited) {
  // Graphical Tree Layout on Raylib

  if (!node)
    return;

  if (visited.count(node))
    return;
  visited.insert(node);

  if (node == drag || node->pinned) {

    for (auto *child : node->children) {
      treeLayout(child, depth + 1, visited);
    }
    return;
  }

  if (node->children.empty()) {
    node->screenPos.x = leafindex * 120 +
                        screenWidth / 2; // screen position, space by 120 pixels

    leafindex++;
  } else {
    for (auto *child : node->children) {
      treeLayout(child, depth + 1, visited); // recurse into children first, add
                                             // 1 layer of depth
    }

    float first =
        node->children.front()->screenPos.x; // put parent first (first element)
    float last = node->children.back()->screenPos.x;
    node->screenPos.x = (first + last) / 2.0f;
  }
  node->screenPos.y = depth * 100 + 60;
}

void treeLayout(Node<std::string> *node,
                int depth) { // Wrapper so caller doesnt manage set
  std::unordered_set<Node<std::string> *> visited;
  treeLayout(node, depth, visited);
}

void drawTree(Node<std::string> *node) {

  if (!node)
    return;

  // Draw node

  int compareBlank = ColorToInt(BLANK);
  int nodeCtoInt = ColorToInt(node->color);

  if (nodeCtoInt == compareBlank) {
    node->color = WHITE;
  }

  Color fill =
      (node == selected) ? ORANGE : BLANK; // Highlight if node is selected

  float textWidth = 0;
  std::string text = node->data.empty() ? node->name : node->data;
  textWidth = MeasureText(text.c_str(), fontSize);
  float w = textWidth + nodePaddingX;
  float h = 30 + nodePaddingY;

  // Draw lines to children first so they appear behind nodes
  for (auto *child : node->children) {
    DrawLineV(node->screenPos, child->screenPos, node->color);
    drawTree(child);
  }

  DrawRectangleV(node->screenPos, {w, h}, fill);
  DrawText(text.c_str(), node->screenPos.x + textPaddingX,
           node->screenPos.y + textPaddingY, fontSize, node->color);
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

Node<std::string> *
getClicked(Node<std::string> *node, Vector2 mouse,
           std::unordered_set<Node<std::string> *> &visited) {

  if (!node)
    return nullptr;
  if (visited.count(node))
    return nullptr; // Already visited
  visited.insert(node);

  std::string text = node->data.empty() ? node->name : node->data;

  float textWidth = MeasureText(text.c_str(), fontSize);
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
    Node<std::string> *hit = getClicked(child, mouse, visited);
    if (hit)
      return hit;
  }
  return nullptr;
}

Node<std::string> *getClicked(Node<std::string> *node,
                              Vector2 mouse) { // Wrapper
  std::unordered_set<Node<std::string> *> visited;
  return getClicked(node, mouse, visited);
}

void updateContextMenu(ContextMenu &menu,
                       TextBox &dataInput) { // Context menu on right click
  if (IsMouseButtonPressed(MOUSE_RIGHT_BUTTON)) {
    Node<std::string> *hit = getClicked(&root, GetMousePosition());
    if (hit) {
      menu.open = true;
      menu.pos = GetMousePosition();
      menu.target = hit;
      dataInput.text = "";
      dataInput.active = false;
    } else {
      menu.open = false;
      menu.target = nullptr;
    }
  }

  if (IsKeyPressed(KEY_ESCAPE)) {
    menu.open = false;
    menu.target = nullptr;
    dataInput.active = false;
  }
}

void drawTextBox(const TextBox &box) {
  DrawRectangleRec(box.rect, WHITE); // Draw input background
  DrawRectangleLinesEx(box.rect, 1,
                       box.active ? ORANGE : DARKGRAY); // Draw border
  DrawText(box.text.c_str(), box.rect.x + textPaddingX,
           box.rect.y + box.rect.height / 2 - box.fontSize / 2, box.fontSize,
           BLACK); // Draw text typed into box
  if (box.active && ((int)(GetTime() * 2) % 2 ==
                     0)) { // Blinking Cursor, blink twice per second
    float cursorX =
        box.rect.x + 5 + MeasureText(box.text.c_str(), box.fontSize);
    DrawLine(cursorX + 10, box.rect.y + 4, cursorX + 10,
             box.rect.y + box.rect.height - 4, BLACK);
  }
}

void drawContextMenu(ContextMenu &menu, TextBox &dataInput) {
  if (!menu.open || !menu.target)
    return;

  Rectangle menuRect = {menu.pos.x, menu.pos.y, menuWidth, menuHeight};

  DrawRectangleRec(menuRect, ORANGE);
  DrawRectangleLinesEx(menuRect, 1, DARKGRAY);

  DrawText(TextFormat("Node %s", menu.target->name.c_str()), // Title (nodename)
           menu.pos.x + 8, menu.pos.y + 8, 14, DARKGRAY);

  DrawText("Note File: ", menu.pos.x + 8, menu.pos.y + 32, 12,
           DARKGRAY); // Label

  dataInput.rect = {menu.pos.x + 8, menu.pos.y + 50, menuWidth - 16,
                    28}; // Input box
  dataInput.active = true;
  drawTextBox(dataInput);

  Rectangle changeColor = {menu.pos.x + 8, menu.pos.y + 90, menuWidth - 16, 18};

  Rectangle confirmCreate = {menu.pos.x + 8, menu.pos.y + 108,
                             menuWidth / 2 - 8, 18}; // Confirm button
  Rectangle deleteNodeRect = {menu.pos.x + (menuWidth / 2), menu.pos.y + 108,
                              menuWidth / 2 - 8, 18};
  bool colorHovered = CheckCollisionPointRec(GetMousePosition(), changeColor);
  DrawRectangleRec(changeColor, colorHovered ? DARKBROWN : BROWN);
  DrawText("Change Color", changeColor.x + 6, changeColor.y + 3, 10, WHITE);

  bool confirmHovered =
      CheckCollisionPointRec(GetMousePosition(), confirmCreate);
  DrawRectangleRec(confirmCreate, confirmHovered ? DARKGREEN : GREEN);
  DrawText("Confirm", confirmCreate.x + 6, confirmCreate.y + 3, 10, WHITE);

  bool deleteHovered =
      CheckCollisionPointRec(GetMousePosition(), deleteNodeRect);
  DrawRectangleRec(deleteNodeRect, deleteHovered ? DARKPURPLE : RED);
  DrawText("Delete Node", deleteNodeRect.x + 6, deleteNodeRect.y + 3, 10,
           WHITE);

  bool confirmed = (IsKeyPressed(KEY_ENTER) && dataInput.active) ||
                   (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && confirmHovered);

  if (confirmed && !dataInput.text.empty()) {
    std::string nodePath = path + menu.target->name + ".txt";
    menu.target->data = dataInput.text;
    createFile(*menu.target, dataInput.text, nodePath);
    std::cout << "Set Data On: " << menu.target->name << ": " << dataInput.text
              << std::endl;
    menu.open = false;
    menu.target = nullptr;
    dataInput.text = "";
    dataInput.active = false;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && deleteHovered) {
    deleteNode(menu.target);
    menu.open = false;
    menu.target = nullptr;
    dataInput.text = "";
    dataInput.active = false;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && colorHovered) {
    colorPickerOpen = true;
    colorTarget = menu.target;
    colorPickerPos = menu.pos;
    menu.open = false;
  }
}

void drawColorPicker() {
  if (!colorPickerOpen || !colorTarget) {
    return;
  }

  float colorPickerHeight = 100;
  float boxWidth = 60;
  float boxHeight = 30;
  float colorPickerWidth = (boxWidth * 3) + 12;

  Rectangle colorPicker = {colorPickerPos.x, colorPickerPos.y, colorPickerWidth,
                           colorPickerHeight};

  Rectangle colorRed = {colorPickerPos.x + 6, colorPickerPos.y + 30, boxWidth,
                        boxHeight};
  Rectangle colorBlue = {colorPickerPos.x + boxWidth + 6, colorPickerPos.y + 30,
                         boxWidth, boxHeight};
  Rectangle colorGreen = {colorPickerPos.x + (boxWidth * 2) + 6,
                          colorPickerPos.y + 30, boxWidth, boxHeight};

  DrawRectangleRec(colorPicker, YELLOW);
  DrawText("Change Node Color", colorPickerPos.x + 6, colorPickerPos.y + 12, 15,
           BLACK);

  bool hoverRed = (CheckCollisionPointRec(GetMousePosition(), colorRed));
  DrawRectangleRec(colorRed, hoverRed ? MAROON : RED);
  DrawText("RED", colorRed.x + (boxWidth / 3) - 5,
           colorRed.y + (boxHeight / 3) - 3, 15, BLACK);

  bool hoverBlue = (CheckCollisionPointRec(GetMousePosition(), colorBlue));
  DrawRectangleRec(colorBlue, hoverBlue ? DARKBLUE : BLUE);
  DrawText("BLUE", colorBlue.x + (boxWidth / 3) - 10,
           colorBlue.y + (boxHeight / 3) - 3, 15, BLACK);

  bool hoverGreen = (CheckCollisionPointRec(GetMousePosition(), colorGreen));
  DrawRectangleRec(colorGreen, hoverGreen ? DARKGREEN : GREEN);
  DrawText("GREEN", colorGreen.x + (boxWidth / 3) - 15,
           colorGreen.y + (boxHeight / 3) - 3, 15, BLACK);

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverRed) {
    colorTarget->color = RED;
  } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverBlue) {
    colorTarget->color = BLUE;
  } else if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) && hoverGreen) {
    colorTarget->color = GREEN;
  }

  if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON) &&
      !CheckCollisionPointRec(GetMousePosition(), colorPicker)) {
    colorPickerOpen = false;
    colorTarget = nullptr;
  }
}
