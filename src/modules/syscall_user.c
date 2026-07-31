/**
Modulo di test per verificare la gestione degli accessi in user mode su Zephyr RTOS. 
Analizza il comportamento delle syscall con puntatori validi e non validi, utilizzando memory domain 
e permessi sugli oggetti kernel per dimostrare i meccanismi di protezione della memoria tra user space e kernel space.

** Booting Zephyr OS build v4.4.0-9180-g9885b002c1bd ***
=== Test Zephyr: avvio ===
Modulo attivo : syscall
Modulo syscall_ptr attivo
Sono in user mode = 1
buffer=0x20000040
Chiamata con buffer valido
Buffer valido: 0
 
 
Chiamata con puntatore NON valido...
scrittura diretta
***** MPU FAULT *****
  Data Access Violation
  MMFAR Address: 0x20000040
r0/a1:  0x0800798d  r1/a2:  0x00000000  r2/a3:  0x00000055
r3/a4:  0x20000040 r12/ip:  0x080059bf r14/lr:  0x080008ed
 xpsr:  0x21000000
Faulting instruction address (r15/pc): 0x080008f2

 */



#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include <zephyr/app_memory/app_memdomain.h>
#include <zephyr/sys/libc-hooks.h>  
#include "modules.h"


K_APPMEM_PARTITION_DEFINE(protected_part);
K_APP_BMEM(protected_part) uint8_t buffer_protetto[16];
struct k_mem_domain dom_syscall;
K_MSGQ_DEFINE(my_msgq, sizeof(uint32_t), 4, 4);
K_THREAD_STACK_DEFINE(syscall_stack, 1024);
struct k_thread syscall_thread;

static void syscall_user_thread(void *p1, void *p2, void *p3)
{
    ARG_UNUSED(p1); ARG_UNUSED(p2); ARG_UNUSED(p3);
    printk("Sono in user mode = %d\n", k_is_user_context());
    printk("buffer=%p\n", buffer_protetto);
    uint32_t buffer_valido;
    int ret;
    printk("Chiamata con buffer valido\n");
    ret = k_msgq_get(&my_msgq, &buffer_valido, K_NO_WAIT);
    printk("Buffer valido: %d\n \n \n", ret);
    printk("Chiamata con puntatore NON valido...\n");
    //ret = k_msgq_get(&my_msgq, buffer_protetto, K_NO_WAIT); Non funziona
    printk("scrittura diretta\n");
    buffer_protetto[0] = 0x55;
    printk("OK\n");
}

void module_syscall_ptr_run(void)
{
    int r;
    struct k_mem_partition *dom_parts[] = {
        #if Z_LIBC_PARTITION_EXISTS
        &z_libc_partition,
        #endif
    };
    printk("Modulo syscall_ptr attivo\n");
    uint32_t val = 42;
    k_msgq_put(&my_msgq, &val, K_NO_WAIT);
    r = k_mem_domain_init(&dom_syscall, ARRAY_SIZE(dom_parts), dom_parts);
    __ASSERT(r == 0, "k_mem_domain_init() fallita: %d", r);
    if (r != 0) {
        return;
    }
    k_tid_t tid = k_thread_create(&syscall_thread, syscall_stack, K_THREAD_STACK_SIZEOF(syscall_stack), syscall_user_thread, NULL, NULL, NULL, 5, K_USER, K_FOREVER);
    k_thread_access_grant(tid, &my_msgq);
    r = k_mem_domain_add_thread(&dom_syscall, tid);
    __ASSERT(r == 0, "k_mem_domain_add_thread() fallita: %d", r);
    if (r != 0) {
        return;
    }
    k_thread_start(tid);
    k_thread_join(tid, K_FOREVER);
}