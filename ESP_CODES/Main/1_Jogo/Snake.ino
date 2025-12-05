struct Point {
  int x;
  int y;
};

Point snake[256];
int snakeLength = 3;
String direction = "RIGHT";
String lastDirection = "RIGHT";
Point food;
int gameSpeed = 150;
String currentDifficulty = "medium";
bool start_game = true;

// --- Spawn da comida ---
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

// --- Direção ---
// void handleDir() {
//   Serial.println("📩 handleDir chamado!");

//   if (server.hasArg("move")) {
//     String newDir = server.arg("move");

//     Serial.println("🕹️ Tecla recebida: " + newDir);
//     Serial.println("Última direção: " + lastDirection);

//     if (newDir == "up" || newDir == "down" || newDir == "left" || newDir == "right") {

//       // EVITA reversão
//       if (!((lastDirection == "up" && newDir == "down")
//             || (lastDirection == "down" && newDir == "up")
//             || (lastDirection == "left" && newDir == "right")
//             || (lastDirection == "right" && newDir == "left"))) {
        
//         direction = newDir;
//         lastDirection = direction;

//         Serial.println("➡️ Direção atualizada para: " + direction);
//       } else {
//         Serial.println("⚠️ Reversão bloqueada!");
//       }
//     } else {
//       Serial.println("❌ Direção inválida: " + newDir);
//     }

//     server.send(200, "text/plain", "ok");
//   } else {
//     Serial.println("❌ Nenhum argumento 'move' encontrado!");
//   }
}


// --- Movimento da cobra ---
void moveSnake() {
  for (int i = snakeLength - 1; i > 0; i--)
    snake[i] = snake[i - 1];

  if (direction == "up") snake[0].y--;
  else if (direction == "down") snake[0].y++;
  else if (direction == "left") snake[0].x--;
  else if (direction == "right") snake[0].x++;

  if (snake[0].x < 0) snake[0].x = WIDTH - 1;
  if (snake[0].x >= WIDTH) snake[0].x = 0;
  if (snake[0].y < 0) snake[0].y = HEIGHT - 1;
  if (snake[0].y >= HEIGHT) snake[0].y = 0;

  if (snake[0].x == food.x && snake[0].y == food.y) {
    snakeLength++;
    spawnFood();
  }
}

void criarCobra() {
  snake[0] = { 8, 8 };
  snake[1] = { 7, 8 };
  snake[2] = { 6, 8 };
}

// --- Jogo principal (chamado pela main) ---
void startSnakeGame() {
  static unsigned long lastMove = 0;
  static bool initialized = false;

  // Inicializa uma única vez
  if (!initialized) {
    FastLED.clear();
    criarCobra();
    spawnFood();  // ✅ comida é gerada só uma vez
    drawSnake();
    initialized = true;
    Serial.println("🐍 Snake game iniciado!");
  }

  // Atualiza o jogo sem travar o ESP
  unsigned long now = millis();
  if (now - lastMove > gameSpeed) {
    moveSnake();

    // ✅ Verifica se a cobra comeu a comida
    if (snake[0].x == food.x && snake[0].y == food.y) {
      snakeLength++;
      spawnFood();  // Gera nova comida apenas quando come
    }

    if (checkCollision()) {
      deathAnimation();
      snakeLength = 3;
      criarCobra();
      spawnFood();
      return;  // ✅ Sai e volta ao loop
    }

    drawSnake();  // Desenha tudo só depois da lógica de jogo
    lastMove = now;
  }

  // Atualiza LEDs
  FastLED.show();
}
