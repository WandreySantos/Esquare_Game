// -------------------- CONFIGURAÇÃO / VARIÁVEIS GERAIS --------------------
struct Point {
  int x;
  int y;
};

// -------------------- COBRA --------------------
Point snake[256];
int snakeLength = 3;
String direction = "right";       // "up","down","left","right" (interno)
String lastDirection = "right";

// -------------------- COMIDA --------------------
Point food;

// -------------------- GAMEPLAY --------------------
int gameSpeed = 150;
bool start_game = true;

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
    leds[xy(snake[i].x, snake[i].y)] = CRGB::Red;
    FastLED.show();
    delay(80);
    leds[xy(snake[i].x, snake[i].y)] = CRGB::Black;
  }
  FastLED.clear();
  start_game = true;
}

// -------------------- DESENHAR COBRA --------------------
void drawSnake() {
  FastLED.clear();
  leds[xy(food.x, food.y)] = CRGB::Red;
  for (int i = 0; i < snakeLength; i++)
    leds[xy(snake[i].x, snake[i].y)] = (i == 0) ? CRGB::White : CRGB(0, 100, 0);
  FastLED.show();
}

// -------------------- SPAWN DE COMIDA --------------------
void spawnFood() {
  bool valid = false;
  while (!valid) {
    food.x = random(0, WIDTH);
    food.y = random(0, HEIGHT);
    valid = true;
    for (int i = 0; i < snakeLength; i++) {
      if (snake[i].x == food.x && snake[i].y == food.y) {
        valid = false;
        break;
      }
    }
  }
}

// -------------------- CRIAR COBRA --------------------
void criarCobra() {
  snake[0] = { WIDTH / 2, HEIGHT / 2 };
  snake[1] = { WIDTH / 2 - 1, HEIGHT / 2 };
  snake[2] = { WIDTH / 2 - 2, HEIGHT / 2 };
  snakeLength = 3;
  direction = "right";
  lastDirection = "right";
}

// -------------------- MOVIMENTO --------------------
void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--)
    snake[i] = snake[i - 1];

  if (direction == "up") snake[0].y--;
  else if (direction == "down") snake[0].y++;
  else if (direction == "left") snake[0].x--;
  else if (direction == "right") snake[0].x++;

  // Teletransporte nas bordas
  if (snake[0].x < 0) snake[0].x = WIDTH - 1;
  if (snake[0].x >= WIDTH) snake[0].x = 0;
  if (snake[0].y < 0) snake[0].y = HEIGHT - 1;
  if (snake[0].y >= HEIGHT) snake[0].y = 0;

  // Comer comida
  if (snake[0].x == food.x && snake[0].y == food.y) {
    snakeLength++;
    spawnFood();
  }
}

// -------------------- PROCESSAR DIREÇÃO --------------------
void processDirection(String newDir) {
  if(newDir == "center" || newDir == "") return;
  String mapped = "center";

  // Rotaciona 90° se necessário
  if (newDir == "up") mapped = "up";
  else if (newDir == "right") mapped = "right";
  else if (newDir == "down") mapped = "down";
  else if (newDir == "left") mapped = "left";

  if (mapped == "center") return;

  // Impede reversão imediata
  bool reverse =
      (lastDirection == "up" && mapped == "down") ||
      (lastDirection == "down" && mapped == "up") ||
      (lastDirection == "left" && mapped == "right") ||
      (lastDirection == "right" && mapped == "left");

  if (reverse) return;

  direction = mapped;
  lastDirection = direction;
}

// -------------------- JOGO PRINCIPAL --------------------
void startSnakeGame() {
  static unsigned long lastMove = 0;
  static bool initialized = false;

  if (!initialized) {
    FastLED.clear();
    criarCobra();
    spawnFood();
    drawSnake();
    initialized = true;
    Serial.println("🐍 Snake game iniciado!");
  }

  unsigned long now = millis();
  if (now - lastMove > gameSpeed) {
    moveSnake();

    if (checkCollision()) {
      deathAnimation();
      criarCobra();
      spawnFood();
      lastMove = now;
      return;
    }

    processDirection(directionJoy);  // directionJoy vem do joystick principal
    drawSnake();
    lastMove = now;
  }
}
