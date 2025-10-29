// -------------------- IMPORTAÇÕES --------------------
import express from 'express';
import http from 'http';
import { Server } from 'socket.io';
import cors from 'cors';

// -------------------- CONFIGURAÇÃO --------------------
const app = express();
app.use(cors());

const server = http.createServer(app);
const io = new Server(server, {
  cors: { origin: '*' }, // permite qualquer origem (React + ESP)
});

const PORT = process.env.PORT || 3001;

// -------------------- ESTADO DO JOGO --------------------
let players = {}; // { socketId: "X" ou "O" }
let currentPlayer = 'X';
let board = Array(3)
  .fill()
  .map(() => Array(3).fill(null));

// -------------------- EVENTOS DE CONEXÃO --------------------
io.on('connection', (socket) => {
  console.log(`Novo cliente conectado: ${socket.id}`);

  // Jogador escolhe símbolo (X ou O)
  socket.on('chooseSymbol', (symbol) => {
    if (!Object.values(players).includes(symbol)) {
      players[socket.id] = symbol;
      socket.emit('symbolChosen', symbol);
      console.log(`Jogador ${socket.id} escolheu ${symbol}`);
    } else {
      socket.emit('symbolRejected', 'Símbolo já escolhido!');
    }
  });

  // Jogador faz uma jogada
  socket.on('makeMove', ({ row, col }) => {
    const symbol = players[socket.id];
    if (!symbol || board[row][col]) return; // posição ocupada ou jogador inválido

    // Só o jogador da vez pode jogar
    if (symbol !== currentPlayer) return;

    board[row][col] = symbol;
    currentPlayer = currentPlayer === 'X' ? 'O' : 'X';

    io.emit('updateBoard', { board, currentPlayer });

    // Envia movimento pro ESP32
    io.emit('espCommand', `MOVE ${row},${col},${symbol}`);
  });

  // Reinicia o jogo
  socket.on('resetGame', () => {
    board = Array(3)
      .fill()
      .map(() => Array(3).fill(null));
    currentPlayer = 'X';
    io.emit('updateBoard', { board, currentPlayer });
    io.emit('espCommand', 'RESET_VELHA');
  });

  // ESP envia confirmação de ação
  socket.on('espStatus', (msg) => {
    console.log('ESP:', msg);
  });

  // Quando alguém desconecta
  socket.on('disconnect', () => {
    console.log(`Cliente desconectado: ${socket.id}`);
    delete players[socket.id];
  });
});

// -------------------- ROTAS DE TESTE --------------------
app.get('/', (req, res) => {
  res.send('Servidor do Jogo da Velha ativo!');
});

// -------------------- INICIALIZAÇÃO --------------------
server.listen(PORT, () => {
  console.log(`✅ Servidor rodando em http://localhost:${PORT}`);
});
