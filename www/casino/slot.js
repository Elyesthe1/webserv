const symbols = ["🍒", "🍋", "🔔", "🍉", "⭐", "💎"];
let balance = 100;

const reels = document.querySelectorAll(".reel");
const resultText = document.getElementById("result");
const balanceText = document.getElementById("balance");

function updateBalanceDisplay() {
  balanceText.innerText = `💰 Solde: ${balance}€`;
}

function spin() {
  if (balance < 10) {
    resultText.innerText = "❌ Solde insuffisant !";
    return;
  }

  balance -= 10;
  updateBalanceDisplay();
  resultText.innerText = "🎲 Lancement...";

  const results = [];

  let spins = 10;
  let interval = setInterval(() => {
    for (let i = 0; i < reels.length; i++) {
      reels[i].innerText = symbols[Math.floor(Math.random() * symbols.length)];
    }
    spins--;
    if (spins <= 0) {
      clearInterval(interval);

      for (let i = 0; i < reels.length; i++) {
        let finalSymbol = symbols[Math.floor(Math.random() * symbols.length)];
        reels[i].innerText = finalSymbol;
        results.push(finalSymbol);
      }

      if (results[0] === results[1] && results[1] === results[2]) {
        balance += 100;
        resultText.innerText = "🎉 JACKPOT +100€ !";
      } else if (results[0] === results[1] || results[1] === results[2] || results[0] === results[2]) {
        balance += 20;
        resultText.innerText = "👏 Deux identiques +20€ !";
      } else {
        resultText.innerText = "🙁 Perdu -10€...";
      }

      updateBalanceDisplay();
    }
  }, 100);
}

updateBalanceDisplay();
