#include <stdio.h>
#include <cflags.h>
#include <mem/ps1system.h>
#include <log.h>

void usage(cflags_t* flags) {
    cflags_print_usage(flags,
                       "[OPTION]... [FILE]",
                       "dgb-ps1, a dgb ps1 emulator",
                       "https://github.com/Dillonb/ps1");
}

int main(int argc, char** argv) {
    cflags_t* flags = cflags_init();
    cflags_flag_t * verbose = cflags_add_bool(flags, 'v', "verbose", NULL, "enables verbose output, repeat up to 4 times for more verbosity");
    bool dump_on_fatal = false;
    cflags_add_bool(flags, 'd', "dump-on-fatal", &dump_on_fatal, "create crash dump on fatal error");

    bool help = false;
    cflags_add_bool(flags, 'h', "help", &help, "Display this help message");

    cflags_parse(flags, argc, argv);

    if (help || flags->argc != 1) {
        usage(flags);
        return 0;
    }
    log_set_verbosity(verbose->count);
    if (dump_on_fatal) {
        log_set_fatal_handler(ps1_create_crash_dump);
    }

    ps1_system_init();
    ps1_system_loop();
}