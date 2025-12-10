
void starVelhaGame() {
  static unsigned long last = 0;
  if (millis() - last > 1000) {
    Serial.println("⭕ Rodando Jogo da Velha...");
    last = millis();
  }
}

// #include <Arduino.h>
// #include <WiFi.h>
// #include <FastLED.h>
// #include <ArduinoJson.h>  // Para manipular JSON facilmente

// #define DATA_PIN 25
// #define WIDTH 16
// #define HEIGHT 16
// #define NUM_LEDS (WIDTH * HEIGHT)

// CRGB leds[NUM_LEDS];
// int brilho = 50;

// // -------------------- CONFIG WIFI --------------------
// const char* SSID = "Celio";
// const char* PASSWORD = "12345678";

// // -------------------- SERVIDOR ------------------------
// WebServer server(80);
// String currentGame = "menu";          // Menu inicial
// DynamicJsonDocument gameState(1024);  // Estado do jogo (Snake, Velha, etc.)

// // -------------------- WIFI ----------------------------
// void connectWiFi() {
//   Serial.println("🔌 Iniciando conexão Wi-Fi...");
//   WiFi.mode(WIFI_STA);

//   // IP FIXO
//   IPAddress local_IP(192, 168, 0, 200);
//   IPAddress gateway(192, 168, 0, 1);
//   IPAddress subnet(255, 255, 255, 0);
//   WiFi.config(local_IP, gateway, subnet);

//   WiFi.begin(SSID, PASSWORD);

//   int tentativas = 0;
//   Serial.print("Conectando");
//   while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
//     delay(400);
//     Serial.print(".");
//     tentativas++;
//   }

//   if (WiFi.status() == WL_CONNECTED) {
//     Serial.println("\n✅ Wi-Fi conectado!");
//     Serial.print("📡 IP: ");
//     Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("\n❌ Falha ao conectar ao Wi-Fi!");
//   }

//   FastLED.addLeds<WS2812B, DATA_PIN, GRB>(leds, NUM_LEDS);
//   FastLED.setBrightness(brilho);
//   FastLED.clear();
//   FastLED.show();
// }

// // -------------------- MATRIX --------------------------
// void initMatrix() {
//   Serial.println("💡 Matriz inicializada!");
// }

// // -------------------- ROTAS ---------------------------
// void handleSelectGame() {
//   if (!server.hasArg("plain")) {
//     server.send(400, "application/json", "{\"error\":\"Requisição inválida\"}");
//     return;
//   }

//   String body = server.arg("plain");
//   Serial.println("📩 POST recebido: " + body);

//   DynamicJsonDocument doc(1024);
//   deserializeJson(doc, body);
//   String game = doc["game"];

//   if (game == "snake") currentGame = "snake";
//   else if (game == "velha") currentGame = "velha";
//   else if (game == "dama") currentGame = "dama";
//   else currentGame = "tetris";

//   // Inicializar estado do jogo
//   gameState.clear();
//   if (currentGame == "snake") {
//     gameState["snake_running"] = true;
//     gameState["snake_score"] = 0;
//     gameState["snake_food"]["x"] = 10;
//     gameState["snake_food"]["y"] = 10;
//   } else if (currentGame == "velha") {
//     JsonArray board = gameState.createNestedArray("board");
//     for (int i = 0; i < 3; i++) {
//       JsonArray row = board.createNestedArray();
//       for (int j = 0; j < 3; j++) row.add(nullptr);
//     }
//     gameState["currentPlayer"] = "X";
//   }

//   server.send(200, "application/json",
//               "{\"status\":\"ok\",\"currentGame\":\"" + currentGame + "\"}");
// }

// void handleGetGame() {
//   String output;
//   serializeJson(gameState, output);
//   server.send(200, "application/json", output);
// }

// // -------------------- SETUP ---------------------------
// // --------- SETUP COM CORS CORRETO ---------
// void setup() {
//   Serial.begin(115200);
//   delay(300);

//   connectWiFi();

//   // ==================  CORS  ===================
//   auto enableCORS = []() {
//     server.sendHeader("Access-Control-Allow-Origin", "*");
//     server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
//     server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
//   };

//   // -------- OPTIONS (preflight) da rota /select ----------
//   server.on("/select", HTTP_OPTIONS, [enableCORS]() {
//     enableCORS();
//     server.send(204);  // sem body
//   });

//   // -------- POST /select ----------
//   server.on("/select", HTTP_POST, [enableCORS]() {
//     enableCORS();

//     if (!server.hasArg("plain")) {
//       server.send(400, "application/json", "{\"error\":\"invalid body\"}");
//       return;
//     }

//     String body = server.arg("plain");
//     if (body.indexOf("snake") >= 0) currentGame = "snake";
//     else if (body.indexOf("velha") >= 0) currentGame = "velha";
//     else if (body.indexOf("dama") >= 0) currentGame = "dama";
//     else currentGame = "tetris";

//     server.send(200, "application/json",
//                 "{\"status\":\"ok\",\"currentGame\":\"" + currentGame + "\"}");
//     Serial.println(currentGame);
//   });

//   // ---------- GET /status ----------
//   server.on("/status", HTTP_GET, [enableCORS]() {
//     enableCORS();
//     server.send(200, "application/json",
//                 "{\"currentGame\":\"" + currentGame + "\"}");
//   });

//   // ---------- GET /ip ----------
//   server.on("/ip", HTTP_GET, [enableCORS]() {
//     enableCORS();
//     server.send(200, "text/plain", WiFi.localIP().toString());
//   });

//   // ---------- ROTA PRINCIPAL ----------
//   server.on("/", HTTP_GET, [enableCORS]() {
//     enableCORS();
//     server.send(200, "text/plain", "Servidor funcionando!");
//   });

//   server.on("/move", HTTP_POST, [enableCORS]() {
//     enableCORS();
//     if (!server.hasArg("move")) {
//       server.send(400, "text/plain", "missing move");
//       return;
//     }
//     handleDir();  // chama sua função de movimento
//   });


//   // ---------- INICIAR SERVIDOR ----------
//   server.begin();
//   Serial.println("🌐 Servidor iniciado!");
// }



// // -------------------- LOOP ----------------------------
// void loop() {
//   server.handleClient();

//   if (currentGame == "menu") {
//     mostrarImagem();
//   } else if (currentGame == "snake") {
//     startSnakeGame();
//   } else if (currentGame == "velha") {
//     starVelhaGame();
//   } else if (currentGame == "dama") {
//     Serial.println("dama");
//   }

//   delay(10);
// }
