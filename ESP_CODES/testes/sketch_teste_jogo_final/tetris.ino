// ===== MATRIZ SCORE (5 colunas x 9 altura) =====
#define SCORE_W 5
#define SCORE_H 9
#define SCORE_X 0  // canto esquerdo
#define SCORE_Y 6  // inicia na linha 6 da matriz 16x16

// ---------- CONFIG -----------------
#define T_WIDTH 10
#define T_HEIGHT 16
#define OFFSET_X 6  // deslocamento horizontal na matriz 16x16
unsigned long lastFall = 0;
int fallSpeed = 350;

bool tetrisStarted = false;
bool gameOver = false;

// ---------- SCORE -------------
int score = 0;

// ---------- TABULEIRO -------------
CRGB tetrisBoard[T_HEIGHT][T_WIDTH];

// ----------- PEÇAS ----------------
struct Piece {
  int shape[4][2];
  CRGB color;
} currentPiece, nextPiece;

// ---- Formas e cores padrão ----
Piece pieces[7] = {
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 3, 0 } }, CRGB::Cyan },    // I
  { { { 2, 0 }, { 1, 0 }, { 0, 0 }, { 0, 1 } }, CRGB::Blue },    // J
  { { { 0, 0 }, { 1, 0 }, { 2, 0 }, { 2, 1 } }, CRGB::Orange },  // L
  { { { 0, 0 }, { 0, 1 }, { 1, 0 }, { 1, 1 } }, CRGB::Yellow },  // O
  { { { 0, 1 }, { 1, 1 }, { 1, 0 }, { 2, 0 } }, CRGB::Green },   // S
  { { { 1, 0 }, { 0, 1 }, { 1, 1 }, { 2, 1 } }, CRGB::Purple },  // T
  { { { 0, 0 }, { 1, 0 }, { 1, 1 }, { 2, 1 } }, CRGB::Red }      // Z
};

// ------------ SCORE 5 dígitos ------------
CRGB coresScore[5] = {
  CRGB::Red,
  CRGB(255, 100, 0),  // Laranja
  CRGB::Green,
  CRGB::Blue,
  CRGB::Purple
};

// ------------ Inicializar ------------
void tetrisInit() {
  for (int y = 0; y < T_HEIGHT; y++)
    for (int x = 0; x < T_WIDTH; x++)
      tetrisBoard[y][x] = CRGB::Black;

  currentPiece = pieces[random(0, 7)];
  nextPiece = pieces[random(0, 7)];

  px = 3;
  py = 0;
  lastFall = millis();
  score = 0;
  tetrisStarted = true;
}

// ------------ PROXIMA PECA ------------
void desenharProximaPeca() {
  for (int i = 0; i < 4; i++) {
    int tx = nextPiece.shape[i][0];
    int ty = nextPiece.shape[i][1];
    leds[xy(tx + 1, ty + 1)] = nextPiece.color;
  }
}

// ------------ SCORE EM SUBMATRIZ 5x9 ------------
void desenharScore() {
  int valor = score;
  int digitos[5];

  for (int i = 4; i >= 0; i--) {
    digitos[i] = valor % 10;
    valor /= 10;
  }

  for (int x = 0; x < 5; x++)
    for (int y = 0; y < 9; y++)
      leds[xy(x, SCORE_Y + y)] = CRGB::Black;

  for (int col = 0; col < 5; col++) {
    int h = digitos[col];
    CRGB cor = coresScore[col];
    for (int y = 0; y < h; y++)
      leds[xy(col, SCORE_Y + (9 - 1 - y))] = cor;
  }
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

// ------------ Fixar Peça ------------
void fixaPeca() {
  for (int i = 0; i < 4; i++) {
    int tx = px + currentPiece.shape[i][0];
    int ty = py + currentPiece.shape[i][1];
    if (ty >= 0)
      tetrisBoard[ty][tx] = currentPiece.color;
  }
  spawnNovaPeca();  // chama a função que trata game over
}

// ------------ QUEDA ------------
void cair() {
  if (millis() - lastFall > fallSpeed) {
    if (!colide(px, py + 1)) py++;
    else {
      fixaPeca();
      checkLinhas();
    }
    lastFall = millis();
  }
}

// ------------ Controles ------------
unsigned long lastMoveJoy = 0;
int moveDelay = 100;

void controlarTetris() {
  unsigned long agora = millis();

  if (directionJoy != "center" && agora - lastMoveJoy > moveDelay) {

    if (directionJoy == "left" && !colide(px - 1, py)) px--;
    if (directionJoy == "right" && !colide(px + 1, py)) px++;
    if (directionJoy == "down") fallSpeed = 80;

    lastMoveJoy = agora;
  }

  if (directionJoy == "center") fallSpeed = 350;
}

// ------------ DESENHO ------------
void desenharTetris() {
  fill_solid(leds, 256, CRGB::Black);

  for (int y = 0; y < 16; y++) {
    leds[xy(OFFSET_X - 1, y)] = CRGB::White;
    leds[xy(OFFSET_X + T_WIDTH, y)] = CRGB::White;
  }

  for (int y = 0; y < T_HEIGHT; y++) {
    for (int x = 0; x < T_WIDTH; x++) {
      if (tetrisBoard[y][x] != CRGB::Black)
        leds[xy(x + OFFSET_X, y)] = tetrisBoard[y][x];
    }
  }

  for (int i = 0; i < 4; i++) {
    int tx = px + currentPiece.shape[i][0];
    int ty = py + currentPiece.shape[i][1];
    if (ty >= 0)
      leds[xy(tx + OFFSET_X, ty)] = currentPiece.color;
  }

  desenharProximaPeca();
  desenharScore();

  FastLED.show();
}

// ------------ ROTACIONAR ------------
void rotacionarPeca() {
  Piece backup = currentPiece;

  for (int i = 0; i < 4; i++) {
    int x = currentPiece.shape[i][0];
    int y = currentPiece.shape[i][1];
    currentPiece.shape[i][0] = y;
    currentPiece.shape[i][1] = -x;
  }

  if (colide(px, py)) currentPiece = backup;
}

// ------------ CHECAR LINHAS ------------
void piscarLinhas(int linhas[], int qtd) {
  for (int i = 0; i < 3; i++) {  // pisca 3 vezes
    // acender todas as linhas
    for (int l = 0; l < qtd; l++) {
      int y = linhas[l];
      for (int x = 0; x < T_WIDTH; x++)
        leds[xy(x + OFFSET_X, y)] = CRGB::White;
    }
    FastLED.show();
    delay(100);

    // restaurar cores originais
    for (int l = 0; l < qtd; l++) {
      int y = linhas[l];
      for (int x = 0; x < T_WIDTH; x++)
        leds[xy(x + OFFSET_X, y)] = tetrisBoard[y][x];
    }
    FastLED.show();
    delay(100);
  }
}


void checkLinhas() {
  int linhasParaLimpar[4];  // máximo 4 linhas de uma vez
  int qtdLinhas = 0;

  for (int y = T_HEIGHT - 1; y >= 0; y--) {
    bool linhaCompleta = true;
    for (int x = 0; x < T_WIDTH; x++) {
      if (tetrisBoard[y][x] == CRGB::Black) {
        linhaCompleta = false;
        break;
      }
    }

    if (linhaCompleta) {
      linhasParaLimpar[qtdLinhas++] = y;  // adiciona à lista
    }
  }

  // animação de todas as linhas juntas
  if (qtdLinhas > 0) {
    piscarLinhas(linhasParaLimpar, qtdLinhas);

    // remover linhas
    for (int i = 0; i < qtdLinhas; i++) {
      int y = linhasParaLimpar[i];
      for (int yy = y; yy > 0; yy--)
        for (int x = 0; x < T_WIDTH; x++)
          tetrisBoard[yy][x] = tetrisBoard[yy - 1][x];
      for (int x = 0; x < T_WIDTH; x++)
        tetrisBoard[0][x] = CRGB::Black;
    }

    // pontuação
    int pontosTabela[5] = { 0, 40, 100, 300, 1200 };
    if (qtdLinhas <= 4) score += pontosTabela[qtdLinhas];
  }
}

void spawnNovaPeca() {
  currentPiece = nextPiece;
  nextPiece = pieces[random(0, 7)];

  px = 3;
  py = 0;

  if (colide(px, py)) {
    gameOver = true;  // jogo acabou
  }
}

// ------------ SCORE CENTRALIZADO LIMITADO A 9999 ------------
// PRECISO REAJUSTAR ALGUMAS COISAS
void desenharScoreCentralizado() {
  fill_solid(leds, 256, CRGB::Black);

  // Limita o score a 9999
  int valor = (score > 9999) ? 9999 : score;

  // Converte score em dígitos
  int digitos[4] = { 0, 0, 0, 0 };
  int n = 0;
  while (valor > 0 && n < 4) {
    digitos[3 - n] = valor % 10;
    valor /= 10;
    n++;
  }

  // Posição inicial para centralizar na matriz 16x16
  int startX = (16 - 4 * 3) / 2;  // cada dígito ocupa 3 colunas
  int startY = (16 - 5) / 2;      // cada dígito ocupa 5 linhas

  // Digitos em matriz 3x5
  const byte numeros[10][5] = {
    { 0b111, 0b101, 0b101, 0b101, 0b111 },  //0
    { 0b010, 0b110, 0b010, 0b010, 0b111 },  //1
    { 0b111, 0b001, 0b111, 0b100, 0b111 },  //2
    { 0b111, 0b001, 0b111, 0b001, 0b111 },  //3
    { 0b101, 0b101, 0b111, 0b001, 0b001 },  //4
    { 0b111, 0b100, 0b111, 0b001, 0b111 },  //5
    { 0b111, 0b100, 0b111, 0b101, 0b111 },  //6
    { 0b111, 0b001, 0b010, 0b100, 0b100 },  //7
    { 0b111, 0b101, 0b111, 0b101, 0b111 },  //8
    { 0b111, 0b101, 0b111, 0b001, 0b111 }   //9
  };

  // Cores para cada dígito (4 dígitos)
  CRGB coresDigit[4] = { CRGB::Blue, CRGB::Green, CRGB::Yellow, CRGB::Red };

  for (int d = 0; d < 4; d++) {  // cada dígito
    CRGB cor = coresDigit[d];    // cor específica para este dígito
    for (int row = 0; row < 5; row++) {
      for (int col = 0; col < 3; col++) {
        if (numeros[digitos[d]][row] & (0b100 >> col)) {
          leds[xy(startX + d * 3 + col, startY + row)] = cor;
        }
      }
    }
  }

  FastLED.show();
}

void reiniciarJogo() {
  gameOver = false;
  tetrisStarted = false;  // tetrisLoop() vai chamar tetrisInit() automaticamente
  score = 0;
  fill_solid(leds, 256, CRGB::Black);  // limpa LEDs
  FastLED.show();
}

// ------------ LOOP PRINCIPAL ------------
void tetrisLoop() {
  if (!tetrisStarted) tetrisInit();

  if (!gameOver) {
    controlarTetris();
    cair();
    desenharTetris();
    if (botaoClique) rotacionarPeca();
  } else {
    desenharScoreCentralizado();  // exibe score centralizado quando game over
    delay(3000);
    reiniciarJogo();
  }
}