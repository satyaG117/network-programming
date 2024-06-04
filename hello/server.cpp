#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <errno.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/socket.h>
#include <netinet/ip.h>

static void msg(const char *msg)
{
    fprintf(stderr, "%s\n", msg);
}

// error handler function that prints error and then aborts program
static void die(const char *msg)
{
    int err = errno;
    fprintf(stderr, "[%d] %s\n", err, msg);
    abort();
}

// function that reads the message from client and sends one back
static void do_something(int connfd)
{
    char rbuf[64] = {};
    ssize_t n = read(connfd, rbuf, sizeof(rbuf) - 1);
    if (n < 0)
    {
        msg("read() error");
        return;
    }
    printf("client says: %s\n", rbuf);

    char wbuf[] = "world";
    write(connfd, wbuf, strlen(wbuf));
}

int main()
{
    /*
    create a new socket, returns a file descriptor
    socket() params : AF_INET (ipv4)
            SOCK_STREAM (TCP socket )
            0 : TCP protocol 
    */ 
    int fd = socket(AF_INET, SOCK_STREAM, 0);
    // error ; unable to create a socket
    if (fd < 0)
    {
        die("socket()");
    }

    // this is needed for most server applications
    int val = 1;
    /*
    sets options on a socket
    setsocketopt() params 
    fd : fd of socket
    SOL_SOCKET : options defined at socket level not protocol level
    SO_REUSEADDR : bind to port that already in use
    val : enables or disables the SO_REUSEADDR option,
    sizeof(val) : self explanatory
    */ 
    setsockopt(fd, SOL_SOCKET, SO_REUSEADDR, &val, sizeof(val));

    // bind
    // configure address of server and bind socket fd to it
    struct sockaddr_in addr = {};
    addr.sin_family = AF_INET;
    addr.sin_port = ntohs(1234);
    addr.sin_addr.s_addr = ntohl(0); // wildcard address 0.0.0.0
    int rv = bind(fd, (const sockaddr *)&addr, sizeof(addr));
    if (rv)
    {
        die("bind()");
    }

    // start listening from the socket , 
    //SOMAXCONN is the maximum no of pending connections in queue
    rv = listen(fd, SOMAXCONN);
    if (rv)
    {
        die("listen()");
    }


    // keep listening for client messages
    while (true)
    {
        // accept
        struct sockaddr_in client_addr = {};
        socklen_t socklen = sizeof(client_addr);
        int connfd = accept(fd, (struct sockaddr *)&client_addr, &socklen);
        if (connfd < 0)
        {
            continue; // error
        }

        do_something(connfd);
        close(connfd);
    }

    return 0;
}