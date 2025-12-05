#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include <ArduinoJson.h>  // Para manipular JSON facilmente

#define DATA_PIN 25
#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS];
int brilho = 50;

// -------------------- CONFIG WIFI --------------------
const char* SSID = "Celio";
const char* PASSWORD = "12345678";

// -------------------- SERVIDOR ------------------------
WebServer server(80);
String currentGame = "menu";          // Menu inicial
DynamicJsonDocument gameState(1024);  // Estado do jogo (Snake, Velha, etc.)

// -------------------- WIFI ----------------------------
void connectWiFi() {
  Serial.println("🔌 Iniciando conexão Wi-Fi...");
  WiFi.mode(WIFI_STA);

  // IP FIXO
  IPAddress local_IP(192, 168, 0, 200);
  IPAddress gateway(192, 168, 0, 1);
  IPAddress subnet(255, 255, 255, 0);
  WiFi.config(local_IP, gateway, subnet);

  WiFi.begin(SSID, PASSWORD);

  int tentativas = 0;
  Serial.print("Conectando");
  while (WiFi.status() != WL_CONNECTED && tentativas < 20) {
    delay(400);
    Serial.print(".");
    tentativas++;
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\n✅ Wi-Fi conectado!");
    Serial.print("📡 IP: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\n❌ Falha ao conectar ao Wi-Fi!");
  }
}



void setup() {
  Serial.begin(115200);
  delay(300);

  connectWiFi();

  // ---------- INICIAR SERVIDOR ----------
  server.begin();
  Serial.println("🌐 Servidor iniciado!");
}



// -------------------- LOOP ----------------------------
void loop() {
  server.handleClient();

  if (currentGame == "menu") {
    mostrarImagem();
  } else if (currentGame == "snake") {
    startSnakeGame();
  } else if (currentGame == "velha") {
    starVelhaGame();
  } else if (currentGame == "dama") {
    Serial.println("dama");
  }

  delay(10);
}
