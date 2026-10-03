static inline int sys_write(int fd, const char *buf, unsigned int n) {
    int ret;
    __asm__ volatile("int $0x80"
                     : "=a"(ret)
                     : "a"(4), "b"(fd), "c"(buf), "d"(n));
    return ret;
}

int main(int argc, char **argv) {
    sys_write(1, "Hello from ELF!\n", 16);
    return 0;
}
