#define PIN_X 34
#define PIN_Y 35
#define PIN_SW 14

// Centro REAL
int centerX = 1770;
int centerY = 1800;

int deadzone = 200;

void setup() {
  Serial.begin(115200);
  pinMode(PIN_SW, INPUT_PULLUP);
}

void loop() {
  int eixoX = analogRead(PIN_X);
  int eixoY = analogRead(PIN_Y);
  int botao  = digitalRead(PIN_SW);

  String dirOriginal = "CENTRO";

  // Detecta direção ORIGINAL
  if (eixoX < centerX - deadzone) dirOriginal = "ESQUERDA";
  else if (eixoX > centerX + deadzone) dirOriginal = "DIREITA";

  if (eixoY < centerY - deadzone) dirOriginal = "CIMA";
  else if (eixoY > centerY + deadzone) dirOriginal = "BAIXO";

  // --- MAPEAMENTO INVERTIDO ---
  String direcao = "CENTRO";

  if (dirOriginal == "CIMA")      direcao = "DIREITA";
  else if (dirOriginal == "DIREITA") direcao = "BAIXO";
  else if (dirOriginal == "BAIXO")   direcao = "ESQUERDA";
  else if (dirOriginal == "ESQUERDA")direcao = "CIMA";

  Serial.print("X: "); Serial.print(eixoX);
  Serial.print(" | Y: "); Serial.print(eixoY);
  Serial.print(" | Direcao: "); Serial.print(direcao);
  Serial.print(" | Botao: ");
  Serial.println(botao == 0 ? "Pressionado" : "Solto");

  delay(60);
}
