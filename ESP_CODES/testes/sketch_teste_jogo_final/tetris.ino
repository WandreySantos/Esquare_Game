// ---------- CONFIG -----------------
#define T_WIDTH 10
#define T_HEIGHT 16
#define OFFSET_X 3  // deslocamento horizontal na matriz 16x16
unsigned long lastFall = 0;
int fallSpeed = 350;  // velocidade normal

bool tetrisStarted = false;

// ---------- TABULEIRO -------------
CRGB tetrisBoard[T_HEIGHT][T_WIDTH];

// ----------- PEÇAS ----------------
struct Piece {
  int shape[4][2];
  CRGB color;
} currentPiece;

// ---- Formas ----
Piece pieces[7] = {
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } } },  // I
  { { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } } },  // O
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 } } },  // L
  { { { 2, 0 }, { 1, 0 }, { 0, 0 }, { 0, 1 } } },  // J
  { { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 2, 0 } } },  // S
  { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } } },  // Z
  { { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } } }   // T
};


CRGB coresDisponiveis[] = {
  ROSA_ROXO,
  VERDE_LIMAO,
  MARROM_ESCURO,
  VERMELHO,
  AZUL_PETROLEO,
  VERDE_MUSGO,
  CIANO_CLARO,
  AZUL,
  AZUL_FORTE,
  AMARELO_CLARO,
  AZUL_ESCURO
};

int totalCores = sizeof(coresDisponiveis) / sizeof(coresDisponiveis[0]);

// ------------ Inicializar ------------
void tetrisInit() {
  for (int y = 0; y < T_HEIGHT; y++)
    for (int x = 0; x < T_WIDTH; x++)
      tetrisBoard[y][x] = CRGB::Black;

  currentPiece = pieces[random(0, 7)];
  currentPiece.color = coresDisponiveis[random(0, totalCores)];

  px = 3;
  py = 0;
  lastFall = millis();
  tetrisStarted = true;
}

// ------------ Colisão ------------
bool colide(int nx, int ny) {
  for (int i = 0; i < 4; i++) {
    int tx = nx + currentPiece.shape[i][0];
    int ty = ny + currentPiece.shape[i][1];

    if (tx < 0 || tx >= T_WIDTH || ty >= T_HEIGHT) return true;
    if (ty >= 0 && tetrisBoard[ty][tx] != CRGB::Black) return true;
  }
  return false;
}

// ------------ Fixar ------------
void fixaPeca() {
  for (int i = 0; i < 4; i++) {
    int tx = px + currentPiece.shape[i][0];
    int ty = py + currentPiece.shape[i][1];
    if (ty >= 0)
      tetrisBoard[ty][tx] = currentPiece.color;
  }

  currentPiece = pieces[random(0, 7)];
  currentPiece.color = coresDisponiveis[random(0, totalCores)];
  px = 3;
  py = 0;

  if (colide(px, py)) tetrisInit();  // restart se travar
}

// ------------ Queda ------------
void cair() {
  if (millis() - lastFall > fallSpeed) {
    if (!colide(px, py + 1)) py++;
    else {
      fixaPeca();
       ();  // Novo!
    }
    lastFall = millis();
  }
}

// ------------ Controles ------------
void controlarTetris() {
  if (directionJoy == "left" && !colide(px - 1, py)) px--;
  if (directionJoy == "right" && !colide(px + 1, py)) px++;
  fallSpeed = (directionJoy == "down") ? 80 : 350;
}

// ------------ Desenhar ------------
void desenharTetris() {
  fill_solid(leds, 256, CRGB::Black);

  // paredes azuis
  for (int y = 0; y < 16; y++) {
    leds[xy(OFFSET_X - 1, y)] = CRGB::Blue;
    leds[xy(OFFSET_X + T_WIDTH, y)] = CRGB::Blue;
  }

  // tabuleiro fixo
  for (int y = 0; y < T_HEIGHT; y++) {
    for (int x = 0; x < T_WIDTH; x++) {
      if (tetrisBoard[y][x] != CRGB::Black)
        leds[xy(x + OFFSET_X, y)] = tetrisBoard[y][x];
    }
  }

  // peça caindo
  for (int i = 0; i < 4; i++) {
    int tx = px + currentPiece.shape[i][0];
    int ty = py + currentPiece.shape[i][1];
    if (ty >= 0)
      leds[xy(tx + OFFSET_X, ty)] = currentPiece.color;
  }

  FastLED.show();
}
void rotacionarPeca() {
  Piece backup = currentPiece;  // salva original

  for (int i = 0; i < 4; i++) {
    int x = currentPiece.shape[i][0];
    int y = currentPiece.shape[i][1];
    currentPiece.shape[i][0] = y;
    currentPiece.shape[i][1] = -x;
  }

  if (colide(px, py)) {
    currentPiece = backup;  // desfaz se colidir
  }
}
void checkLinhas() {
  for (int y = T_HEIGHT - 1; y >= 0; y--) {
    bool linhaCheia = true;

    // verifica se a linha está completa
    for (int x = 0; x < T_WIDTH; x++) {
      if (tetrisBoard[y][x] == CRGB::Black) {
        linhaCheia = false;
        break;
      }
    }

    // Se estiver cheia → apaga e puxa tudo
    if (linhaCheia) {
      for (int ty = y; ty > 0; ty--) {
        for (int tx = 0; tx < T_WIDTH; tx++) {
          tetrisBoard[ty][tx] = tetrisBoard[ty - 1][tx];
        }
      }

      // linha 0 (topo) vira preta
      for (int tx = 0; tx < T_WIDTH; tx++) {
        tetrisBoard[0][tx] = CRGB::Black;
      }

      y++;  // reprocessar mesma linha após shift
    }
  }
}

// ------------ Loop Principal ------------
void tetrisLoop() {
  if (!tetrisStarted) tetrisInit();

  controlarTetris();
  cair();
  desenharTetris();
  if (botaoClique) rotacionarPeca();
}
