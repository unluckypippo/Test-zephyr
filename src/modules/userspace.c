/*
    Questo esercizio è un test per vedere come la MPU blocca l'accesso ad elementi protetti ad un 
    thread in modalità utente.
    Il risultato ottenuto è il seguente :
    ===============================================================

    --- Miniterm on /dev/ttyACM0  115200,8,N,1 ---
    --- Quit: Ctrl+] | Menu: Ctrl+T | Help: Ctrl+T followed by Ctrl+H ---
    ng Zephyr OS build v4.4.0-9180-g9885b002c1bd ***
    === Test Zephyr: avvio ===
    Modulo attivo : userspace
    Modulo UserSpace attivo: Test accesso negato
    thread in user mode
    ***** MPU FAULT *****
    Data Access Violation
    MMFAR Address: 0x20001f60
    r0/a1:  0x00000000  r1/a2:  0x00000000  r2/a3:  0x00000000
    r3/a4:  0x00000014 r12/ip:  0x080074c5 r14/lr:  0x08000f6b
    xpsr:  0x61000000
    Faulting instruction address (r15/pc): 0x08000f6a

    ============================================================
*/



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

#define PRIORITY -1

struct k_thread user_id;
K_THREAD_STACK_DEFINE(stack, 1024);
static void user_thread (void *p1, void *p2, void *p3){
    ARG_UNUSED(p2);
    ARG_UNUSED(p3);
    int *ptr = (int *)p1;
    printk("thread in user mode\n");
    while(true){
        *ptr ++;
        printk(" %d -", *ptr);
        k_msleep(500);
    }
}



void module_user_run(void){
    printk("Modulo UserSpace attivo: Test accesso negato\n");
    int var = 5;

    k_thread_create(&user_id, stack, K_THREAD_STACK_SIZEOF(stack), user_thread, &var, NULL, NULL, PRIORITY, K_USER, K_MSEC(0));
    k_sleep(K_FOREVER);
}