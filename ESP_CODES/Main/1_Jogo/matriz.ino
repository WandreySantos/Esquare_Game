int board[3][3] = {
  {0, 0, 0},
  {0, 0, 0},
  {0, 0, 0}
};

// -------------------- MAPA LED --------------------
int XY(int x, int y) {
  if (x % 2 == 0)
    return x * HEIGHT + y;
  else
    return x * HEIGHT + (HEIGHT - 1 - y);
}

void handleMatrix() {
  String json = "{\"matrix\":[";

  // ----- Estado dos LEDs -----
  for (int y = 0; y < HEIGHT; y++) {
    json += "[";
    for (int x = 0; x < WIDTH; x++) {
      CRGB c = leds[XY(x, y)];
      json += "[" + String(c.r) + "," + String(c.g) + "," + String(c.b) + "]";
      if (x < WIDTH - 1) json += ",";
    }
    json += "]";
    if (y < HEIGHT - 1) json += ",";
  }
  json += "]";

  // ----- Se o jogo atual for o da velha -----
  if (currentGame == "velha") {
    json += ",\"board\":[";
    for (int r = 0; r < 3; r++) {
      json += "[";
      for (int c = 0; c < 3; c++) {
        json += String(board[r][c]);
        if (c < 2) json += ",";
      }
      json += "]";
      if (r < 2) json += ",";
    }
    json += "]";
  }

  json += "}"; // fecha JSON final

  server.send(200, "application/json", json);
}

// ===================================================================================================================
// -------------------------------------SNAKE-------------------------------------------------------------------------

// -------------------- CHECAGEM DE COLISÃO --------------------
bool checkCollision() {
  for (int i = 1; i < snakeLength; i++)
    if (snake[0].x == snake[i].x && snake[0].y == snake[i].y)
      return true;
  return false;
}

// -------------------- ANIMAÇÃO DE MORTE --------------------
void deathAnimation() {
  for (int i = snakeLength - 1; i >= 0; i--) {
    leds[XY(snake[i].x, snake[i].y)] = CRGB::Red;
    FastLED.show();
    delay(80);
    leds[XY(snake[i].x, snake[i].y)] = CRGB::Black;
  }
  FastLED.clear();
  start_game = true;
}

// --------------------DESENHA COBRA --------------------
void drawSnake() {
  FastLED.clear();
  leds[XY(food.x, food.y)] = CRGB::Red;
  for (int i = 0; i < snakeLength; i++)
    leds[XY(snake[i].x, snake[i].y)] = (i == 0) ? CRGB::White : CRGB(0, 100, 0);
  FastLED.show();
}

