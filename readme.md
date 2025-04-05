socket = connecteur reseau (bas nv lie au system d'exploitation) permet d'etablir connection entre deux processus 
    - de flux (TCP):  connection, fiable(verification de la reception donne), bidirectionnel, ordonne
    - de datagram (UDP) : pas de connection, pas fiable, mais plus rapide (utilise pour jeux en ligne)
int socket(int domain, int type, int protocol)
https://tala-informatique.fr/index.php?title=C_socket
https://man7.org/linux/man-pages/man2/socket.2.html
![Workflow Socket](https://tala-informatique.fr/images/c/cd/Socket_workflow.png)


int setsockopt(int sockfd, int level, int optname, const void *optval, socklen_t optlen)
permet de mettre des option de socket, pas obligatoire mais permet d'eviter certaine erreur et + opti car tu regle des options qui permet d'etre + specifique

int bind(int sockfd, const struct sockaddr *addr, socklen_t addrlen)
permet d'atacher un socket avec un port internet et une ip 

il faut utiliser la struc :
cette struc permet de stocker des info
struct sockaddr_in {
    short            sin_family;   // Type d'adresse (ex: AF_INET pour IPv4)
    unsigned short   sin_port;     // Numéro du port (doit être converti avec htons)
    struct in_addr   sin_addr;     // Adresse IP (ex: 192.168.1.1)
    char             sin_zero[8];  // Rempli avec des zéros (non utilisé)
};

int listen(int sockfd, int backlog) permet de mettre sont socket en en ecoute pour recevoir des connexion 
backlog (lorsque que le serv recoie une co, le met en attente dans une file d'attente avant un accept) permet de definir la taille de la file d'attente

