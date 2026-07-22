# Zephyr playground
Freestanding app creata con lo scopo di sperimentare le varie funzionalità di zephyr RTOS.
Le funzionalità sono isolate tra loro e attivabili tramite kconfig.

## Obbiettivo
- Prendere confidenza con il flusso di lavoro west + CMake * Kconfig.
- Provare i sottosistemi principali (GPIO, thread, log, ...)
- Creare un punto di riferimento con esempi funzionanti.

## Hardware Utilizzato
Nucleo L552ZE-Q della STMicroelettronics

## Struttura del Progetto
```bash

Test-zephyr/
├── CMakeLists.txt          # build; include i moduli con target_sources_ifdef
├── Kconfig                 # opzioni CONFIG_APP_MODULE_* (+ source Kconfig.zephyr)
├── prj.conf                # quale modulo è attivo in questa build
├── app.overlay		    # modifica configurazioni hardware senza modificare il DT originale
├── README.md
└── src/
    ├── main.c              # dispatcher: chiama il modulo abilitato
    ├── modules.h           # prototipi
    └── modules/
        ├── countdown.c     # countdown da pulsante + LED rosso/blu alternati
        ├── led_blink.c     # blink base
		├── temp_read.c	    # stampa la temperatura del chip  
        └── button_irq.c    # pulsante a interrupt,  debounce, semaforo

```


## Build ed Esecuzione
```bash

west build -p always -b nucleo_l55ze_q/stm32l552xx <path/to/app>

west flash --runner openocd

python -m serial.tools.miniterm /dev/ttyACM0 115200

```
