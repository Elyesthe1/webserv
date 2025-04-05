# 🌐 Socket - Serveur Web en C++

Ce projet vise à créer un serveur web en C++98 en utilisant les sockets bas-niveau du système d'exploitation.

---

## 🔌 Qu'est-ce qu'un Socket ?

Un **socket** est un **connecteur réseau** (niveau bas) permettant d'établir une **connexion entre deux processus**.

### Types de sockets :
- **Flux (TCP)** :
  - Connexion établie
  - Fiable (vérifie la réception des données)
  - Bidirectionnel
  - Ordonné
- **Datagramme (UDP)** :
  - Pas de connexion
  - Non fiable
  - Très rapide (utilisé pour les jeux en ligne, etc.)

---

## ⚙️ Fonctions essentielles

### `int socket(int domain, int type, int protocol)`
Crée un socket :
- `domain` : ex. `AF_INET` (IPv4)
- `type` : `SOCK_STREAM` (TCP) ou `SOCK_DGRAM` (UDP)
- `protocol` : généralement `0` (choix automatique)

📄 [Voir la doc officielle](https://man7.org/linux/man-pages/man2/socket.2.html)  
📘 [Article explicatif](https://tala-informatique.fr/index.php?title=C_socket)

---

### `int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)`
Permet de **configurer certaines options du socket** :
- Pas obligatoire, mais souvent utilisé pour éviter des erreurs ou améliorer les performances.
- Exemple : `SO_REUSEADDR`, `TCP_NODELAY`, etc.

---

### `int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)`
Associe un **socket à une adresse IP et un port**.

### 📦 Structure `sockaddr_in`
```c
struct sockaddr_in {
    short            sin_family;   // Type d'adresse (ex: AF_INET pour IPv4)
    unsigned short   sin_port;     // Port (converti avec htons)
    struct in_addr   sin_addr;     // Adresse IP (ex: 127.0.0.1)
    char             sin_zero[8];  // Padding (non utilisé)
};
