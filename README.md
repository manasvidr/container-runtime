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

(Add your 8 screenshots in /screenshots folder)

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

The module showcases how kernel-space can observe and control processes.

---

### Scheduling Experiment

Workloads provided:

* `cpu_hog` → CPU intensive loop
* `memory_hog` → allocates memory continuously
* `log_spam` → continuous logging

Using `nice`:

```bash
nice -n -10 ./cpu_hog
nice -n 10 ./cpu_hog
```

Observation:

* Lower nice value → higher CPU priority
* Higher nice value → slower execution

---

## 5. Design Decisions

### Use of clone() over fork()

`clone()` allows direct namespace creation, enabling container-like isolation.

---

### Use of chroot()

Provides simple filesystem isolation without complexity of pivot_root.

---

### Pipe-based Logging

Chosen for simplicity and reliability in transferring container output.

---

### UNIX Domain Sockets

Used for IPC between CLI and supervisor:

* lightweight
* efficient
* local communication

---

### Kernel Module Simplicity

A minimal kernel module is used to demonstrate:

* process inspection
* signal sending

This keeps the implementation understandable while still showcasing kernel interaction.

---

## 6. Observations

* Multiple containers can run simultaneously under one supervisor
* Logging output is captured successfully
* Scheduler behavior changes with `nice` values
* Kernel module demonstrates process-level control

---

## 7. Limitations

* No advanced memory tracking (RSS-based enforcement not implemented)
* Logging is not a full bounded-buffer system
* CLI arguments are simplified
* Kernel module uses demonstration logic rather than full monitoring

---

## 8. Conclusion

This project demonstrates core operating system concepts:

* process isolation
* inter-process communication
* basic containerization
* scheduling behavior
* kernel interaction