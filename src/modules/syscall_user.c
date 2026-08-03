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



L'hard fault dell'MPU è data dal tentativo di scrittura diretta in memoria protetta 
data da :
    printk("scrittura diretta\n");
    buffer_protetto[0] = 0x55;
    printk("OK\n");

Invece con ret = k_msgq_get(&my_msgq, buffer_protetto, K_NO_WAIT); la protezione scatta 
a livello di verifica della syscall, che verifica che il thread abbia i permessi per 
poter fare l'azione. 
K_OOPS da syscall non arriva mai alla chiamata illegale, la funzione di verifica della syscall 
controlla il puntatore, vede che è fuori dal memory domain, e chiama k_oops() volontariamente. 
Il kernel che decide di terminare il thread. Il "fault" viene generato in modo sintetico,
e il frame associato non corrisponde a nessun contesto di esecuzione realmente crashato, 
da qui i registri a zero e il PC fittizio 0xf9b6f7fc, che non è un indirizzo reale ma un valore 
segnaposto/sentinella: 

    Chiamata non valida
    r0/a1:  0x00000000  r1/a2:  0x00000000  r2/a3:  0x00000000
    r3/a4:  0x00000000 r12/ip:  0x00000000 r14/lr:  0x00000000
    xpsr:  0x00000000
    Faulting instruction address (r15/pc): 0xf9b6f7fc

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
    uint32_t buffer_valido;
    int ret;
    printk("Chiamata valida\n");
    ret = k_msgq_get(&my_msgq, &buffer_valido, K_NO_WAIT);
    printk("%d\n \n", ret);
    printk("Chiamata non valida\n");
    ret = k_msgq_get(&my_msgq, buffer_protetto, K_NO_WAIT); 
    /*
    printk("scrittura diretta\n");
    buffer_protetto[0] = 0x55;
    printk("OK\n");
    */
}

void module_syscall_ptr_run(void)
{
    int r;
    struct k_mem_partition *dom_parts[] = {
        #if Z_LIBC_PARTITION_EXISTS
        &z_libc_partition,
        #endif
    };
    printk("Modulo syscall_user\n");
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