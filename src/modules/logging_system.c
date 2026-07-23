#include <zephyr/kernel.h>
#include <zephyr/logging/log.h>
#include "modules.h"

LOG_MODULE_REGISTER(logging_system, LOG_LEVEL_WRN); // Poicé il livello settato è warning, i log info e debug vengono rimossi dal binario

void module_log_run (void){   
    LOG_WRN("log warning");
    LOG_ERR("log errore");
    LOG_INF("log informazioni");
    LOG_DBG("log debug");
    k_sleep(K_MSEC(100));
    return;
}