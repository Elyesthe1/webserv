
# 🌐 Web Server en C++98

Ce projet a pour but de créer un **serveur web** en **C++98** en utilisant les **sockets bas niveau** du système d’exploitation.

---

## 🔌 C’est quoi un Socket ?

Un **socket**, c’est un **connecteur réseau** bas-niveau (lié au système d’exploitation) qui permet d’établir une connexion entre deux processus.

### Types de socket :
- **Socket de flux (TCP)** :
  - Avec connexion
  - Fiable (vérifie la réception des données)
  - Bidirectionnel
  - Ordonné
- **Socket de datagram (UDP)** :
  - Sans connexion
  - Pas fiable
  - Plus rapide (utilisé par exemple pour les jeux en ligne)

---

## ⚙️ Fonctions socket importantes

### `int socket(int domain, int type, int protocol)`
Cette fonction permet de **créer un socket** :
- `domain` : type d’adresse, ex: `AF_INET` pour IPv4
- `type` : ex: `SOCK_STREAM` pour TCP ou `SOCK_DGRAM` pour UDP
- `protocol` : généralement `0` (le système choisit le bon)

📄 Docs utiles :  
- [Man page officielle](https://man7.org/linux/man-pages/man2/socket.2.html)  
- [Tala Informatique - socket](https://tala-informatique.fr/index.php?title=C_socket)

---

### `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)`
Cette fonction permet de **mettre des options sur le socket**.  
Pas obligatoire, mais utile pour :
- Éviter certaines erreurs
- Améliorer les performances
- Régler des détails comme `TCP_NODELAY`, `SO_REUSEADDR`, etc.

🛠 Exemple d’option :
```cpp
setsockopt(socketFD, IPPROTO_TCP, TCP_NODELAY, &option, sizeof(option));
```

---

### `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`
Cette fonction **attache le socket à une IP et un port**.

📦 Structure utilisée :
```cpp
struct sockaddr_in {
    short            sin_family;   // Type d'adresse (ex: AF_INET pour IPv4)
    unsigned short   sin_port;     // Numéro du port (doit être converti avec htons)
    struct in_addr   sin_addr;     // Adresse IP (ex: 192.168.1.1)
    char             sin_zero[8];  // Rempli avec des zéros (non utilisé)
};
```

---

### `int listen(int sockfd, int backlog)`
Cette fonction **met le socket en écoute** pour recevoir des connexions entrantes.

- ⚠️ `backlog` est un paramètre important :
  > Lorsqu’un client demande une connexion, elle est **mise dans une file d’attente** avant d’être acceptée avec `accept()`.  
  > `backlog` détermine la **taille de cette file**.

---

## 🧠 Schéma explicatif

![Workflow Socket](https://tala-informatique.fr/images/c/cd/Socket_workflow.png)

---

## 🔗 Liens utiles

- [Man page socket](https://man7.org/linux/man-pages/man2/socket.2.html)
- [Tala Informatique - socket](https://tala-informatique.fr/index.php?title=C_socket)
