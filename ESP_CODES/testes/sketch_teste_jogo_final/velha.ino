void drawVelhaBoard() {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      leds[xy(x, y)] = velhaboard[y * WIDTH + x];
    }
  }
  FastLED.show();
}
void previewSprite4x4(int x0, int y0, const CRGB* sprite) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      leds[xy(x0 + x, y0 + y)] = sprite[y * 4 + x];
    }
  }
}

int cellXFromPixel(int x) {
  return x / 5;  // 1→0 | 6→1 | 11→2
}
int cellYFromPixel(int y) {
  return y / 5;
}
bool casaLivreVelha(int cx, int cy) {
  return board[cy][cx] == 0;
}

void desenharSprite4x4(int x0, int y0, const CRGB* sprite) {
  for (int y = 0; y < 4; y++) {
    for (int x = 0; x < 4; x++) {
      velhaboard[(y0 + y) * WIDTH + (x0 + x)] = sprite[y * 4 + x];
    }
  }
}

void fazerJogadaVelha() {
  if (!botaoSoltoAgora()) return;

  int cx = cellXFromPixel(xvelha);
  int cy = cellYFromPixel(yvelha);

  if (!casaLivreVelha(cx, cy)) return;

  const CRGB* sprite = turno ? criaX : criaO;

  desenharSprite4x4(xvelha, yvelha, sprite);
  board[cy][cx] = turno ? 1 : 2;

  turno = !turno;
}


int checkWinner() {
  // linhas
  for (int i = 0; i < 3; i++) {
    if (board[i][0] != 0 && board[i][0] == board[i][1] && board[i][1] == board[i][2]) {
      return board[i][0];
    }
  }

  // colunas
  for (int i = 0; i < 3; i++) {
    if (board[0][i] != 0 && board[0][i] == board[1][i] && board[1][i] == board[2][i]) {
      return board[0][i];
    }
  }

  // diagonais
  if (board[0][0] != 0 && board[0][0] == board[1][1] && board[1][1] == board[2][2]) {
    return board[0][0];
  }

  if (board[0][2] != 0 && board[0][2] == board[1][1] && board[1][1] == board[2][0]) {
    return board[0][2];
  }

  // empate?
  bool cheio = true;
  for (int y = 0; y < 3; y++)
    for (int x = 0; x < 3; x++)
      if (board[y][x] == 0) cheio = false;

  if (cheio) return -1;

  return 0;
}

void mostrarResultado(int winner) {
  if (winner == 1) Serial.println("X venceu");
  else if (winner == 2) Serial.println("O venceu");
  else if (winner == -1) Serial.println("Empate");
}

void startVelhaGame() {

  // 1. Desenha estado fixo
  drawVelhaBoard();

  int cx = cellXFromPixel(xvelha);
  int cy = cellYFromPixel(yvelha);

  // 2. Preview (somente se casa estiver livre E botão não foi solto)
  if (casaLivreVelha(cx, cy) && !botaoSoltoAgora()) {
    const CRGB* sprite = turno ? criaX : criaO;
    previewSprite4x4(xvelha, yvelha, sprite);
    FastLED.show();
  }

  // 3. Confirma jogada
  fazerJogadaVelha();

  // 4. Verifica vitória
  int winner = checkWinner();
  if (winner != 0) {
    mostrarResultado(winner);
  }
}

