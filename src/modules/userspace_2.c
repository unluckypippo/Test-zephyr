/*
    Nuovo nucleo Test accesso ad una partizione di memoria protetta;
*/


#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/app_memory/app_memdomain.h>
#include "modules.h"

K_APPMEM_PARTITION_DEFINE(my_part);
K_APP_DMEM(my_part) int var = 3; 
struct k_mem_domain dom;


K_THREAD_STACK_DEFINE(my_stack, 1024);
struct k_thread my_thread;

static void user_thread (void *p1, void *p2, void *p3){
    printk("thread in user mode \n");   
    int *ptr = (int *)p1;
    printk("%d", *ptr); 
    k_msleep(2000);
}

void k_sys_fatal_error_handler(unsigned int reason, const struct arch_esf *esf)
{
    ARG_UNUSED(esf);
    printk("Gestione fault del thread, reason=%u -> abort del thread\n", reason);
}

void module_user_run(void){
    k_mem_domain_init(&dom, 0, NULL);
    k_mem_domain_add_partition(&dom, &my_part);
    printk("Modulo UserSpace attivo: Test accesso partizione di memoria da user thread\n");
    printk("Thread senza accesso\n");
    k_tid_t user_thread_id = k_thread_create(&my_thread, my_stack, K_THREAD_STACK_SIZEOF(my_stack), user_thread, &var, NULL, NULL, 5, K_USER, K_NO_WAIT);
    k_thread_start(user_thread_id);
    k_sleep(K_FOREVER);   
}