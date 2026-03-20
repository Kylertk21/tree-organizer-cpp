#include "tree.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <string>
template <typename T>

std::vector<Node<T> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";
Node<std::string> root("root");

const int screenWidth = 1920;
const int screenHeight = 1080;

void inputText(TextBox &box) {
  if (IsMouseButtonPressed(
          MOUSE_LEFT_BUTTON)) { // Check if box has been clicked
    box.active = CheckCollisionPointRec(GetMousePosition(), box.rect);
  }

  if (!box.active)
    return;

  int c = GetCharPressed();  // Get input char
  while (c > 0) {            // while not null terminated
    if (c >= 32 && c <= 125) // Only printable ASCII
      box.text += (char)c;
    c = GetCharPressed();
  }

  if (IsKeyPressed(KEY_BACKSPACE) &&
      !box.text.empty()) // Backspace, if box isnt empty
    box.text.pop_back();
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

  std::string text = node->data.empty() ? node->name : node->data;
  if (!node)
    return nullptr;

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
    Node<std::string> *hit = getClicked(child, mouse);
    if (hit)
      return hit;
  }
  return nullptr;
}

int main(void) {

  TextBox input = {
      {10, 10, 200, 30}, "", false, 20}; // L X W X H, text, active, fontsize

  InitWindow(screenWidth, screenHeight, "Tree Organizer");

  while (!WindowShouldClose()) {
    inputText(input);

    BeginDrawing();
    ClearBackground(WHITE);
    drawTextBox(input);
    leafindex = 0;
    treeLayout(&root, 0);
    drawTree(&root);

    if (IsKeyPressed(KEY_ENTER) && input.active) {
      std::cout << "Input: " << input.text << std::endl;
      input.text = "";
    }

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
