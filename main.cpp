#include "tree.hpp"
#include <functional>
#include <iostream>
#include <memory>
#include <raylib.h>
#include <string>

std::vector<Node<std::string> *> nodes;
std::string path = "/home/kyler/dev/raylib-projects/tree-organizer-cpp/";

const int screenWidth = 1920;
const int screenHeight = 1080;

void createNode(std::string input) {
  if (input.empty())
    return;

  for (auto *node : nodes) {
    if (node->name == input) {
      std::cout << "Name taken" << std::endl;
      return;
    }
  }

  Node<std::string> *newNode = new Node<std::string>(input);
  nodes.push_back(newNode);

  if (selected != nullptr) {
    selected->addChild(newNode);
    selected = nullptr;
  } else {
    root.addChild(newNode);
  }

  std::cout << "Created Node: " << input << std::endl;
}

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
      createNode(input.text);
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
