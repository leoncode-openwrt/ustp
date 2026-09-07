#include <assert.h>
#include <stdbool.h>
#include <stdarg.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>

static int test_socket(int domain, int type, int protocol);
static int test_setsockopt(int fd, int level, int option,
                          const void *value, socklen_t size);
static int test_fcntl(int fd, int command, ...);
static int test_close(int fd);

#define socket test_socket
#define setsockopt test_setsockopt
#define fcntl test_fcntl
#define close test_close
#include "../packet.c"
#undef socket
#undef setsockopt
#undef fcntl
#undef close

int log_level;
static bool fail_ignore;
static unsigned int filtered, ignored, closed, registered;

void Dprintf(int level, const char *fmt, ...)
{
    (void)level;
    (void)fmt;
}

static int test_socket(int domain, int type, int protocol)
{
    assert(domain == PF_PACKET);
    assert(type == SOCK_RAW);
    assert(protocol == htons(ETH_P_ALL));
    return 42;
}

static int test_setsockopt(int fd, int level, int option,
                          const void *value, socklen_t size)
{
    assert(fd == 42);
    if (option == SO_ATTACH_FILTER && level == SOL_SOCKET) {
        const struct sock_fprog *filter = value;
        assert(size == sizeof(*filter));
        assert(filter->len > 0);
        filtered++;
        return 0;
    }
    assert(level == SOL_PACKET && option == PACKET_IGNORE_OUTGOING);
    assert(size == sizeof(int) && *(const int *)value == 1);
    ignored++;
    return fail_ignore ? -1 : 0;
}

static int test_fcntl(int fd, int command, ...)
{
    va_list args;
    assert(fd == 42 && command == F_SETFL);
    va_start(args, command);
    assert(va_arg(args, int) == O_NONBLOCK);
    va_end(args);
    return 0;
}

static int test_close(int fd)
{
    assert(fd == 42);
    closed++;
    return 0;
}

int uloop_fd_add(struct uloop_fd *fd, unsigned int flags)
{
    assert(fd->fd == 42 && flags == (ULOOP_READ | ULOOP_EDGE_TRIGGER));
    registered++;
    return 0;
}

bool worker_queue_event(const struct worker_event *event)
{
    (void)event;
    return true;
}

int main(void)
{
    assert(packet_sock_init() == 0);
    assert(filtered == 1 && ignored == 1 && registered == 1 && closed == 0);
    fail_ignore = true;
    assert(packet_sock_init() < 0);
    assert(filtered == 2 && ignored == 2 && registered == 1 && closed == 1);
    return 0;
}
