#define _GNU_SOURCE
#include <assert.h>
#include <stdarg.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

static bool fail_calloc;
static unsigned int restored_ifindexes[4];
static unsigned int restored_states[4];
static unsigned int restored_count;

static void *test_calloc(size_t count, size_t size)
{
	if (fail_calloc)
		return NULL;
	return calloc(count, size);
}

#define SYSFS_CLASS_NET "/tmp/ustp-test-bridge"
#define calloc test_calloc
#include "../bridge_track.c"
#undef calloc

int log_level;
struct rtnl_handle rth_state;

static int record_state(struct rtnl_handle *rth, unsigned int ifindex,
                        __u8 state)
{
    (void)rth;
    assert(restored_count < 4);
    restored_ifindexes[restored_count] = ifindex;
    restored_states[restored_count] = state;
    restored_count++;
    return 0;
}

void Dprintf(int level, const char *fmt, ...);
void Dprintf(int level, const char *fmt, ...)
{
	(void)level;
	(void)fmt;
}

static void test_port_scan(void)
{
	struct dirent **names = NULL;
	int count;

	assert(system("rm -rf " SYSFS_CLASS_NET) == 0);
	assert(mkdir(SYSFS_CLASS_NET, 0700) == 0);
	assert(mkdir(SYSFS_CLASS_NET "/br0", 0700) == 0);
	assert(mkdir(SYSFS_CLASS_NET "/br0/brif", 0700) == 0);
	assert(close(creat(SYSFS_CLASS_NET "/br0/brif/eth2", 0600)) == 0);
	assert(close(creat(SYSFS_CLASS_NET "/br0/brif/eth10", 0600)) == 0);
	count = get_port_list("br0", &names);
	assert(count == 2);
	assert(!strcmp(names[0]->d_name, "eth2"));
	assert(!strcmp(names[1]->d_name, "eth10"));
	free(names[0]);
	free(names[1]);
	free(names);

	assert(mkdir(SYSFS_CLASS_NET "/empty", 0700) == 0);
	assert(mkdir(SYSFS_CLASS_NET "/empty/brif", 0700) == 0);
	assert(get_port_list("empty", &names) == 0);
	free(names);
	assert(get_port_list("missing", &names) < 0);
	assert(system("rm -rf " SYSFS_CLASS_NET) == 0);
}

static struct dirent **make_names(const char *first, const char *second)
{
	struct dirent **names = calloc(2, sizeof(*names));

	assert(names);
	names[0] = calloc(1, sizeof(*names[0]));
	names[1] = calloc(1, sizeof(*names[1]));
	assert(names[0] && names[1]);
	strcpy(names[0]->d_name, first);
	strcpy(names[1]->d_name, second);
	return names;
}

static void test_port_resolution_errors(void)
{
	struct dirent **names;
	int *ports = (int *)1;

	names = make_names("lo", "ustp-port-does-not-exist");
	assert(resolve_port_list(names, 2, &ports) == -1);
	assert(ports == NULL);

	names = make_names("lo", "lo");
	fail_calloc = true;
	assert(resolve_port_list(names, 2, &ports) == -1);
	fail_calloc = false;
	assert(ports == NULL);
}

static void test_shutdown_restores_forwarding(void)
{
    bridge_t bridge = {};
    port_t first = {}, second = {};

    INIT_LIST_HEAD(&bridge.ports);
    INIT_LIST_HEAD(&first.br_list);
    INIT_LIST_HEAD(&second.br_list);
    first.bridge = &bridge;
    second.bridge = &bridge;
    first.sysdeps.if_index = 3;
    second.sysdeps.if_index = 6;
    list_add_tail(&first.br_list, &bridge.ports);
    list_add_tail(&second.br_list, &bridge.ports);

    restored_count = 0;
    restore_forwarding(&bridge, record_state);
    assert(restored_count == 2);
    assert(restored_ifindexes[0] == 3);
    assert(restored_ifindexes[1] == 6);
    assert(restored_states[0] == BR_STATE_FORWARDING);
    assert(restored_states[1] == BR_STATE_FORWARDING);
}

int main(void)
{
    test_port_scan();
    test_port_resolution_errors();
    test_shutdown_restores_forwarding();
    return 0;
}
