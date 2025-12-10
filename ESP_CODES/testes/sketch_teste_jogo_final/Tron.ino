// // ---------------- TRON 2 PLAYERS  -----------------

// struct Player {
//   int x;
//   int y;
//   String dir;
//   CRGB color;
//   bool alive;
// };

// Player p1, p2;

// const int arenaW = 16;
// const int arenaH = 16;

// unsigned long lastMoveTRON = 0;
// int tronSpeed = 130; // mais baixo = mais rápido

// String getDirection(int analogX, int analogY, String currentDir) {
//   int x = analogRead(analogX);
//   int y = analogRead(analogY);

//   if (x < 1300 && currentDir != "right") return "left";
//   if (x > 3000 && currentDir != "left")  return "right";
//   if (y < 1300 && currentDir != "down") return "up";
//   if (y > 3000 && currentDir != "up")   return "down";

//   return currentDir;
// }

// void resetTRON() {
//   FastLED.clear();

//   p1 = { 2, 8, "right", CRGB::Blue, true };
//   p2 = { 13, 8, "left",  CRGB::Red,  true };

//   leds[xy(p1.x, p1.y)] = p1.color;
//   leds[xy(p2.x, p2.y)] = p2.color;

//   FastLED.show();
//   lastMoveTRON = millis();
// }

// bool checkDeath(int x, int y) {
//   if (x < 0 || x >= arenaW || y < 0 || y >= arenaH) return true;
//   if (leds[xy(x, y)] != CRGB::Black) return true;
//   return false;
// }

// void movePlayer(Player p, int analogX, int analogY) {
//   p.dir = getDirection(analogX, analogY, p.dir);

//   if (p.dir == "up") p.y--;
//   if (p.dir == "down") p.y++;
//   if (p.dir == "left") p.x--;
//   if (p.dir == "right") p.x++;
// }

// void startTron() {
//   unsigned long now = millis();
//   if (now - lastMoveTRON < tronSpeed) return;

//   if (!p1.alive && !p2.alive) {
//     FastLED.clear();
//     FastLED.show();
//     delay(900);
//     resetTRON();
//   }
//   if (p1.alive) movePlayer(p1, PIN_X, PIN_Y);
//   if (p2.alive) movePlayer(p2, PIN_X2, PIN_Y2);

//   if (p1.alive && checkDeath(p1.x,p1.y)) p1.alive = false;
//   else if (p1.alive) leds[xy(p1.x,p1.y)] = p1.color;

//   if (p2.alive && checkDeath(p2.x,p2.y)) p2.alive = false;
//   else if (p2.alive) leds[xy(p2.x,p2.y)] = p2.color;

//   FastLED.show();
//   lastMoveTRON = now;
// }
