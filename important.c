#include <stdio.h>

#define STATUS_SUCCESS ((int)0x00000000L)

#define okay(msg, ...) printf("\x1b[1;32m[+] " msg "\n\x1b[0m", ##__VA_ARGS__)
#define info(msg, ...) printf("\x1b[1;34m[i] " msg "\n\x1b[0m\x1b[0m", ##__VA_ARGS__)
#define warn(msg, ...) printf("\x1b[0;101m[-] " msg "\n\x1b[0m\x1b[0m", ##__VA_ARGS__)  // Red text

