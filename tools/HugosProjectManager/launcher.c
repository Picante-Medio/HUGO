/*
 * launcher.c
 * Lee el ejecutable y sus argumentos desde "launch.config" y lo lanza.
 * Compatible con Windows, macOS y Linux.
 *
 * Formato de launch.config (una entrada por línea):
 *   línea 1 → path al ejecutable
 *   línea 2 → arg1  (opcional)
 *   línea 3 → arg2  (opcional)
 *   línea 4 → arg3  (opcional)
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef _WIN32
    #include <windows.h>
#else
    #include <unistd.h>
    #include <sys/wait.h>
    #include <errno.h>
#endif

#define CONFIG_FILE  "launch.config"
#define MAX_LINE     4096

/* Lee una línea del archivo eliminando el salto de línea final.
   Devuelve 1 si leyó algo, 0 si EOF o error. */
static int read_line(FILE *f, char *buf, size_t size)
{
    if (!fgets(buf, (int)size, f)) return 0;

    size_t len = strlen(buf);
    /* Eliminar \n y \r\n */
    while (len > 0 && (buf[len - 1] == '\n' || buf[len - 1] == '\r'))
        buf[--len] = '\0';

    return 1;
}

int main(void)
{
    FILE *cfg = fopen(CONFIG_FILE, "r");
    if (!cfg) {
        fprintf(stderr, "No se pudo abrir '%s'\n", CONFIG_FILE);
        return EXIT_FAILURE;
    }

    char target[MAX_LINE] = {0};
    char arg1  [MAX_LINE] = {0};
    char arg2  [MAX_LINE] = {0};
    char arg3  [MAX_LINE] = {0};

    /* La primera línea es obligatoria */
    if (!read_line(cfg, target, sizeof(target)) || target[0] == '\0') {
        fprintf(stderr, "El archivo '%s' no contiene el path del ejecutable\n",
                CONFIG_FILE);
        fclose(cfg);
        return EXIT_FAILURE;
    }

    /* Las tres siguientes son opcionales */
    int has_arg1 = read_line(cfg, arg1, sizeof(arg1)) && arg1[0] != '\0';
    int has_arg2 = read_line(cfg, arg2, sizeof(arg2)) && arg2[0] != '\0';
    int has_arg3 = read_line(cfg, arg3, sizeof(arg3)) && arg3[0] != '\0';

    fclose(cfg);

/* ------------------------------------------------------------------ */
/*  Windows                                                             */
/* ------------------------------------------------------------------ */
#ifdef _WIN32

    char cmdline[32768] = {0};

    #define APPEND_QUOTED(s)                                                \
        do {                                                                \
            strncat(cmdline, "\"", sizeof(cmdline) - strlen(cmdline) - 1); \
            strncat(cmdline, (s),  sizeof(cmdline) - strlen(cmdline) - 1); \
            strncat(cmdline, "\"", sizeof(cmdline) - strlen(cmdline) - 1); \
        } while (0)

    #define APPEND_SPACE() \
        strncat(cmdline, " ", sizeof(cmdline) - strlen(cmdline) - 1)

    APPEND_QUOTED(target);
    if (has_arg1) { APPEND_SPACE(); APPEND_QUOTED(arg1); }
    if (has_arg2) { APPEND_SPACE(); APPEND_QUOTED(arg2); }
    if (has_arg3) { APPEND_SPACE(); APPEND_QUOTED(arg3); }

    STARTUPINFOA        si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    if (!CreateProcessA(NULL, cmdline, NULL, NULL, FALSE, 0, NULL, NULL, &si, &pi))
    {
        fprintf(stderr, "Error al lanzar '%s' (codigo Windows: %lu)\n",
                target, (unsigned long)GetLastError());
        return EXIT_FAILURE;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code = 0;
    GetExitCodeProcess(pi.hProcess, &exit_code);

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);

    return (int)exit_code;

/* ------------------------------------------------------------------ */
/*  POSIX (Linux / macOS)                                               */
/* ------------------------------------------------------------------ */
#else

    const char *child_argv[5];
    int idx = 0;

    child_argv[idx++] = target;
    if (has_arg1) child_argv[idx++] = arg1;
    if (has_arg2) child_argv[idx++] = arg2;
    if (has_arg3) child_argv[idx++] = arg3;
    child_argv[idx] = NULL;

    pid_t pid = fork();

    if (pid < 0) {
        perror("fork");
        return EXIT_FAILURE;
    }

    if (pid == 0) {
        execv(target, (char * const *)child_argv);
        perror("execv");
        _exit(EXIT_FAILURE);
    }

    int status = 0;
    if (waitpid(pid, &status, 0) < 0) {
        perror("waitpid");
        return EXIT_FAILURE;
    }

    if (WIFEXITED(status))
        return WEXITSTATUS(status);

    return EXIT_FAILURE;

#endif
}
