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

#define DATA_PIN 25

#define PIN_X 34
#define PIN_Y 35
#define PIN_SW 14

#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH * HEIGHT)

CRGB leds[NUM_LEDS];
int brilho = 20;

// -------------------- CONFIG WIFI --------------------
const char* SSID = "Celio";
const char* PASSWORD = "12345678";

// -------------------- SERVIDOR ------------------------


// --- Variáveis gerais do jogo
int board[3][3] = { { 0, 0, 0 }, { 0, 0, 0 }, { 0, 0, 0 } };

// -------------------- MAPA LED --------------------
int xy(int x, int y) {
  // serpentina por coluna (x)
  if (x % 2 == 0) return x * HEIGHT + y;
  else return x * HEIGHT + (HEIGHT - 1 - y);
}
// ===================================================================================================================
// -------------------------------------VELHA-------------------------------------------------------------------------

int xvelha = 1;
int yvelha = 1;
bool turno = true;

int lastX = -1;
int lastY = -1;

// ===================================================================================================================
// -------------------------------------SNAKE-------------------------------------------------------------------------

int xmenu = 1;
int ymenu = 1;

String directionJoy = "";      // Direção atual (canonical)
String lastDirectionJoy = "";  // direção enviada
String lastMove = "center";
String currentGame = "menu";

unsigned long tempoPressionado = 0;  //tempo de joysticbutton pressionado

// --- Definição de Cores Padrão ---
#define PRETO   CRGB(0,0,0)
#define BRANCO  CRGB(255,255,255)
#define ROSA_ROXO       CRGB(212,0,255)
#define VERDE_LIMAO     CRGB(167,255,0)
#define MARROM_ESCURO   CRGB(52,25,25)
#define VERMELHO        CRGB(255,0,0)
#define AZUL_PETROLEO   CRGB(0,1,93)
#define VERDE_MUSGO     CRGB(141,205,20)
#define CIANO_CLARO     CRGB(0,255,227)
#define AZUL      CRGB(0,100,255)
#define AZUL_FORTE      CRGB(0,36,255)
#define AMARELO_CLARO CRGB(255,255,20)

// --- Matriz 16x16 ---
const CRGB image16x16[16*16] = {
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, VERDE_LIMAO, BRANCO, MARROM_ESCURO, VERMELHO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, BRANCO, AZUL_PETROLEO, BRANCO, AZUL_PETROLEO, PRETO, PRETO,
  PRETO, PRETO, VERDE_MUSGO, MARROM_ESCURO, MARROM_ESCURO, MARROM_ESCURO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, AZUL_PETROLEO, BRANCO, AZUL_PETROLEO, BRANCO, PRETO, PRETO,
  PRETO, PRETO, VERDE_LIMAO, MARROM_ESCURO, MARROM_ESCURO, VERDE_MUSGO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, BRANCO, AZUL_PETROLEO, BRANCO, AZUL_PETROLEO, PRETO, PRETO,
  PRETO, PRETO, VERDE_MUSGO, VERDE_LIMAO, VERDE_MUSGO, VERDE_LIMAO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, AZUL_PETROLEO, BRANCO, AZUL_PETROLEO, BRANCO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, PRETO,
  PRETO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, ROSA_ROXO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, VERDE_LIMAO, VERDE_LIMAO, VERDE_LIMAO, CIANO_CLARO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, PRETO, CIANO_CLARO, PRETO, PRETO, PRETO,
  PRETO, PRETO, AZUL_FORTE, VERMELHO, VERDE_LIMAO, ROSA_ROXO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, VERMELHO, CIANO_CLARO, VERMELHO, CIANO_CLARO, PRETO, PRETO,
  PRETO, PRETO, AZUL_FORTE, VERMELHO, VERMELHO, ROSA_ROXO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, VERMELHO, CIANO_CLARO, PRETO, PRETO, PRETO,
  PRETO, PRETO, AZUL_FORTE, VERMELHO, ROSA_ROXO, ROSA_ROXO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, VERMELHO, PRETO, VERMELHO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, ROSA_ROXO, ROSA_ROXO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
  PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO, PRETO,
};


void mostrarImagem(int x, int y) {
  for (int y = 0; y < HEIGHT; y++) {
    for (int x = 0; x < WIDTH; x++) {
      leds[xy(x, y)] = image16x16[y * WIDTH + x];
    }
  }
  drawBorder6x6(x, y, CRGB::Green);

  FastLED.show();
}

// -------------------- SETUP / WIFI / SERVER --------------------
void connectWiFi() {
  Serial.println("🔌 Iniciando conexão Wi-Fi...");
  WiFi.mode(WIFI_STA);

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



// -------------------- TETRIS --------------------
int px, py;
// -------------------- Joystic --------------------


int eixoX = 0;
int eixoY = 0;

int botao = HIGH;         // estado atual lido do pino
int botaoAnterior = HIGH; // estado do ciclo anterior
bool botaoClique = false; // detecta apenas o clique

String direcaoAnterior = "CENTRO";

int centerX = 1770;
int centerY = 1800;

int deadzone = 200;

void setup() {
  Serial.begin(115200);
  delay(300);

  //Joystic
  pinMode(PIN_SW, INPUT_PULLUP);

  // FastLED init
  FastLED.addLeds<NEOPIXEL, DATA_PIN>(leds, NUM_LEDS);
  FastLED.setBrightness(brilho);

  connectWiFi();

  Serial.println("🌐 Servidor iniciado!");
}


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

  // mapeamento invertido
  if (original == "up") directionJoy = "left";
  else if (original == "right") directionJoy = "down";
  else if (original == "down") directionJoy = "right";
  else if (original == "left") directionJoy = "up";
  else directionJoy = "center";

  // imprime quando mudar
  if (directionJoy != lastDirectionJoy) {
    Serial.println(directionJoy);
    lastDirectionJoy = directionJoy;
  }
}

void atualizarCursorMenu() {
  if (directionJoy == "center") {
    lastMove = "center";
    return;
  }
  if (lastMove != "center") return; // impede repetição contínua
  lastMove = directionJoy; // registra o movimento único

  if(currentGame == "menu"){ 
    if (directionJoy == "down")  ymenu = 9;
    if (directionJoy == "up")    ymenu = 1;
    if (directionJoy == "right") xmenu = 9;
    if (directionJoy == "left")  xmenu = 1;
  }

  if(currentGame == "velha"){ 
    // MOVIMENTO VERTICAL (3 casas → limites: 1 , 6 , 11)
    if(directionJoy == "down"  && yvelha < 11) yvelha += 5;
    if(directionJoy == "up"    && yvelha > 1 ) yvelha -= 5;

    // MOVIMENTO HORIZONTAL (3 casas → limites: 1 , 6 , 11)
    if(directionJoy == "right" && xvelha < 11) xvelha += 5;
    if(directionJoy == "left"  && xvelha > 1 ) xvelha -= 5;
  }

  if(currentGame == "tetris"){ 
      if(directionJoy == "right" && lastMove == "center"){
        if(!colide(px+1, py)) px++;
      }
      if(directionJoy == "left" && lastMove == "center"){
        if(!colide(px-1, py)) px--;
      }
      if(directionJoy == "down"){
        if(!colide(px,py+1)) py++;
        else fixaPeca();
      }
      
  }


  // if(currentGame == "dama"){ 
  //   if (directionJoy == "down" && yvelha > 0)  yvelha += 5;
  //   if (directionJoy == "up" && yvelha < 2)    yvelha -= 5;
  //   if (directionJoy == "right" && xvelha > 0) xvelha += 5;
  //   if (directionJoy == "left" && xvelha < 2)  xvelha -= 5;
  // }
}
void detectarSelecaoMenu() {
  if (botao != 0) return;  // só reage se botão apertado

  if (xmenu == 1 && ymenu == 1) currentGame = "snake";
  else if (xmenu == 1 && ymenu == 9) currentGame = "tetris";
  else if (xmenu == 9 && ymenu == 1) currentGame = "dama";
  else if (xmenu == 9 && ymenu == 9) currentGame = "velha";
}
void verificarBotaoPressionado() {
  if (botao == LOW) {

    if (tempoPressionado == 0)
      tempoPressionado = millis();

    unsigned long t = millis() - tempoPressionado;

    if (t > 1500) ESP.restart();

  } else {
    tempoPressionado = 0;
  }
}
void executarJogoAtual() {
  if (currentGame == "menu") {
    mostrarImagem(xmenu, ymenu);
  }
  else if (currentGame == "snake") {
    startSnakeGame();
  }
  else if (currentGame == "velha") {
    startVelhaGame();
    //aqui continuaremos
  }
  else if (currentGame == "tetris") {
    tetrisLoop();
  }
  else if (currentGame == "dama") {
    // futuro jogo
  }
}

void loop() {
  lerJoystick();
  atualizarDirecaoJoystick();
  atualizarCursorMenu();
  detectarSelecaoMenu();
  verificarBotaoPressionado();
  executarJogoAtual();
  delay(10);
}
