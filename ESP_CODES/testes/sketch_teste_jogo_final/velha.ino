// ---- Funções ----
void drawVelhaBoard() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      leds[xy(x, y)] = velhaboard[y * WIDTH + x];
    }
  }
  FastLED.show();
}

void confirmarJogada(int x0, int y0, bool turno) {
  const CRGB* sprite = turno ? criaX : criaO;

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      velhaboard[(y0 + y) * WIDTH + (x0 + x)] = sprite[y * 4 + x];
    }
  }

  drawVelhaBoard();  // redesenha já confirmado
}

void previewJogada(int x0, int y0, bool turno) {
  const CRGB* sprite = turno ? criaX : criaO;

  drawVelhaBoard(); // redesenha o board real

  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      leds[xy(x0 + x, y0 + y)] = sprite[y * 4 + x];
    }
  }
  FastLED.show();
}

void startVelhaGame() {
  FastLED.clear();
  drawVelhaBoard();  // apenas mostra matriz limpa

  if (!botaoClique) {
    confirmarJogada(xvelha, yvelha, turno);
    turno = !turno;
  }
}
