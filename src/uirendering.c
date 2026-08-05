#include "raylib.h"
#define HEIGHT 900
#include "uirendering.h"

void updateUIState(UIState *currentUIState) {
  UIState state = *currentUIState;
  if (IsKeyPressed(KEY_L)) {
    if (state == FILE_PANE_SELECTION) {
      state = INSIDE_FILE_PANE;
    }
  } else if (IsKeyPressed(KEY_H) && state == INSIDE_FILE_PANE) {
    state = FILE_PANE_SELECTION;
  } else if (IsKeyPressed(KEY_J) && state == FILE_PANE_SELECTION) {
    state = VISUALIZER_PANE_SELECTION;
  } else if (IsKeyPressed(KEY_K) && state == VISUALIZER_PANE_SELECTION) {
    state = FILE_PANE_SELECTION;
  }

  *currentUIState = state;
  DrawRectangle(0, 0, 200, HEIGHT, GRAY);
  if (state == FILE_PANE_SELECTION) {
    DrawRectangle(0, 0, 200, 100, BLUE);
  } else if (state == VISUALIZER_PANE_SELECTION) {
    DrawRectangle(0, 100, 200, 100, BLUE);
  }

  DrawText("FilePane", 75, 50, 15, RED);
  DrawText("Visualizer", 75, 150, 15, RED);
}
