#include <complex.h>

#include "raylib.h"

#define BUFFER_SIZE 512
#define PI_CONSTANT 3.1415

void discreteFourierTransform(float *audioBuffer) {
  // DrawRectangle(0, 0, 200, 900, GRAY);
  for (int i = 0; i < BUFFER_SIZE; i++) {
    float complex z1 = 0;
    for (int j = 0; j < BUFFER_SIZE; j++) {
      z1 +=
          ((float complex)audioBuffer[j]) *
          (cexp(
              -I * 2 * PI_CONSTANT * (i / (double)BUFFER_SIZE) *
              j));  // for formula see:
                    // https://en.wikipedia.org/wiki/Discrete_Fourier_transform
    }
    int height = (int)(20 * crealf(z1));
    if (height > 450) {
      height = 450;
    }
    int y_pos = 450 - height;
    DrawRectangle(250 + i, y_pos, 2, height, RED);
  }
}
