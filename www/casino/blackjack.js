const suits = ['♠', '♥', '♣', '♦'];
const values = ['2', '3', '4', '5', '6', '7', '8', '9', '10', 'J', 'Q', 'K', 'A'];

let playerHand = [];
let dealerHand = [];
let balance = 100;
let gameOver = false;

function drawCard() {
  let value = values[Math.floor(Math.random() * values.length)];
  let suit = suits[Math.floor(Math.random() * suits.length)];
  return { value, suit };
}

function handToString(hand) {
  return hand.map(card => card.value + card.suit).join(' ');
}

function getScore(hand) {
  let score = 0;
  let aces = 0;
  hand.forEach(card => {
    if (['J', 'Q', 'K'].includes(card.value)) score += 10;
    else if (card.value === 'A') {
      score += 11;
      aces += 1;
    } else {
      score += parseInt(card.value);
    }
  });
  while (score > 21 && aces > 0) {
    score -= 10;
    aces--;
  }
  return score;
}

function updateDisplay() {
  document.getElementById('player-hand').innerText = handToString(playerHand);
  document.getElementById('dealer-hand').innerText = gameOver ? handToString(dealerHand) : dealerHand[0].value + dealerHand[0].suit + " 🂠";
  document.getElementById('player-score').innerText = "Score : " + getScore(playerHand);
  document.getElementById('dealer-score').innerText = "Score : " + (gameOver ? getScore(dealerHand) : "?");
  document.getElementById('balance').innerText = `💰 Solde : ${balance}€`;
}

function hit() {
  if (gameOver) return;
  playerHand.push(drawCard());
  updateDisplay();
  let score = getScore(playerHand);
  if (score > 21) {
    endGame("💥 Tu as perdu !");
    balance -= 10;
  }
}

function stand() {
  if (gameOver) return;
  while (getScore(dealerHand) < 17) {
    dealerHand.push(drawCard());
  }
  let playerScore = getScore(playerHand);
  let dealerScore = getScore(dealerHand);

  if (dealerScore > 21 || playerScore > dealerScore) {
    endGame("🎉 Tu gagnes !");
    balance += 10;
  } else if (playerScore === dealerScore) {
    endGame("🤝 Égalité !");
  } else {
    endGame("💥 Tu as perdu !");
    balance -= 10;
  }
}

function endGame(msg) {
  gameOver = true;
  updateDisplay();
  document.getElementById('result').innerText = msg;
}

function resetGame() {
  playerHand = [drawCard(), drawCard()];
  dealerHand = [drawCard(), drawCard()];
  gameOver = false;
  document.getElementById('result').innerText = "";
  updateDisplay();
}

resetGame();
