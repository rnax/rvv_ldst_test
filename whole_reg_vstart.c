#include <unistd.h>
#include <signal.h>
#include <stdio.h>
#include <string.h>
#include <malloc.h>
#include <stdlib.h>
#include <errno.h>
#include <sys/mman.h>
#include <riscv_vector.h>

#define FAILED_INDEX 15
#define VSTART_VAL    0
#define MEMCPY_SIZE 128

#define handle_error(msg) \
    do { perror(msg); exit(EXIT_FAILURE); } while (0)

int8_t *buf1;
int8_t *buf2;
long pagesize;

static void handler(int sig, siginfo_t *si, void *unused)
{
    uint32_t vstart;
    asm volatile("csrr %0, vstart":"=r"(vstart));

    if (vstart != FAILED_INDEX) {
        printf("unexpected vstart[0x%x] (expected[0x%x])\n",
               vstart, FAILED_INDEX);
        exit(EXIT_FAILURE);
    }

    exit(EXIT_SUCCESS);
}

void *memcpy_whole_reg(void *restrict destination, const void *restrict source,
                       size_t vstart);

int main(int argc, char *argv[])
{
    struct sigaction sa;

    sa.sa_flags = SA_SIGINFO;
    sigemptyset(&sa.sa_mask);
    sa.sa_sigaction = handler;
    if (sigaction(SIGSEGV, &sa, NULL) == -1) {
        handle_error("sigaction");
    }

    pagesize = sysconf(_SC_PAGE_SIZE);
    if (pagesize == -1) {
        handle_error("sysconf");
    }

    /* Allocate a buf1 aligned on a page boundary;
     *        initial protection is PROT_READ | PROT_WRITE */
    buf1 = memalign(pagesize, 2 * pagesize);
    if (buf1 == NULL) {
        handle_error("memalign");
    }
    memset(buf1, 0, 2 * pagesize);

    buf2 = memalign(pagesize, pagesize);
    if (buf2 == NULL) {
        handle_error("memalign");
    }
    memset(buf2, 0, pagesize);

    for (int i = 0; i < MEMCPY_SIZE; ++i) {
        buf2[i] = i + 1;
    }

    if (mprotect(buf1 + pagesize, pagesize, PROT_READ) == -1) {
        handle_error("mprotect");
    }

    memcpy_whole_reg(buf1 + pagesize - FAILED_INDEX, buf2, VSTART_VAL);

    exit(EXIT_SUCCESS);
}
