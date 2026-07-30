/*
    Questo esercizio è un test per vedere come la MPU blocca l'accesso ad elementi kernel ad un 
    thread in modalità utente e come si può darne l'accesso.
    Il risultato ottenuto è il seguente :
    ===============================================================

    --- Miniterm on /dev/ttyACM0  115200,8,N,1 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ng Zephyr OS build v4.4.0-9180-g9885b002c1bd ***
    === Test Zephyr: avvio ===
    Modulo attivo : userspace
    Modulo UserSpace attivo: Test accesso kernel object da user thread
    thread in user mode con accesso
    ______________________________________
    Il thread user_thread_1 ha accesso al semaforo  
    ======================================
    thread in user mode senza accesso
    ______________________________________
    [00:00:00.000,000] ␛[1;31m<err> os: thread 0x200005d8 (1) does not have permission on k_sem 0x2000020c␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: permission bitmap
                                00 00                                            |..               ␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: syscall z_vrfy_k_sem_take failed check: access denied␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: r0/a1:  0x00000000  r1/a2:  0x00000000  r2/a3:  0x00000000␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: r3/a4:  0x00000000 r12/ip:  0x00000000 r14/lr:  0x00000000␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os:  xpsr:  0x00000000␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: Faulting instruction address (r15/pc): 0x0800d55f␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: >>> ZEPHYR FATAL ERROR 3: Kernel oops on CPU 0␛[0m
    [00:00:00.000,000] ␛[1;31m<err> os: Current thread: 0x200005d8 (unknown)␛[0m
    [00:00:00.032,000] ␛[1;31m<err> os: Halting system
    ␛[0m
    ============================================================
*/



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

K_SEM_DEFINE(sem, 0, 1);
#define STACK_SIZE 1024

static void user_thread_1 (void *p1, void *p2, void *p3){
    printk("thread in user mode con accesso\n");
    printk("______________________________________\n");
    if (k_sem_take(&sem, K_FOREVER) == 0)
        printk(" Il thread user_thread_1 ha accesso al semaforo  \n");
    printk("======================================\n");
}
static void user_thread_2 (void *p1, void *p2, void *p3){
    printk("thread in user mode senza accesso\n");
    printk("______________________________________\n");
    k_sem_take(&sem, K_FOREVER);
}

K_THREAD_DEFINE(user_thread_id_1, STACK_SIZE, user_thread_1, NULL, NULL, NULL, 5, K_USER, 0);
K_THREAD_DEFINE(user_thread_id_2, STACK_SIZE, user_thread_2, NULL, NULL, NULL, 6, K_USER, 0);

void module_user_run(void){
    printk("Modulo UserSpace attivo: Test accesso kernel object da user thread\n");
    k_object_access_grant(&sem, user_thread_id_1);     
    k_sem_give(&sem);
    k_sleep(K_FOREVER);
}