---

# Project HTTPd

The goal of this project is to write a HTTP1 server, following the HTTP/1.1 specification.
The implementation of the HTTP server follows a defined protocol. This protocol is defined in the RFCs.
The HTTP server must be able to run as a daemon.

### Core features
- Basic Server
- Daemon control (Start, Quit, Reload)
- HTTP 1.1 (Conventions, Request/Response message)
- Errors handling
- Logging

## Epoll
Preliminary exercise

Epoll is a Linux kernel syscall that allows you to monitor multiple file descriptors and receive an event when an I/O operation is possible on them. You SHOULD read the epoll(7) man page carefully, as it contains all the information you need about this syscall.
Workflow

The epoll usage flow, from a high-level perspective, is as follows:

    Create an epoll(7) instance with epoll_create1(2).

    Declare your interest in a file descriptor to epoll using epoll_ctl(2).

    Wait for new events using epoll_wait(2).
