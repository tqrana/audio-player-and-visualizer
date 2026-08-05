#include <complex.h>
#include <stdio.h>

#include "raylib.h"
#define BUFFER_SIZE 512
#define HEIGHT 600

void dFT(float audioBufferValue) {
  static int bufferLocation = 0;
  static float complex x_n_summation = 0;
  static float currentBufferValues[512];
  currentBufferValues[bufferLocation] = audioBufferValue;
  if (bufferLocation == BUFFER_SIZE) {
    for (int i = 0; i < BUFFER_SIZE; i++) {
      float complex z1 = 0;
      for (int j = 0; j < BUFFER_SIZE; j++) {
        z1 += (currentBufferValues[j]) *
              (cexp(-I * 2 * PI * (i / (double)BUFFER_SIZE) * j));
        // https://en.wikipedia.org/wiki/Discrete_Fourier_transfor
      }
       int realBarHeight = (int)(20 * crealf(z1));
      realBarHeight = (realBarHeight <= 450) ? realBarHeight : 450;
      // 900 - 200 = 700, 700/2 = 350, 512/2 = 256, 350+200-256=294
      DrawRectangle(294 + i, HEIGHT - realBarHeight, 2, realBarHeight, RED);
    }
  }
  bufferLocation++;

  bufferLocation = (bufferLocation <= BUFFER_SIZE) ? bufferLocation : 0;
  x_n_summation = (bufferLocation <= 512) ? x_n_summation : 0;
}
