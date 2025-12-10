/*
  Projeto unificado - Single file .ino
  - WiFi + WebServer
  - FastLED 16x16 matrix (serpentina)
  - Snake (controle por joystick)
  - Velha (tic-tac-toe)
  - Imagem inicial
  - handleDir() adaptado para joystick com remapeamento 90° CW
*/

#include <Arduino.h>
#include <WiFi.h>
#include <WebServer.h>
#include <FastLED.h>
#include <ArduinoJson.h>

// -------------------- CONFIGURAÇÃO GERAL --------------------
#define DATA_PIN 25
#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS];
int brilho = 20;

// -------------------- JOYSTICK --------------------

// PRIEMIRA  MANETE
#define PIN_X 34
#define PIN_Y 35
#define PIN_SW 14

// SEGUNDA MANETE
#define PIN_X2 32
#define PIN_Y2 33
#define PIN_SW2 27

int eixoX = 0;
int eixoY = 0;
int botao = HIGH;
int botaoAnterior = HIGH;
bool botaoClique = false;
unsigned long tempoPressionado = 0;

int centerX = 2000;
int centerY = 2000;
int deadzone = 200;

String directionJoy = "";
String lastDirectionJoy = "";
String lastMove = "center";

// -------------------- MENU --------------------
int xmenu = 1;
int ymenu = 1;
String currentGame = "menu";

// -------------------- JOGO DA VELHA --------------------
int board[3][3] = { {0,0,0},{0,0,0},{0,0,0} };
int xvelha = 1;
int yvelha = 1;
bool turno = true;
int lastX = -1;
int lastY = -1;

// -------------------- CORES PADRÃO --------------------
#define PRETO   CRGB(0,0,0)
#define BRANCO  CRGB(255,255,255)

// -------------------- IMAGEM INICIAL 16x16 --------------------
const CRGB image16x16[16*16] = {
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, CRGB(167,255,0), BRANCO, CRGB(52,25,25), CRGB(255,0,0), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, BRANCO, CRGB(0,1,93), BRANCO, CRGB(0,1,93), PRETO, PRETO,
  PRETO, PRETO, CRGB(141,205,20), CRGB(52,25,25), CRGB(52,25,25), CRGB(52,25,25), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, CRGB(0,1,93), BRANCO, CRGB(0,1,93), BRANCO, PRETO, PRETO,
  PRETO, PRETO, CRGB(167,255,0), CRGB(52,25,25), CRGB(52,25,25), CRGB(141,205,20), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, BRANCO, CRGB(0,1,93), BRANCO, CRGB(0,1,93), PRETO, PRETO,
  PRETO, PRETO, CRGB(141,205,20), CRGB(167,255,0), CRGB(141,205,20), CRGB(167,255,0), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, CRGB(0,1,93), BRANCO, CRGB(0,1,93), BRANCO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), PRETO,
  PRETO, CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), CRGB(212,0,255), PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, CRGB(167,255,0), CRGB(167,255,0), CRGB(167,255,0), CRGB(0,255,227), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, CRGB(0,255,227), PRETO, PRETO, CRGB(141,205,20), PRETO, PRETO,
  PRETO, PRETO, CRGB(0,36,255), CRGB(255,0,0), CRGB(167,255,0), CRGB(212,0,255), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, PRETO, CRGB(141,205,20), PRETO, PRETO, PRETO,
  PRETO, PRETO, CRGB(0,36,255), CRGB(255,0,0), CRGB(255,0,0), CRGB(212,0,255), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, CRGB(141,205,20), PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, CRGB(0,36,255), CRGB(255,0,0), CRGB(212,0,255), CRGB(212,0,255), PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, CRGB(141,205,20), PRETO, PRETO, CRGB(255,0,0), PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, CRGB(212,0,255), CRGB(212,0,255), PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
};


// -------------------- FUNÇÕES DE MATRIZ --------------------
int xy(int x, int y) {
  // serpentina por coluna
  if (x % 2 == 0) return x * HEIGHT + y;
  else return x * HEIGHT + (HEIGHT - 1 - y);
}

void mostrarImagem(int x, int y) {
  for (int y = 0; y < HEIGHT; y++)
    for (int x = 0; x < WIDTH; x++)
      leds[xy(x, y)] = image16x16[y * WIDTH + x];

  drawBorder6x6(x, y, CRGB::Green);
  FastLED.show();
}

// -------------------- WIFI --------------------
const char* SSID = "Celio";
const char* PASSWORD = "12345678";

// void connectWiFi() {
//   Serial.println("🔌 Iniciando conexão Wi-Fi...");
//   WiFi.mode(WIFI_STA);

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
//     Serial.print("📡 IP: "); Serial.println(WiFi.localIP());
//   } else {
//     Serial.println("\n❌ Falha ao conectar ao Wi-Fi!");
//   }
// }

// -------------------- JOYSTICK: LEITURA E DIREÇÃO --------------------
void lerJoystick() {
  eixoX = analogRead(PIN_X);
  eixoY = analogRead(PIN_Y);

  botaoAnterior = botao;           
  botao = digitalRead(PIN_SW);     
  botaoClique = (botaoAnterior == HIGH && botao == LOW);
}

void atualizarDirecaoJoystick() {
  String original = "center";

  if (eixoX < centerX - deadzone) original = "left";
  else if (eixoX > centerX + deadzone) original = "right";
  if (eixoY < centerY - deadzone) original = "down";
  else if (eixoY > centerY + deadzone) original = "up";

  // mapeamento invertido 90° CW
  if (original == "up") directionJoy = "left";
  else if (original == "right") directionJoy = "down";
  else if (original == "down") directionJoy = "right";
  else if (original == "left") directionJoy = "up";
  else directionJoy = "center";

  if (directionJoy != lastDirectionJoy) {
    Serial.println(directionJoy);
    lastDirectionJoy = directionJoy;
  }
}

// -------------------- TETRIS --------------------
int px, py;

// -------------------- CURSOR / MOVIMENTOS --------------------

void atualizarCursor() {

  if (directionJoy == "center") { lastMove = "center"; return; }
  if (lastMove != "center") return;
  lastMove = directionJoy;

  if(currentGame == "menu"){ 
    if (directionJoy == "down")  ymenu = 9;
    if (directionJoy == "up")    ymenu = 1;
    if (directionJoy == "right") xmenu = 9;
    if (directionJoy == "left")  xmenu = 1;
  }

  if(currentGame == "velha"){ 
    if(directionJoy == "down"  && yvelha < 11) yvelha += 5;
    if(directionJoy == "up"    && yvelha > 1 ) yvelha -= 5;
    if(directionJoy == "right" && xvelha < 11) xvelha += 5;
    if(directionJoy == "left"  && xvelha > 1 ) xvelha -= 5;
  }

  if(currentGame == "tetris"){ 
    if(directionJoy == "right" && lastMove == "center" && !colide(px+1, py)) px++;
    if(directionJoy == "left" && lastMove == "center" && !colide(px-1, py)) px--;
    if(directionJoy == "down"){
      if(!colide(px,py+1)) py++;
      else fixaPeca();
    }
  }
}

// -------------------- MENU / SELEÇÃO --------------------
void detectarSelecaoMenu() {
  if (botao != 0) return;
  if (xmenu == 1 && ymenu == 1) currentGame = "snake";
  else if (xmenu == 1 && ymenu == 9) currentGame = "tetris";
  else if (xmenu == 9 && ymenu == 1) currentGame = "tron";
  else if (xmenu == 9 && ymenu == 9) currentGame = "velha";
}

void verificarBotaoPressionado() {
  if (botao == LOW) {
    if (tempoPressionado == 0) tempoPressionado = millis();
    if (millis() - tempoPressionado > 1500) ESP.restart();
  } else tempoPressionado = 0;
}

// -------------------- EXECUÇÃO DE JOGOS --------------------
void executarJogoAtual() {
  if (currentGame == "menu") mostrarImagem(xmenu, ymenu);
  else if (currentGame == "snake") startSnakeGame();
  else if (currentGame == "velha") startVelhaGame();
  else if (currentGame == "tetris") tetrisLoop();
  else if (currentGame == "tron") startTron();
}

// -------------------- SETUP --------------------
void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(PIN_SW, INPUT_PULLUP);

  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brilho);

  // connectWiFi();
  Serial.println("  ESP iniciado!");
}

// -------------------- LOOP --------------------
void loop() {
  lerJoystick();
  atualizarDirecaoJoystick();
  atualizarCursor();
  detectarSelecaoMenu();
  verificarBotaoPressionado();
  executarJogoAtual();
  delay(10);
}
