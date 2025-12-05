void drawBorder6x6(int x0, int y0, CRGB color) {
  const int size = 6;
  // topo
  for (int x = 0; x < size; x++) {
    leds[xy(x0 + x, y0 + 0)] = color;
  }
  // direita
  for (int y = 0; y < size; y++) {
    leds[xy(x0 + (size - 1), y0 + y)] = color;
  }
  // baixo
  for (int x = 0; x < size; x++) {
    leds[xy(x0 + x, y0 + (size - 1))] = color;
  }
  // esquerda
  for (int y = 0; y < size; y++) {
    leds[xy(x0 + 0, y0 + y)] = color;
  }
  FastLED.show();
}

// void drawBorder4x4(int x0, int y0, CRGB color) {
//   const int size = 4;
//   // topo
//   for (int x = 0; x < size; x++) {
//     leds[xy(x0 + x, y0 + 0)] = color;
//   }
//   // direita
//   for (int y = 0; y < size; y++) {
//     leds[xy(x0 + (size - 1), y0 + y)] = color;
//   }
//   // baixo
//   for (int x = 0; x < size; x++) {
//     leds[xy(x0 + x, y0 + (size - 1))] = color;
//   }
//   // esquerda
//   for (int y = 0; y < size; y++) {
//     leds[xy(x0 + 0, y0 + y)] = color;
//   }
//   FastLED.show();
// }
