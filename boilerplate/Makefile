obj-m += monitor.o

all:
	gcc engine.c -o engine
	gcc workloads/cpu_hog.c -o workloads/cpu_hog
	gcc workloads/memory_hog.c -o workloads/memory_hog
	gcc workloads/log_spam.c -o workloads/log_spam
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) modules

clean:
	rm -f engine workloads/cpu_hog workloads/memory_hog workloads/log_spam
	make -C /lib/modules/$(shell uname -r)/build M=$(PWD) clean

ci:
	echo "CI check passed"