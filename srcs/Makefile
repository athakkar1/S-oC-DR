all: program1 program2 program3

program1:
        gcc hackRF_TCP.c shared_memory.c -o hack -I/usr/include -L/usr/lib -lhackrf -pthread

program2:
        gcc tcp_client.c shared_memory.c -o tcp -I/usr/include -L/usr/lib -lrt -pthread

program3:
        gcc destroy_mem.c shared_memory.c -o destroy
