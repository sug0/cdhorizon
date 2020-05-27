#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdatomic.h>
#include <string.h>
#include <pthread.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <arpa/inet.h>
#include <libmill.h>

enum error_code {
    ERR_NIL,
    ERR_INTERNAL,
    ERR_INVALID_SIZE,
    ERR_TIMEOUT,
};

char sockets[0x10000] = {0};

void handleterm(int signo) {
    (void)signo;
    fprintf(stderr, "\nquitting... closing sockets\n");
    for (int fd = 3; fd < 0x10000; fd++) {
        if (sockets[fd]) {
            close(fd);
        }
    }
    fprintf(stderr, "bye\n");
    exit(0);
}

void write_control(tcpsock as, uint32_t code) {
    const uint32_t out = htonl(code);
    tcpsend(as, &out, 4, now() + 3000);
}

coroutine void timeout(int64_t deadline, chan advertise) {
    msleep(deadline);
    chs(advertise, int, 0);
}

void *worker(void *arg) {
    atomic_int *done = (atomic_int*)arg;

    sleep(1);
    atomic_store(done, 1);

    return NULL;
}

coroutine void request(tcpsock as) {
    enum error_code status;

    uint32_t size;
    int received = tcprecv(as, &size, 4, now() + 3000);
    (void)received;

    if (errno != 0) {
        status = ERR_INVALID_SIZE;
        goto done;
    }

    // convert size to little endian
    size = ntohl(size);

    // start new process

    pthread_t thr;
    atomic_int thread_quit = 0;

    if (pthread_create(&thr, NULL, worker, &thread_quit) != 0) {
        fprintf(stderr, "pthread_create() failed\n");
        status = ERR_INTERNAL;
        goto done;
    }

    chan fire_timeout = chmake(int, 0);
    go(timeout(now() + 3000, fire_timeout));

    for (;;) {
        choose {
        in(fire_timeout, int, _fired):
            (void)_fired;
            pthread_cancel(thr);
            status = ERR_TIMEOUT;
            goto done;
        otherwise:
            if (atomic_load(&thread_quit) == 1) {
                status = ERR_NIL;
                write_control(as, status);
                if (errno != 0) {
                    status = ERR_INTERNAL;
                }
                goto done;
            }
            msleep(now() + 100);
        end
        }
    }

done:
    if (status != ERR_NIL) {
        write_control(as, status);
        if (errno != 0) {
            status = ERR_INTERNAL;
        }
    }
    tcpflush(as, -1);
    tcpclose(as);
}

int main() {
    const int listen_port = 4400;
    sockets[listen_port] = 1;

    ipaddr addr = iplocal(NULL, listen_port, 0);
    tcpsock ls = tcplisten(addr, -1);

    if (!ls) {
        fprintf(stderr, "failed to listen on :%d\n", listen_port);
        return 1;
    }
    fprintf(stderr, "listening on :%d\n", listen_port);

    // install signal handlers
    signal(SIGINT, handleterm);
    signal(SIGHUP, handleterm);
    signal(SIGQUIT, handleterm);
    signal(SIGTERM, handleterm);

    for (;;) {
        tcpsock as = tcpaccept(ls, -1);
        const int fd = tcpport(as);
        sockets[fd] = 1;
        ipaddr peer = tcpaddr(as);
        char buf[IPADDR_MAXSTRLEN];
        fprintf(stderr, "accepted %s:%d\n", ipaddrstr(peer, buf), fd);
        go(request(as));
    }
}
