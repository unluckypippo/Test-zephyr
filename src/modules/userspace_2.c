/*
    Nuovo nucleo Test accesso ad una partizione di memoria protetta;
    === Test Zephyr: avvio ===
    Modulo attivo : userspace
    Modulo UserSpace attivo: Test accesso partizione di memoria da user thread
    thread con accesso 
    thread in user mode 
    valore = 3 
    Thread senza accesso
    thread in user mode 
    ***** MPU FAULT *****
    Data Access Violation
    MMFAR Address: 0x20000040
    r0/a1:  0x080486fe  r1/a2:  0x00000000  r2/a3:  0x00000000
    r3/a4:  0x20000040 r12/ip:  0x0804699d r14/lr:  0x08041c59
    xpsr:  0x61000000
    Faulting instruction address (r15/pc): 0x08041c5c
*/


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/app_memory/app_memdomain.h>
#include <zephyr/sys/libc-hooks.h>  
#include "modules.h"

K_APPMEM_PARTITION_DEFINE(my_part);
K_APP_DMEM(my_part) int var = 3; 
struct k_mem_domain dom;


K_THREAD_STACK_DEFINE(my_stack, 1024);
K_THREAD_STACK_DEFINE(my_stack2, 1024);
struct k_thread my_thread;
struct k_thread my_thread2;

static void user_thread (void *p1, void *p2, void *p3){
    printk("thread in user mode \n");   
    printk("valore = %d \n", var); 
}


void module_user_run(void)
{
    int ret;
    struct k_mem_partition *dom_parts[] = {
        #if Z_LIBC_PARTITION_EXISTS
        &z_libc_partition,
        #endif
        &my_part,
    };
    printk("Modulo UserSpace attivo: Test accesso partizione di memoria da user thread\n");
    ret = k_mem_domain_init(&dom, ARRAY_SIZE(dom_parts), dom_parts);
    __ASSERT(ret == 0, "k_mem_domain_init() fallita: %d", ret);
    if (ret != 0) {
        printk("Errore init dominio (%d)\n", ret);
        return;
    }
    printk("thread con accesso \n");
    k_tid_t user_thread_id_2 = k_thread_create(&my_thread2, my_stack2,K_THREAD_STACK_SIZEOF(my_stack2), user_thread, NULL, NULL, NULL, 5, K_USER, K_FOREVER);
    ret = k_mem_domain_add_thread(&dom, user_thread_id_2);
    __ASSERT(ret == 0, "k_mem_domain_add_thread() fallita: %d", ret);
    if (ret != 0) {
        printk("Errore add_thread (%d)\n", ret);
        return;
    }
    k_thread_start(user_thread_id_2);
    k_thread_join(user_thread_id_2, K_FOREVER);
    k_msleep(1000);

    printk("Thread senza accesso\n");
    k_tid_t user_thread_id = k_thread_create(&my_thread, my_stack, K_THREAD_STACK_SIZEOF(my_stack), user_thread, NULL, NULL, NULL, 5, K_USER, K_NO_WAIT);
    k_thread_start(user_thread_id);
 
    k_sleep(K_FOREVER);
}
