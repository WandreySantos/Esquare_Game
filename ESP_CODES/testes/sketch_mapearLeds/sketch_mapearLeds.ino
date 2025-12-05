#include <Arduino.h>
#include <FastLED.h>

#define DATA_PIN 25
#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS];
int brilho = 50;

// Função de mapeamento serpentina por coluna
int xy(int x, int y) {
  if (x % 2 == 0)
    return x * HEIGHT + y;
  else
    return x * HEIGHT + (HEIGHT - 1 - y);
}

void setup() {
  Serial.begin(115200);
  delay(500);

  // Inicialização FastLED
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brilho);
  FastLED.clear();
  FastLED.show();

  Serial.println("\nIniciando varredura da matriz 16x16...\n");

  // Percorre toda a matriz e acende LED por LED
  for (int x = 0; x < WIDTH; x++) {
    for (int y = 0; y < HEIGHT; y++) {

      int index = xy(x, y);

      // Imprime posição no Serial
      Serial.print("X: ");
      Serial.print(x);
      Serial.print(" | Y: ");
      Serial.print(y);
      Serial.print(" | LED Index: ");
      Serial.println(index);

      // Acende somente o LED atual (vermelho)
      FastLED.clear();
      leds[index] = CRGB::Red;
      FastLED.show();

      delay(50);
    }
  }

  Serial.println("\nFinalizado!");
}

void loop() {
  // nada aqui
}
