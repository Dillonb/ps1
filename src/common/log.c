#include <stdlib.h>

void (*fatal_handler)() = NULL;

void log_set_fatal_handler(void (*handler)()) {
    fatal_handler = handler;
}

void log_call_fatal_handler() {
    if (fatal_handler != NULL) {
        fatal_handler();
    }
}

unsigned int ps1_log_verbosity = 0;
unsigned int next_ps1_log_verbosity = 0;
