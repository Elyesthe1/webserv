# Webserv

> A high-performance HTTP/1.1 server built from scratch in C++, inspired by Nginx architecture.

![C++](https://img.shields.io/badge/C++-98-blue.svg)
![Linux](https://img.shields.io/badge/Linux-epoll-green.svg)
![Team](https://img.shields.io/badge/Team-3_devs-orange.svg)

---

## ⚡ Performance

Benchmarked with [`wrk`](https://github.com/wg/wrk) on localhost:

```
wrk -t4 -c100 -d30s http://localhost:8080/
```

| Metric | Result |
|---|---|
| Requests/sec | **5,200 req/s** |
| Avg latency | **4.63 ms** |
| Concurrent connections | 100 |

---

## ✨ Features

- **Event-driven architecture** — non-blocking I/O via `epoll` on Linux
- **HTTP/1.1** — GET, POST, DELETE methods
- **CGI support** — Python and PHP script execution
- **Virtual hosts** — multiple servers on the same port via `server_name`
- **File upload** — via POST with configurable upload directory
- **Autoindex** — directory listing when no index file is found
- **Redirections** — per-location HTTP redirects
- **Custom error pages** — configurable per server (404, 413, ...)
- **Custom config file** — Nginx-inspired syntax

---

## 🛠️ Build & Run

### Requirements
- Linux (epoll)
- `g++` or `clang++` with C++98
- `make`

### Build
```bash
git clone https://github.com/Elyesthe1/webserv.git
cd webserv
make
```

### Run
```bash
./webserv config.conf
```

---

## ⚙️ Configuration

The server uses a custom Nginx-inspired config format.

### Example

```nginx
server:
    listen=8080
    error_page_404=www/casino/errors/404.html
    error_page_413=www/default/errors/413.html

    location_/:
        root=www/casino
        index=index.html
        autoindex=off
        allow_methods=GET POST DELETE

    location_/upload:
        root=www/casino/uploads
        upload=www/casino/uploads
        allow_methods=POST

    location_/redirect:
        redirection=https://www.google.com

    location_/cgi-bin:
        root=www/casino/cgi
        allow_methods=GET POST

    location_/images:
        root=www/casino/uploads
        index=default.jpg
        autoindex=on

server:
    listen=8080
    server_name=site1.com
    error_page_404=www/default/errors/404.html
    error_page_413=www/default/errors/413.html

    location_/:
        root=www/default
        index=index.html
        autoindex=off
        allow_methods=GET POST
```

### Config reference

| Directive | Scope | Description |
|---|---|---|
| `listen` | server | Port to listen on |
| `server_name` | server | Virtual host name |
| `error_page_XXX` | server | Custom error page path |
| `root` | location | Document root |
| `index` | location | Default index file |
| `autoindex` | location | Enable directory listing |
| `allow_methods` | location | Allowed HTTP methods |
| `upload` | location | Upload destination directory |
| `redirection` | location | HTTP redirect target |

---

## 🏗️ Architecture

```
┌─────────────────────────────────────────┐
│              epoll event loop           │
│                                         │
│  ┌──────────┐    ┌────────────────────┐ │
│  │ Listener │───▶│  Client Handler    │ │
│  └──────────┘    │  - HTTP Parser     │ │
│                  │  - Request Router  │ │
│                  │  - Response Builder│ │
│                  └────────┬───────────┘ │
│                           │             │
│            ┌──────────────┼──────────┐  │
│            ▼              ▼          ▼  │
│       Static Files     CGI Fork   Upload│
│       (GET/DELETE)  (Python/PHP)  (POST)│
└─────────────────────────────────────────┘
```

---

## 👥 Team

Built by 3 students at [42 Nice](https://42nice.fr) using Agile methodology and Git workflow.

---

## 📄 License

This project was developed as part of the 42 curriculum.
