#ifndef UI_RENDERING
#define UI_RENDERING

#include "raylib.h"

typedef enum {
  FILE_PANE_SELECTION,
  INSIDE_FILE_PANE,
  VISUALIZER_PANE_SELECTION,
} UIState;

void updateUIState(UIState *);
#endif
