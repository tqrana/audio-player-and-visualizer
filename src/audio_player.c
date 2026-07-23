#define _XOPEN_SOURCE 700
#include <dirent.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <time.h>

#include "math_visualization.h"

#if defined(_WIN32)
#include <conio.h>  // Windows only, no stardard library
#else
// Required for kbhit() function in non-Windows platforms
#include <fcntl.h>
#include <stdio.h>
#include <termios.h>
#include <unistd.h>
#endif

#include "raylib.h"

#define KEY_ESCAPE 27
#define WIDTH 1500
#define HEIGHT 600
#define SONGTITLESPACING 100

#define BUFFER_SIZE 512

float frameArray[BUFFER_SIZE] = {0};

void callback(void *bufferData, unsigned int frames) {
  float *audioBuffer = (float *)bufferData;
  for (int i = 0; i < BUFFER_SIZE; i++) {
    frameArray[i] = (audioBuffer[i * 2] + audioBuffer[i * 2 + 1]) / 2;
  }
  // discreteFourierTransform(frameArray);
}
void obtainMusicFiles(char **fileArray, struct dirent **list, int count) {
  if (count < 0) {
    perror("Couldn't open the directory");
    exit(1);
  }
  printf("%u items in directory\n", count);
  for (int i = 0; i < count; i++) {
    fileArray[i] = list[i]->d_name;
  }
}

void printFilePane(char **fileArray, int num_files, int file_currently_at) {
  int y_pos = 20;
  for (int i = 2; i < num_files; i++) {
    if (i == file_currently_at) {
      DrawText(fileArray[i], 250, y_pos, 18, RED);
    } else {
      DrawText(fileArray[i], 250, y_pos, 18, BLACK);
    }
    y_pos += SONGTITLESPACING;
  }
}

void displayLeftPaneText() {
  DrawText("Directory", 75, 50, 15, BLACK);
  DrawText("Now Playing", 75, 150, 15, BLACK);
  DrawText("Visualizer", 75, 250, 15, BLACK);
}

void displayCurrentLeftPane(char *leftPaneState) {
  DrawRectangle(0, 0, 200, HEIGHT, GRAY);
  if (strcmp(leftPaneState, "hoverOnDirectory") == 0) {
    DrawRectangle(0, 0, 200, HEIGHT, GRAY);
    DrawRectangle(0, 0, 200, 100, BLUE);
    displayLeftPaneText();
  } else if (strcmp(leftPaneState, "hoverOnNowPlaying") == 0) {
    DrawRectangle(0, 0, 200, HEIGHT, GRAY);
    DrawRectangle(0, 100, 200, 100, BLUE);
    displayLeftPaneText();
  } else if (strcmp(leftPaneState, "hoverOnVisualizer") == 0) {
    DrawRectangle(0, 0, 200, HEIGHT, GRAY);
    DrawRectangle(0, 200, 200, 100, BLUE);
    displayLeftPaneText();
  }
  displayLeftPaneText();
}

void displayNowPlaying() {}
void displayVisualizer() {}

// change such that malloc is done after knowing directory size
int main(int argc, char *argv[]) {
  DIR *dp;
  struct dirent **list;
  int num_files = scandir(".", &list, NULL, alphasort);
  char **array = (char **)malloc(num_files * sizeof(char[1024]));
  obtainMusicFiles(array, list, num_files);
  printf("%s\n", array[1]);
  static unsigned char key = 0;
  InitWindow(WIDTH, HEIGHT, "Audio Player");
  SetTargetFPS(60);
  InitAudioDevice();

  // char *uiState[] = {"rightMainGUI", "leftPane"};
  //
  char *currentState = "insideFileRightPane";
  char *rightPaneState =
      "file_pane";  // add into while loop once more panes are added
  char *leftPaneState = "insideRightPane";
  char *selectionInFilePane = "firstFileInDirectory";
  bool inWindowPane = false;  // might remove?
  int file_currently_at = 2;
  // Music music = LoadMusicStream((char *)array[4]);
  Music music = LoadMusicStream("YOUR_SONG.mp3");
  AttachAudioStreamProcessor(music.stream, callback);
  PlayMusicStream(music);
  while (!WindowShouldClose()) {
    BeginDrawing();
    ClearBackground(WHITE);

    DrawRectangle(0, 0, 200, HEIGHT, GRAY);

    // discreteFourierTransform(frameArray);
    UpdateMusicStream(music);
    // displayLeftPaneText();
    if (IsKeyPressed(KEY_H) &&
        (strcmp(currentState, "insideFileRightPane") == 0) &&
        inWindowPane == false) {
      printFilePane(array, num_files, -1);
      displayCurrentLeftPane(leftPaneState);
      currentState = "leftPane";
      leftPaneState = "hoverOnDirectory";
      inWindowPane = true;
    } else if (IsKeyPressed(KEY_L) && (strcmp(currentState, "leftPane") == 0)) {
      if (strcmp(leftPaneState, "hoverOnDirectory") == 0) {
        printFilePane(array, num_files, file_currently_at);
        currentState = "insideFileRightPane";
        leftPaneState = "insideRightPane";
      } else if (strcmp(leftPaneState, "hoverOnNowPlaying") == 0) {
        displayNowPlaying();
        currentState = "insideFileRightPane";
        leftPaneState = "insideRightPane";
      } else if (strcmp(leftPaneState, "hoverOnVisualizer") == 0) {
        displayVisualizer();
        currentState = "insideFileRightPane";
        leftPaneState = "insideRightPane";
      }
      displayCurrentLeftPane(leftPaneState);
      inWindowPane = false;
    } else if (IsKeyPressed(KEY_J) &&
               (strcmp(currentState, "insideFileRightPane") == 0)) {
      if ((file_currently_at + 1) < num_files) {
        file_currently_at++;
        printFilePane(array, num_files, file_currently_at);
      } else {
        printFilePane(array, num_files, file_currently_at);
      }
    } else if (IsKeyPressed(KEY_K) &&
               (strcmp(currentState, "insideFileRightPane") == 0)) {
      if ((file_currently_at - 1) >= 2) {
        file_currently_at = file_currently_at - 1;
        printFilePane(array, num_files, file_currently_at);
      } else {
        printFilePane(array, num_files, file_currently_at);
      }
    } else if (IsKeyPressed(KEY_J) && (strcmp(currentState, "leftPane") == 0)) {
      if (strcmp(leftPaneState, "hoverOnDirectory") == 0) {
        rightPaneState = "nowPlaying";
        leftPaneState = "hoverOnNowPlaying";
        displayCurrentLeftPane(leftPaneState);
      } else if (strcmp(leftPaneState, "hoverOnNowPlaying") == 0) {
        rightPaneState = "visualizer";
        leftPaneState = "hoverOnVisualizer";
        displayCurrentLeftPane(leftPaneState);
      }
    } else if (IsKeyPressed(KEY_K) && (strcmp(currentState, "leftPane") == 0)) {
      if (strcmp(leftPaneState, "hoverOnVisualizer") == 0) {
        rightPaneState = "nowPlaying";
        leftPaneState = "hoverOnNowPlaying";
        displayCurrentLeftPane(leftPaneState);
      } else if (strcmp(leftPaneState, "hoverOnNowPlaying") == 0) {
        rightPaneState = "file_pane";
        leftPaneState = "hoverOnDirectory";
        displayCurrentLeftPane(leftPaneState);
      }
    } else if (IsKeyPressed(KEY_ENTER) &&
               (strcmp(currentState, "insideFileRightPane") == 0)) {
      if (strstr(((char *)(array[file_currently_at])), ".mp3") != NULL) {
        StopMusicStream(music);
        Music new_music = LoadMusicStream((char *)array[file_currently_at]);
        music = new_music;
        AttachAudioStreamProcessor(music.stream, callback);
        WaitTime(0.35);
        PlayMusicStream(music);
      }
      printFilePane(array, num_files, file_currently_at);
    } else if (strcmp(currentState, "insideFileRightPane") == 0) {
      printFilePane(array, num_files, file_currently_at);
      displayCurrentLeftPane(leftPaneState);
    } else {
      displayCurrentLeftPane(leftPaneState);
      if (strcmp(rightPaneState, "file_pane") == 0) {
        printFilePane(array, num_files, -1);
      } else if (strcmp(leftPaneState, "hoverOnNowPlaying") == 0) {
        displayNowPlaying();
      } else if (strcmp(leftPaneState, "hoverOnVisualizer") == 0) {
        discreteFourierTransform(frameArray);
      }
    }
    EndDrawing();
  }

  UnloadMusicStream(music);
  CloseAudioDevice();
  CloseWindow();
  return 0;
}
