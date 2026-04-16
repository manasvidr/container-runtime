# Multi-Container Runtime (Simplified Implementation)

A lightweight Linux container runtime in C demonstrating basic container isolation, supervisor control, logging, and scheduling behavior.

---

## 1. Team Information

| Name        | SRN           |
| ----------- | ------------- |
| Manasvi D R | PES1UG24CS262 |
| Manoj Kumar | PES1UG24CS264 |

---

## 2. Build and Run Instructions

### Prerequisites

Ubuntu 22.04/24.04 system

```bash
sudo apt update
sudo apt install -y build-essential linux-headers-$(uname -r)
```

---

### Build

```bash
gcc engine.c -o engine
make
```

---

### Run Supervisor

```bash
./engine supervisor
```

---

### Run Commands (New Terminal)

```bash
./engine start alpha
./engine start beta
./engine ps
./engine stop alpha
```

---

### Load Kernel Module

```bash
sudo insmod monitor.ko
dmesg | tail
```

---

## 3. Demo Screenshots

### Screenshot 1 — Multi-Container Supervision

![Multi-container](screenshots/01_multi_container.png)

Two containers (alpha and beta) running simultaneously under a single supervisor process. Each container is created using `clone()` with namespace isolation.

---

### Screenshot 2 — Metadata Tracking

![PS output](screenshots/02_ps_output.png)

Output of the `engine ps` command showing active containers and their corresponding host PIDs managed by the supervisor.

---

### Screenshot 3 — Logging

![Logging](screenshots/03_logging.png)

Container output captured through pipe redirection and written to `container.log`, demonstrating logging from container to host.

---

### Screenshot 4 — CLI and IPC

![CLI IPC](screenshots/04_cli_ipc.png)

CLI commands sent to the supervisor using a UNIX domain socket. The supervisor processes commands such as start, ps, and stop.

---

### Screenshot 5 — Soft Limit (Monitoring)

![Soft limit](screenshots/05_soft_limit.png)

Kernel module generating warning messages in `dmesg`, demonstrating monitoring of running processes.

---

### Screenshot 6 — Hard Limit (Enforcement)

![Hard limit](screenshots/06_hard_limit.png)

Kernel module enforcing control by sending SIGKILL to a selected process, demonstrating kernel-level process management.

---

### Screenshot 7 — Scheduling Experiment

![Scheduling](screenshots/07_scheduling.png)

CPU scheduling experiment using different `nice` values. Lower nice values receive higher CPU priority, resulting in faster execution.

---

### Screenshot 8 — Clean Teardown

![Teardown](screenshots/08_teardown.png)

System state after stopping containers, showing no leftover or zombie processes, confirming clean teardown.

---

## 4. System Design Overview

### Container Execution

Containers are created using `clone()` with Linux namespaces:

* PID namespace → isolates process IDs
* UTS namespace → isolates hostname
* Mount namespace → isolates filesystem

Each container performs:

* `chroot()` into its root filesystem
* mounts `/proc`
* executes `/bin/sh`

---

### Supervisor and Control Flow

A central supervisor process:

* Listens on a UNIX domain socket (`/tmp/engine_socket`)
* Handles commands:

  * `start`
  * `ps`
  * `stop`

The CLI acts as a client and sends commands to the supervisor.

---

### Logging Mechanism

Container output is redirected using pipes:

* stdout/stderr → pipe
* supervisor reads pipe
* writes output to `container.log`

This demonstrates a basic logging pipeline between container and host.

---

### Kernel Monitoring (Demonstration)

A simple kernel module is used to:

* iterate over system processes
* print kernel logs (`dmesg`)
* demonstrate enforcement behavior

---

### Scheduling Experiment

Workloads provided:

* `cpu_hog` → CPU intensive loop
* `memory_hog` → allocates memory continuously
* `log_spam` → continuous logging

Using `nice`:

```bash
nice -n -10 ./workloads/cpu_hog
nice -n 10 ./workloads/cpu_hog
```

---

## 5. Design Decisions

* `clone()` enables namespace-based isolation
* `chroot()` provides simple filesystem separation
* Pipes used for logging simplicity
* UNIX sockets used for IPC
* Kernel module kept minimal for demonstration

---

## 6. Observations

* Multiple containers run concurrently
* Logging is successfully captured
* Scheduling priority affects execution time
* Kernel module demonstrates process control

---

## 7. Limitations

* No full RSS-based memory enforcement
* Logging is not a full bounded-buffer system
* CLI arguments are simplified
* Kernel module uses demonstration logic
* This implementation focuses on demonstrating core operating system concepts in a simplified and educational manner rather than providing a production-grade container runtime.

---

## 8. Conclusion

This project demonstrates core OS concepts:

* process isolation
* inter-process communication
* containerization basics
* scheduling behavior
* kernel interaction
