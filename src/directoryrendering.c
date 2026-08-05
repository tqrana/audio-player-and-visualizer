#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include "raylib.h"
#include "stdlib.h"

void updateInsidePane(int *indexFileSelected,
                      char **fileNames, bool *musicPlaying, Music *music,
                      int *numFile) {
  // int num_files = scandir(".", &list, NULL, alphasort);
  // char **fileNames = malloc(num_files * sizeof(char *));
  bool emptyDir = false;
  //  if (IsKeyPressed)
  //    if (dr == NULL) {
  //      DrawText("NO FILES FOUND", 500, 300, 30, RED);
  //      emptyDir = true;
  //    } else {
  //      for (int i = 0; i < num_files; i++) {
  //        fileNames[i] = list[i]->d_name;
  //      }
  //      char *fileSelected = fileNames[0];
  //    }

  if (IsKeyPressed(KEY_J)) {
    if ((*indexFileSelected + 1) <= *numFile) {
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
  // IMPORTANT
  // DrawText(fileNames[2], 300, 400, 60, BLUE);
  if (!emptyDir) {
    for (int i = *indexFileSelected; i > (*indexFileSelected - 3); i--) {
      if (i >= 0) {
        DrawText(fileNames[i], 300, 300 - 40 * (*indexFileSelected - i), 20, BLUE);
      }
    }
    for (int i = *indexFileSelected; i < (*indexFileSelected + 3); i++) {
      if (i <= (*numFile)) {
        DrawText(fileNames[i], 300, 40 * (i - *indexFileSelected) + 300, 20, BLUE);
      }
    }
    DrawText(fileNames[*indexFileSelected], 300, 300, 20, RED);
  }
}
