#include <dirent.h>
#include <stdatomic.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#if defined(_WIN32)
#include <conio.h>  // Windows only, no stardard library
#else
// Required for kbhit() function in non-Windows platforms
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#endif

// #include "audio_playback.h"
#include "directoryrendering.h"
#include "math_visualization.h"
#include "raylib.h"
#include "uirendering.h"
#define WIDTH 1000
#define HEIGHT 1000

#define BUFFER_SIZE 512

static atomic_int visualizerActive = 0;

void setVisualizerActive(bool isActive) {
  atomic_store(&visualizerActive, ((isActive == true) ? 1 : 0));
}

void callback(void *bufferData, unsigned int frames) {
  if (!(atomic_load(&visualizerActive) == 1)) return;
  float (*audioBuffer)[2] = bufferData;
  for (int i = 0; i < 512; i++) {
    dFT(audioBuffer[i][0]);
  }
}

void updateInsidePane(int *indexFileSelected,
                      char **fileNames, bool *musicPlaying, Music *music,
                      int *numFile) {
  bool emptyDir = false;

  if (IsKeyPressed(KEY_J)) {
    if ((*indexFileSelected + 1) <= (*numFile - 1)) {
      *indexFileSelected = *indexFileSelected + 1;
    }
  }
  if (IsKeyPressed(KEY_K)) {
    if ((*indexFileSelected - 1) >= 0) {
      *indexFileSelected = *indexFileSelected - 1;
    }
  }
  if (IsKeyPressed(KEY_ENTER) && !emptyDir) {
    if (strstr(fileNames[*indexFileSelected], ".mp3") != NULL ||
        strstr(fileNames[*indexFileSelected], ".wav") != NULL ||
        strstr(fileNames[*indexFileSelected], ".ogg") != NULL) {
      if (*musicPlaying) StopMusicStream(*music);
      *music = LoadMusicStream(fileNames[*indexFileSelected]);
      AttachAudioStreamProcessor((*music).stream, callback);
      *musicPlaying = true;
      WaitTime(0.1);

      PlayMusicStream(*music);
    }
  } else if (IsKeyPressed(KEY_R)) {
    struct dirent **new;
    int num_files = scandir(".", &new, NULL, alphasort);
    char **newFileNames = malloc(num_files * sizeof(char *));
    int offset = num_files - *numFile;
    fileNames =
        (char **)realloc(fileNames, (*numFile + offset) * sizeof(char *));
    if (num_files != 0) {
      for (int i = 0; i < num_files; i++) {
        fileNames[i] = new[i]->d_name;
      }
    }

    *numFile = num_files;
  }
  if (!emptyDir) {
    for (int i = *indexFileSelected; i > (*indexFileSelected - 3); i--) {
      if (i >= 0) {
        DrawText(fileNames[i], 300, 300 - 40 * (*indexFileSelected - i), 20, BLUE);
      }
    }
    for (int i = *indexFileSelected; i < (*indexFileSelected + 3); i++) {
      if (i <= (*numFile - 1)) {
        DrawText(fileNames[i], 300, 40 * (i - *indexFileSelected) + 300, 20, BLUE);
      }
    }
    DrawText(fileNames[*indexFileSelected], 300, 300, 20, RED);
  }
}


int main(int argc, char *argv[]) {
  static unsigned char key = 0;
  InitWindow(WIDTH, HEIGHT, "Audio Player");
  SetTargetFPS(60);
  InitAudioDevice();

  Music music;
  struct dirent **list;
  int num_files = scandir(".", &list, NULL, alphasort);
  char **fileNames = malloc(num_files * sizeof(char *));
  DIR *dr = opendir(".");
  bool emptyDir = false;
  if (dr == NULL) {
    DrawText("NO FILES FOUND", 500, 300, 30, RED);
    emptyDir = true;
  }

  for (int i = 0; i < num_files; i++) {
    fileNames[i] = list[i]->d_name;
  }
  int indexFileSelected = 0;
  // PlayMusicStream(music);
  UIState state = INSIDE_FILE_PANE;
  bool musicPlaying = false;
  bool isPaused = false;
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);
    if (musicPlaying) {
      UpdateMusicStream(music);
    }
    updateUIState(&state);
    if (state == INSIDE_FILE_PANE) {
      updateInsidePane(&indexFileSelected, fileNames,
                       &musicPlaying, &music, &num_files);
    }

    if (IsKeyPressed(KEY_SPACE)) {
        if (musicPlaying == true && isPaused == false) {
            PauseMusicStream(music);
            isPaused = true;
        } else if (musicPlaying == true && isPaused == true) {
           ResumeMusicStream(music);
           isPaused = false;
        }
    }

    setVisualizerActive(state == VISUALIZER_PANE_SELECTION);

    EndDrawing();
  }
  free(fileNames);
  UnloadMusicStream(music);
  CloseAudioDevice();
  CloseWindow();
  closedir(dr);
  return 0;
}
