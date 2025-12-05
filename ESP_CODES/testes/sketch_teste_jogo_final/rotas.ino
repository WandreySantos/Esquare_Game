// // --- Versão que aceita requisição HTTP (se quiser controlar por web)
// void handleDir() {
//   enableCORS();

//   // Verifica se foi enviado ?move=...
//   if (!server.hasArg("move")) {
//     server.send(400, "text/plain", "no move");
//     return;
//   }

//   // Lê o valor enviado
//   String newDir = server.arg("move");  
 
//   // Chama a função que realmente muda a direção
//   processDirection(newDir);

//   // responde para o cliente
//   server.send(200, "text/plain", "ok");
// }


// // implementação simples de velha: respostas via HTTP (exemplo)
// void handleSetGame() {
//   enableCORS();
//   if (server.hasArg("game")) {
//     currentGame = server.arg("game");
//     Serial.println("Jogo escolhido " + currentGame);
//     server.send(200, "text/plain", "game set");
//   } else {
//     server.send(400, "text/plain", "no game");
//   }
// }
// void handleBrilho(){
  
// }

// //gera a amtrix
// void handleMatrix() {
//   enableCORS();
//   String json = "{\"matrix\":[";

//   for (int y = 0; y < HEIGHT; y++) {
//     json += "[";
//     for (int x = 0; x < WIDTH; x++) {
//       CRGB c = leds[xy(x, y)];
//       json += "[" + String(c.r) + "," + String(c.g) + "," + String(c.b) + "]";
//       if (x < WIDTH - 1) json += ",";
//     }
//     json += "]";
//     if (y < HEIGHT - 1) json += ",";
//   }
//   json += "]";

//   if (currentGame == "velha") {
//     json += ",\"board\":[";
//     for (int r = 0; r < 3; r++) {
//       json += "[";
//       for (int c = 0; c < 3; c++) {
//         json += String(board[r][c]);
//         if (c < 2) json += ",";
//       }
//       json += "]";
//       if (r < 2) json += ",";
//     }
//     json += "]";
//   }

//   json += "}";
//   server.send(200, "application/json", json);
// }

// void enableCORS() {
//   server.sendHeader("Access-Control-Allow-Origin", "*");
//   server.sendHeader("Access-Control-Allow-Methods", "GET, POST, OPTIONS");
//   server.sendHeader("Access-Control-Allow-Headers", "Content-Type");
// }
