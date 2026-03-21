#include "tree.hpp"

#include <iostream>
#include <raylib.h>

std::vector<Node<std::string> *> nodes;

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

int main(void) {

  TextBox input = {{10, screenHeight - 40, 200, 30},
                   "",
                   false,
                   20}; // L X W X H, text, active, fontsize

  TextBox dataInput = {{0, 0, 0, 0}, "", false, 14}; // Context menu input

  ContextMenu contextMenu;

  InitWindow(screenWidth, screenHeight, "Tree Organizer");

  while (!WindowShouldClose()) {
    inputText(input);
    inputText(dataInput);

    BeginDrawing();
    ClearBackground(WHITE);

    leafindex = 0;
    treeLayout(&root, 0); // Layout
    drawTree(&root);
    drawTextBox(input);

    updateContextMenu(contextMenu, dataInput);

    if (!contextMenu.open &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // Only open if closed
      Node<std::string> *hit = getClicked(&root, GetMousePosition());
      if (hit)
        clickNode(hit);
    }

    if (IsKeyPressed(KEY_ENTER) && input.active) {
      std::cout << "Input: " << input.text << std::endl;
      createNode(input.text);
      input.text = "";
    }

    drawContextMenu(contextMenu, dataInput);
    EndDrawing();
  }

  return 0;
}
