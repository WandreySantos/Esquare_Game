#include <WiFi.h>
#include <FastLED.h>
#include <WebServer.h>

#define DATA_PIN 25
#define WIDTH 16
#define HEIGHT 16
#define NUM_LEDS (WIDTH*HEIGHT)

CRGB leds[NUM_LEDS];
WebServer server(80);

const char* ssid = "Celio";
const char* password = "12345678";

enum Modo {MENU, SNAKE, VELHA};
Modo modo = MENU;

// ------------------- MATRIZ -------------------
int XY(int x, int y){
  if(x%2==0) return x*HEIGHT+y;
  else return x*HEIGHT+(HEIGHT-1-y);
}

// ------------------- VARIÁVEIS SNAKE -------------------
struct Point { int x; int y; };
Point snake[256];
int snakeLength = 3;
String direction = "RIGHT";
Point food;
int gameSpeed = 150;
bool snakeStarted = false;
unsigned long lastMove = 0;

// ------------------- VARIÁVEIS JOGO DA VELHA -------------------
int board[3][3];
int cellSize = 5;

// ------------------- FUNÇÕES SNAKE -------------------
void spawnFood(){
  bool valid = false;
  while(!valid){
    food.x = random(0, WIDTH);
    food.y = random(0, HEIGHT);
    valid = true;
    for(int i=0;i<snakeLength;i++)
      if(snake[i].x==food.x && snake[i].y==food.y) valid=false;
  }
}

void resetSnake(){
  snakeLength=3;
  snake[0]={8,8};
  snake[1]={7,8};
  snake[2]={6,8};
  direction="RIGHT";
  snakeStarted=false;
  spawnFood();
}

void moveSnake(){
  for(int i=snakeLength-1;i>0;i--) snake[i]=snake[i-1];
  if(direction=="up") snake[0].y--;
  else if(direction=="down") snake[0].y++;
  else if(direction=="left") snake[0].x--;
  else if(direction=="right") snake[0].x++;

  if(snake[0].x<0) snake[0].x=WIDTH-1;
  if(snake[0].x>=WIDTH) snake[0].x=0;
  if(snake[0].y<0) snake[0].y=HEIGHT-1;
  if(snake[0].y>=HEIGHT) snake[0].y=0;

  if(snake[0].x==food.x && snake[0].y==food.y){
    snakeLength++;
    spawnFood();
  }
}

bool checkSnakeCollision(){
  for(int i=1;i<snakeLength;i++)
    if(snake[0].x==snake[i].x && snake[0].y==snake[i].y) return true;
  return false;
}

void drawSnake(){
  FastLED.clear();
  leds[XY(food.x, food.y)] = CRGB::Red;
  for(int i=0;i<snakeLength;i++)
    leds[XY(snake[i].x,snake[i].y)] = (i==0)?CRGB::White:CRGB(0,100,0);
  FastLED.show();
}

// ------------------- FUNÇÕES JOGO DA VELHA -------------------
void drawBoard(){
  for(int i=0;i<NUM_LEDS;i++) leds[i]=CRGB::Black;
  for(int i=1;i<3;i++){
    int pos=i*cellSize;
    for(int j=0;j<WIDTH;j++){
      leds[XY(pos,j)] = CRGB::Green;
      leds[XY(j,pos)] = CRGB::Green;
    }
  }
}

void drawX(int r,int c){
  int startX=c*cellSize+1;
  int startY=r*cellSize+1;
  for(int i=0;i<4;i++) for(int j=0;j<4;j++)
    if(i==j || i+j==3) leds[XY(startX+i,startY+j)] = CRGB::Red;
}

void drawO(int r,int c){
  int startX=c*cellSize+1;
  int startY=r*cellSize+1;
  for(int i=0;i<4;i++) for(int j=0;j<4;j++)
    if(i==0||i==3||j==0||j==3) leds[XY(startX+i,startY+j)] = CRGB::Blue;
}

void updateVelhaMatrix(){
  drawBoard();
  for(int r=0;r<3;r++){
    for(int c=0;c<3;c++){
      if(board[r][c]==1) drawX(r,c);
      else if(board[r][c]==2) drawO(r,c);
    }
  }
  FastLED.show();
}

void resetVelha(){
  memset(board,0,sizeof(board));
  updateVelhaMatrix();
}

int checkWinner(){
  for(int r=0;r<3;r++)
    if(board[r][0]!=0 && board[r][0]==board[r][1] && board[r][1]==board[r][2]) return board[r][0];
  for(int c=0;c<3;c++)
    if(board[0][c]!=0 && board[0][c]==board[1][c] && board[1][c]==board[2][c]) return board[0][c];
  if(board[0][0]!=0 && board[0][0]==board[1][1] && board[1][1]==board[2][2]) return board[0][0];
  if(board[0][2]!=0 && board[0][2]==board[1][1] && board[1][1]==board[2][0]) return board[0][2];
  bool full=true;
  for(int r=0;r<3;r++) for(int c=0;c<3;c++) if(board[r][c]==0) full=false;
  if(full) return 3;
  return 0;
}

// ------------------- ROTAS WEB -------------------
void handleRoot() {
  String page = R"rawliteral(
<html>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>ESP32 Games</title>
<style>
body{background:#000;color:#0f0;text-align:center;font-family:'Courier New',monospace;margin:0;padding:20px;}
button{background:#111;color:#0f0;border:2px solid #0f0;border-radius:12px;font-size:24px;padding:15px;margin:10px;cursor:pointer;transition:0.2s;}
button:hover{background:#0f0;color:#000;}
h1{text-shadow:0 0 5px #0f0;}
</style>
<h1>Escolha o Jogo</h1>
<button onclick="window.location.href='/snakePage'">Snake</button>
<button onclick="window.location.href='/velhaPage'">Jogo da Velha</button>
</html>
)rawliteral";
  server.send(200,"text/html",page);
}

void handleSnakePage() {
  String page = R"rawliteral(
<html>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Snake</title>
<style>
body {
  background:#fff;       /* fundo branco */
  color:#000;            /* texto preto */
  font-family:'Courier New',monospace;
  display:flex;
  flex-direction:column;
  align-items:center;
  padding:20px;
  margin:0;
}
h1 {
  font-size:24px;
  font-weight:bold;
  margin-bottom:10px;
  color:#000;
  text-shadow:0 0 5px #0f0; /* neon verde no título */
}
button {
  background:#fff;       /* botão branco */
  color:#000;            /* texto preto */
  border:2px solid #0f0; /* borda neon verde */
  border-radius:12px;
  font-size:24px;
  padding:15px;
  margin:10px;
  cursor:pointer;
  transition:0.2s;
}
button:hover {
  background:#0f0;       /* neon verde ao passar mouse */
  color:#000;
}
button.cell {
  background:#fff;
  color:#000;
  border:1px solid #0f0;
}
button.cell:hover {
  background:#0f0;
  color:#fff;
}
button.action {
  margin-top:20px;
  padding:10px;
  border-radius:5px;
  background:#fff;
  color:#000;
  font-size:18px;
  cursor:pointer;
  border:1px solid #0f0;
}
button.action:hover {
  background:#0f0;
  color:#fff;
}
canvas {
  border:3px solid #0f0;
  margin-top:20px;
}
</style>

<h1>Snake Game</h1>
<canvas id='matrix' width='320' height='320'></canvas>
<button class="action" onclick="window.location.href='/'">Menu</button>
<button class="action" onclick="location.reload()">Reiniciar</button>
<script>
const PIXEL_SIZE=20;
const canvas=document.getElementById('matrix');
const ctx=canvas.getContext('2d');
async function updateMatrix(){
  const res = await fetch('/matrix');
  const data = await res.json();
  for(let y=0;y<data.matrix.length;y++)
    for(let x=0;x<data.matrix[y].length;x++){
      const [r,g,b]=data.matrix[y][x];
      ctx.fillStyle=`rgb(${r},${g},${b})`;
      ctx.fillRect(x*PIXEL_SIZE,y*PIXEL_SIZE,PIXEL_SIZE,PIXEL_SIZE);
    }
  requestAnimationFrame(updateMatrix);
}
updateMatrix();
document.addEventListener('keydown',e=>{
  const map={ArrowUp:'up',ArrowDown:'down',ArrowLeft:'left',ArrowRight:'right'};
  if(map[e.key]) fetch('/dir?move='+map[e.key]);
});
</script>
</html>
)rawliteral";
  modo = SNAKE;
  resetSnake();
  server.send(200,"text/html",page);
}

void handleVelhaPage() {
  String page = R"rawliteral(
<html>
<meta charset="UTF-8">
<meta name="viewport" content="width=device-width, initial-scale=1.0">
<title>Jogo da Velha</title>
<style>
body {
  background:#fff;       /* fundo branco */
  color:#000;            /* texto preto */
  font-family:'Courier New',monospace;
  display:flex;
  flex-direction:column;
  align-items:center;
  padding:20px;
  margin:0;
}
h1 {
  font-size:24px;
  font-weight:bold;
  margin-bottom:10px;
  color:#000;
  text-shadow:0 0 5px #0f0; /* neon verde no título */
}
button {
  background:#fff;       /* botão branco */
  color:#000;            /* texto preto */
  border:2px solid #0f0; /* borda neon verde */
  border-radius:12px;
  font-size:24px;
  padding:15px;
  margin:10px;
  cursor:pointer;
  transition:0.2s;
}
button:hover {
  background:#0f0;       /* neon verde ao passar mouse */
  color:#000;
}
button.cell {
  background:#fff;
  color:#000;
  border:1px solid #0f0;
}
button.cell:hover {
  background:#0f0;
  color:#fff;
}
button.action {
  margin-top:20px;
  padding:10px;
  border-radius:5px;
  background:#fff;
  color:#000;
  font-size:18px;
  cursor:pointer;
  border:1px solid #0f0;
}
button.action:hover {
  background:#0f0;
  color:#fff;
}
canvas {
  border:3px solid #0f0;
  margin-top:20px;
}
</style>

<h1>Jogo da Velha</h1>
<div id="board">
  <button onclick="play(0,0)" id="b00" class="cell"></button>
  <button onclick="play(0,1)" id="b01" class="cell"></button>
  <button onclick="play(0,2)" id="b02" class="cell"></button>
  <button onclick="play(1,0)" id="b10" class="cell"></button>
  <button onclick="play(1,1)" id="b11" class="cell"></button>
  <button onclick="play(1,2)" id="b12" class="cell"></button>
  <button onclick="play(2,0)" id="b20" class="cell"></button>
  <button onclick="play(2,1)" id="b21" class="cell"></button>
  <button onclick="play(2,2)" id="b22" class="cell"></button>
</div>
<p id="status"></p>
<button class="action" onclick="reiniciar()">Reiniciar</button>
<button class="action" onclick="pausar()">Pausar/Continuar</button>
<script>
let player="X"; let paused=false;
async function play(r,c){ if(paused) return; await fetch('/velhaPlay?x='+r+'&y='+c+'&player='+player); updateBoard(); checkWinner(); player=(player=="X")?"O":"X"; }
function pausar(){ paused=!paused; }
function reiniciar(){ fetch('/velhaReset'); updateBoard(); document.getElementById('status').innerText=""; player="X"; paused=false; }
async function updateBoard(){ const res=await fetch('/matrix'); const data=await res.json(); for(let r=0;r<3;r++){ for(let c=0;c<3;c++){ let btn=document.getElementById('b'+r+c); if(data.board[r][c]==1) btn.innerText='X'; else if(data.board[r][c]==2) btn.innerText='O'; else btn.innerText=''; } } }
async function checkWinner(){ const res=await fetch('/velhaWinner'); const w=await res.text(); if(w!="0") document.getElementById('status').innerText = w=="3"?"Empate!":"Vencedor: "+w; }
setInterval(updateBoard,500);
</script>
</html>
)rawliteral";
  modo = VELHA;
  resetVelha();
  server.send(200,"text/html",page);
}

// ------------------- ROTAS AUXILIARES -------------------
void handleMatrix(){
  String json="{\"matrix\":[";
  for(int y=0;y<HEIGHT;y++){
    json += "[";
    for(int x=0;x<WIDTH;x++){
      CRGB c = leds[XY(x,y)];
      json += "["+String(c.r)+","+String(c.g)+","+String(c.b)+"]";
      if(x<WIDTH-1) json+=",";
    }
    json+="]";
    if(y<HEIGHT-1) json+=",";
  }
  json+="]";
  if(modo==VELHA){
    json+=",\"board\":[";
    for(int r=0;r<3;r++){
      json += "[";
      for(int c=0;c<3;c++){
        json += String(board[r][c]);
        if(c<2) json+=",";
      }
      json+="]";
      if(r<2) json+=",";
    }
    json+="]";
  }
  json+="}";
  server.send(200,"application/json",json);
}

void handleDir(){
  if(server.hasArg("move")){
    String d = server.arg("move");
    if((d=="up" && direction!="down")||(d=="down" && direction!="up")||(d=="left" && direction!="right")||(d=="right" && direction!="left"))
      direction=d;
  }
  server.send(200,"text/plain","ok");
}

void handleVelhaPlay(){
  if(!(server.hasArg("x") && server.hasArg("y"))){ server.send(400,"text/plain","Faltam parametros"); return; }
  int x = server.arg("x").toInt();
  int y = server.arg("y").toInt();
  int player = server.hasArg("player") && server.arg("player")=="O"?2:1;
  if(x<0||x>2||y<0||y>2||board[x][y]!=0){ server.send(400,"text/plain","Posição inválida"); return; }
  board[x][y]=player;
  updateVelhaMatrix();
  server.send(200,"text/plain","ok");
}

void handleVelhaReset(){ resetVelha(); server.send(200,"text/plain","ok"); }
void handleVelhaWinner(){ int w=checkWinner(); server.send(200,"text/plain",String(w)); }

// ====== MAPA DE PINOS DO CONTROLE ======
#define BTN_UP     33
#define BTN_DOWN   32
#define BTN_LEFT   27
#define BTN_RIGHT  26

// Estados anteriores dos botões
int lastUp = HIGH;
int lastDown = HIGH;
int lastLeft = HIGH;
int lastRight = HIGH;

// ------------------- SETUP -------------------
void setup(){
  Serial.begin(115200);
  FastLED.addLeds<WS2812B,DATA_PIN,GRB>(leds,NUM_LEDS);
  FastLED.clear(); FastLED.show();

  // Conexão WiFi
  WiFi.begin(ssid,password);
  Serial.print("Conectando WiFi");
  while(WiFi.status()!=WL_CONNECTED){delay(500);Serial.print(".");}
  Serial.println("\nConectado! IP: "); Serial.println(WiFi.localIP());

  // Configura botões com pull-up interno
  pinMode(BTN_UP, INPUT_PULLUP);
  pinMode(BTN_DOWN, INPUT_PULLUP);
  pinMode(BTN_LEFT, INPUT_PULLUP);
  pinMode(BTN_RIGHT, INPUT_PULLUP);

  // Rotas do servidor
  server.on("/", handleRoot);
  server.on("/snakePage", handleSnakePage);
  server.on("/velhaPage", handleVelhaPage);
  server.on("/matrix", handleMatrix);
  server.on("/dir", handleDir);
  server.on("/velhaPlay", handleVelhaPlay);
  server.on("/velhaReset", handleVelhaReset);
  server.on("/velhaWinner", handleVelhaWinner);
  server.begin();

  Serial.println("Sistema pronto. Use o controle físico ou a interface web.");
}

// ------------------- LOOP -------------------
void loop(){
  server.handleClient();

  // --- Controles físicos (modo Snake) ---
  if (modo == SNAKE) {
    int up = digitalRead(BTN_UP);
    int down = digitalRead(BTN_DOWN);
    int left = digitalRead(BTN_LEFT);
    int right = digitalRead(BTN_RIGHT);

    // Detecta borda de descida (pressionado)
    if (up == LOW && lastUp == HIGH && direction != "down") direction = "up";
    if (down == LOW && lastDown == HIGH && direction != "up") direction = "down";
    if (left == LOW && lastLeft == HIGH && direction != "right") direction = "left";
    if (right == LOW && lastRight == HIGH && direction != "left") direction = "right";

    // Atualiza estados anteriores
    lastUp = up;
    lastDown = down;
    lastLeft = left;
    lastRight = right;

    // Movimento da cobra
    unsigned long now = millis();
    if(now - lastMove > gameSpeed){
      moveSnake();
      if(checkSnakeCollision()) resetSnake();
      drawSnake();
      lastMove = now;
    }
  }
}
