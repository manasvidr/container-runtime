#define _GNU_SOURCE
#include <sched.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/mount.h>
#include <sys/wait.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <signal.h>

#define STACK_SIZE (1024 * 1024)
#define SOCKET_PATH "/tmp/engine_socket"

static char stack[STACK_SIZE];

struct container {
    char name[50];
    pid_t pid;
};

struct container containers[10];
int container_count = 0;

// -------- Container --------
int run_container(void *arg) {
    char *rootfs = (char *)arg;

    // Logging output
    for (int i = 0; i < 5; i++) {
        printf("Log entry %d\n", i);
        sleep(1);
    }

    if (chroot(rootfs) != 0) {
        perror("chroot failed");
        exit(1);
    }

    chdir("/");
    mount("proc", "/proc", "proc", 0, NULL);

    char *args[] = {"/bin/sh", NULL};
    execvp(args[0], args);

    return 1;
}

// -------- Start (with logging) --------
void start_container(char *name) {
    char rootfs[100];
    sprintf(rootfs, "rootfs-%s", name);

    int fd[2];
    pipe(fd);

    pid_t pid = fork();

    if (pid == 0) {
        // CHILD
        close(fd[0]);

        dup2(fd[1], STDOUT_FILENO);
        dup2(fd[1], STDERR_FILENO);

        clone(run_container, stack + STACK_SIZE,
              CLONE_NEWPID | CLONE_NEWUTS | CLONE_NEWNS | SIGCHLD,
              rootfs);

        exit(0);
    }

    // PARENT (supervisor)
    close(fd[1]);

    sleep(2);  // allow logs to be generated

    char buffer[1024];
    int n = read(fd[0], buffer, sizeof(buffer));

    if (n > 0) {
        FILE *log = fopen("container.log", "a");
        fwrite(buffer, 1, n, log);
        fclose(log);
    }

    strcpy(containers[container_count].name, name);
    containers[container_count].pid = pid;
    container_count++;

    printf("Started %s (PID %d)\n", name, pid);
}

// -------- PS --------
void list_containers() {
    printf("Running containers:\n");
    for (int i = 0; i < container_count; i++) {
        printf("%s - PID %d\n", containers[i].name, containers[i].pid);
    }
}

// -------- Stop --------
void stop_container(char *name) {
    for (int i = 0; i < container_count; i++) {
        if (strcmp(containers[i].name, name) == 0) {

            kill(containers[i].pid, SIGKILL);

            // Prevent zombie processes
            waitpid(containers[i].pid, NULL, 0);

            printf("Stopped %s\n", name);
            return;
        }
    }
    printf("Container not found\n");
}

// -------- Supervisor --------
void run_supervisor() {
    int server_fd, client_fd;
    struct sockaddr_un addr;

    unlink(SOCKET_PATH);

    server_fd = socket(AF_UNIX, SOCK_STREAM, 0);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    bind(server_fd, (struct sockaddr*)&addr, sizeof(addr));
    listen(server_fd, 5);

    printf("Supervisor running...\n");

    while (1) {
        client_fd = accept(server_fd, NULL, NULL);

        char buffer[100] = {0};
        read(client_fd, buffer, 100);

        char cmd[20], name[20];
        sscanf(buffer, "%s %s", cmd, name);

        if (strcmp(cmd, "start") == 0) {
            start_container(name);
        } else if (strcmp(cmd, "ps") == 0) {
            list_containers();
        } else if (strcmp(cmd, "stop") == 0) {
            stop_container(name);
        }

        close(client_fd);
    }
}

// -------- Client --------
void send_command(char *cmd) {
    int sock;
    struct sockaddr_un addr;

    sock = socket(AF_UNIX, SOCK_STREAM, 0);

    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect failed (is supervisor running?)");
        return;
    }

    write(sock, cmd, strlen(cmd));
    close(sock);
}

// -------- Main --------
int main(int argc, char *argv[]) {
    if (argc < 2) {
        printf("Usage:\n");
        printf("./engine supervisor\n");
        printf("./engine start <name>\n");
        printf("./engine ps\n");
        printf("./engine stop <name>\n");
        return 1;
    }

    if (strcmp(argv[1], "supervisor") == 0) {
        run_supervisor();
    } else if (strcmp(argv[1], "start") == 0) {
        char cmd[50];
        sprintf(cmd, "start %s", argv[2]);
        send_command(cmd);
    } else if (strcmp(argv[1], "ps") == 0) {
        send_command("ps");
    } else if (strcmp(argv[1], "stop") == 0) {
        char cmd[50];
        sprintf(cmd, "stop %s", argv[2]);
        send_command(cmd);
    }

    return 0;
}