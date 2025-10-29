#include <FastLED.h>

#define DATA_PIN 25
#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)

// ====== PINOS DO CONTROLE ======
#define BTN_UP     33
#define BTN_DOWN   32
#define BTN_LEFT   27
#define BTN_RIGHT  26

int lastUp = HIGH;
int lastDown = HIGH;
int lastLeft = HIGH;
int lastRight = HIGH;

CRGB leds[NUM_LEDS];

// --- Função serpentina ---
int XY(int x, int y) {
  if (x % 2 == 0) return x * HEIGHT + y;
  else return x * HEIGHT + (HEIGHT - 1 - y);
}

// --- Cores ---
#define VERDE CRGB(20,255,29)
#define PRETO CRGB(0,0,0)
#define AMARELO CRGB(255,242,0)
#define VERMELHO CRGB(237,28,36)
#define BRANCO CRGB(255,255,255)
#define AZUL CRGB(77,109,243)
#define RED CRGB(255,0,0)

// --- Matriz original (exemplo simplificado, substitua pela sua) ---
const CRGB image16x16[16*16] = {
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, VERDE, BRANCO, PRETO, VERMELHO, PRETO, PRETO, BRANCO, PRETO, BRANCO, PRETO, BRANCO, PRETO, PRETO,
  PRETO, VERDE, PRETO, VERDE, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, BRANCO, PRETO, PRETO, PRETO,
  PRETO, VERDE, PRETO, VERDE, VERDE, PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, BRANCO, PRETO, BRANCO, PRETO, PRETO,
  PRETO, VERDE, VERDE, PRETO, VERDE, PRETO, PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, BRANCO,PRETO, PRETO, PRETO,
  PRETO, PRETO, VERDE, VERDE, VERDE, PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, BRANCO, PRETO, BRANCO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, AMARELO, AMARELO, AMARELO, CRGB(34,177,76), PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, PRETO, BRANCO, PRETO, PRETO, PRETO,
  PRETO, VERMELHO, VERMELHO, AMARELO, CRGB(34,177,76), PRETO, PRETO, PRETO, BRANCO, BRANCO, BRANCO, BRANCO, BRANCO, BRANCO, PRETO, PRETO,
  PRETO, AZUL, VERMELHO, VERMELHO, CRGB(34,177,76), PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, PRETO, BRANCO, PRETO, PRETO, PRETO,
  PRETO, AZUL, AZUL, VERDE, CRGB(34,177,76), PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, PRETO, BRANCO, PRETO, PRETO, PRETO,
  PRETO, AZUL, VERDE, VERDE, VERDE, PRETO, PRETO, PRETO, BRANCO, BRANCO, BRANCO, BRANCO, BRANCO, BRANCO, PRETO, PRETO,
  PRETO, CRGB(255,163,177), CRGB(255,163,177), CRGB(255,163,177), CRGB(255,163,177), PRETO, PRETO, PRETO, PRETO, BRANCO, PRETO, PRETO, BRANCO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
};

// Cursor
int cursorX = 0;          // coluna do cursor
int cursorY = 15;         // linha do cursor
const int linhaJogo1 = 7;
const int linhaJogo2 = 15;
const int barraWidth = 5;  // largura da barra
const int barraStep = 8;   // passo horizontal

void setup() {
  FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
  FastLED.clear();

  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  mostrarImagem();
}

void loop() {
  int btnUpState = digitalRead(BTN_UP);
  int btnDownState = digitalRead(BTN_DOWN);
  int btnLeftState = digitalRead(BTN_LEFT);
  int btnRightState = digitalRead(BTN_RIGHT);

  // Move verticalmente
  if (lastUp == HIGH && btnUpState == LOW) cursorY = linhaJogo1;
  if (lastDown == HIGH && btnDownState == LOW) cursorY = linhaJogo2;

  // Move horizontalmente
  if (lastLeft == HIGH && btnLeftState == LOW) {
    cursorX -= barraStep;
    if(cursorX < 0) cursorX = 0;
  }
  if (lastRight == HIGH && btnRightState == LOW) {
    cursorX += barraStep;
    if(cursorX > WIDTH - barraWidth) cursorX = WIDTH - barraWidth;
  }

  mostrarImagem();

  lastUp = btnUpState;
  lastDown = btnDownState;
  lastLeft = btnLeftState;
  lastRight = btnRightState;

  delay(100);
}

void mostrarImagem() {
  // Aplica a imagem original
  for(int y=0; y<HEIGHT; y++){
    for(int x=0; x<WIDTH; x++){
      leds[XY(x,y)] = image16x16[y*WIDTH + x];
    }
  }

  // Desenha a barra de 5 LEDs na linha atual
  for(int i=0; i<barraWidth; i++){
    leds[XY(cursorX + i, cursorY)] = RED;
  }

  FastLED.show();
}
