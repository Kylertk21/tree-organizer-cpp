#include "tree.hpp"

#include <iostream>
#include <raylib.h>
#include <string>

Node<std::string> *drag = nullptr;
Vector2 dragOffset = {0, 0};
bool isDragging = false;

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
    Vector2 mouse = GetMousePosition();

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // start drag
      Node<std::string> *hit = getClicked(&root, mouse);
      if (hit) {
        drag = hit;
        dragOffset = {mouse.x - hit->screenPos.x, mouse.y - hit->screenPos.y};
      }
    }

    if (IsMouseButtonDown(MOUSE_LEFT_BUTTON) &&
        drag) { // if mouse button is held
      float dx =
          mouse.x -
          (drag->screenPos.x +
           dragOffset.x); // update node position with a calculated drag offset
      float dy = mouse.y - (drag->screenPos.y +
                            dragOffset.y); // mouse.x = 300, screenpos.x = 150,
                                           // dragoffset = 150, dx = 150
      if (!isDragging && (dx * dx + dy * dy) > 25) {
        isDragging = true;
      }
      if (isDragging) {
        drag->screenPos.x = mouse.x - dragOffset.x;
        drag->screenPos.y = mouse.y - dragOffset.y;
      }
    }

    if (IsMouseButtonReleased(MOUSE_LEFT_BUTTON)) {
      if (isDragging && drag) { // keep the node at its position
                                // after dragging (dont recalc layout)
        drag->pinned = true;
      } else if (!isDragging && drag) {
        clickNode(drag);
      }
      drag = nullptr;
      isDragging = false;
    }

    BeginDrawing();
    ClearBackground(BOYGRAY);

    leafindex = 0;
    treeLayout(&root, 0); // Layout
    drawTree(&root);

    drawTextBox(input);

    updateContextMenu(contextMenu, dataInput);

    if (!contextMenu.open &&
        IsMouseButtonPressed(MOUSE_LEFT_BUTTON)) { // Only open if closed
      Node<std::string> *hit = getClicked(&root, mouse);
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
