/*
 * Esempio: produttore/consumatore con buffer circolare condiviso,
 * protetto da un mutex e sincronizzato con due condition variable.
 *
 * - Il PRODUTTORE genera numeri e li mette nel buffer.
 *   Se il buffer è pieno, aspetta finché non si libera spazio.
 * - Il CONSUMATORE preleva i numeri dal buffer e li "elabora".
 *   Se il buffer è vuoto, aspetta finché non arriva qualcosa.
 */

#include <zephyr/kernel.h>
#include <zephyr/sys/printk.h>
#include "modules.h"

#define BUFFER_SIZE   4      
#define STACK_SIZE    1024
#define PRIORITY      5

static int   buffer[BUFFER_SIZE];
static int   count;            /* numero di elementi presenti nel buffer */
static int   head;             /* indice produttore */
static int   tail;             /* indice consumatore */

K_MUTEX_DEFINE(lock);              
K_CONDVAR_DEFINE(not_full);        
K_CONDVAR_DEFINE(not_empty);       


void produttore_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    for (int item = 1; ; item++) {

        k_mutex_lock(&lock, K_FOREVER);

        while (count == BUFFER_SIZE) {
            printk("[P] buffer pieno, aspetto...\n");
            k_condvar_wait(&not_full, &lock, K_FOREVER);
        }

        buffer[head] = item;
        head = (head + 1) % BUFFER_SIZE;
        count++;
        printk("[P] prodotto %d (nel buffer: %d)\n", item, count);
        k_condvar_signal(&not_empty);
        k_mutex_unlock(&lock);
        k_msleep(200);
    }
}

void consumatore_thread(void *a, void *b, void *c)
{
    ARG_UNUSED(a); ARG_UNUSED(b); ARG_UNUSED(c);

    while (1) {

        k_mutex_lock(&lock, K_FOREVER);
        
        while (count == 0) {
            printk("    [C] buffer vuoto, aspetto...\n");
            k_condvar_wait(&not_empty, &lock, K_FOREVER);
        }
        int item = buffer[tail];
        tail = (tail + 1) % BUFFER_SIZE;
        count--;
        printf("    [C] consumato %d (nel buffer: %d)\n", item, count);
        k_condvar_signal(&not_full);
        k_mutex_unlock(&lock);
        k_msleep(500);
    }
}

K_THREAD_DEFINE(producer_id, STACK_SIZE, produttore_thread,
                NULL, NULL, NULL, PRIORITY, 0, 0);

K_THREAD_DEFINE(consumer_id, STACK_SIZE, consumatore_thread,
                NULL, NULL, NULL, PRIORITY, 0, 0);

void module_condvar_run(void){
    printk("Modulo codvar attivo\n");
    k_sleep(K_FOREVER);
}