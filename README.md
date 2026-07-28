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
├── CMakeLists.txt             # build; include i moduli con target_sources_ifdef
├── Kconfig                    # opzioni CONFIG_APP_MODULE_* (+ source Kconfig.zephyr)
├── prj.conf                   # quale modulo è attivo in questa build
├── app.overlay		           # modifica configurazioni hardware senza modificare il DT originale
├── README.md
└── src/
    ├── main.c                 # dispatcher: chiama il modulo abilitato
    ├── modules.h              # moduli
    └── modules/
        ├── countdown.c        # countdown da pulsante + LED rosso/blu alternati
        ├── blink.c            # blink base
		├── temp_read.c	       # temperatura del die via Sensor API  
        ├── button_i.c         # pulsante a interrupt,  debounce, semaforo
        ├── thread_mg.c        # thread statico con K_THREAD_DEFINE, blink + contatore
        └── logging_system.c   # messaggi log a vari livelli (errori, warning, info, debug)
```


## Build ed Esecuzione (TZEN = 0)
Per modalità secure, con Trust Zone disabilitata.

```bash

west build -p always -b nucleo_l552ze_q/stm32l552xx <path/to/app>

west flash --runner openocd

python -m serial.tools.miniterm /dev/ttyACM0 115200

```
## Build ed Esecuzione (TZEN = 1)
In questa modalità Zephyr gira nel mondo non-secure sotto TF-M (Trusted Firmware-M), che occupa il mondo secure. Il target di board cambia con il suffisso `/ns` e il build produce tre immagini: BL2 (MCUboot), TF-M secure e Zephyr non-secure.
 
### Prerequisiti 
- `STM32_Programmer_CLI` nel PATH (incluso in STM32CubeProgrammer o STM32CubeIDE).
- udev rules per l'ST-LINK installate.
- Board provisionata con TrustZone attiva (`TZEN=1`). Verifica con:
```bash
  STM32_Programmer_CLI -c port=SWD mode=HotPlug -ob displ | grep -E "TZEN|RDP"
```
  Se `TZEN` non è `0x1`, esegui il provisioning con lo script generato dopo la prima build (vedi sotto).
 
### Build
Aggiungere flag /ns alla target board.
```bash
 
west build -p always -b nucleo_l552ze_q/stm32l552xx/ns <path/to/app>
 
```
 
### Provisioning (solo la prima volta o se TZEN non è attivo)
Dopo la build viene generato uno script che imposta le option bytes secure. Va eseguito una sola volta per portare la board in stato TF-M (nei cicli di sviluppo successivi si passa direttamente al flash).
```bash
 
./build/tfm/api_ns/regression.sh
 
```
 
### Flash ed esecuzione
Con TF-M si usa il runner `stm32cubeprogrammer` (NON openocd per il flash):
```bash
 
west flash --runner stm32cubeprogrammer
 
python -m serial.tools.miniterm /dev/ttyACM0 115200
 
```
Output atteso: `Starting bootloader` → `Booting TF-M` → `Booting Zephyr OS` → il modulo attivo.
 
### Ciclo di sviluppo (board già provisionata)
Una volta che la board è a `TZEN=1`, il ciclo è solo build + flash, senza ripetere `regression.sh`:
```bash
 
west build -p always -b nucleo_l552ze_q/stm32l552xx/ns <path/to/app>
west flash --runner stm32cubeprogrammer
 
```
 
### Debug con TF-M
Per il flash si usa `stm32cubeprogrammer`, ma per il debug serve OpenOCD:
```bash
 
west debug --runner openocd
 
```
Si debugga il codice non-secure; dentro le chiamate a TF-M (mondo secure) il debugger non entra, per design della TrustZone.
 
### Tornare alla modalità secure (disabilitare TrustZone)
Un binario senza `/ns` non fa boot con `TZEN=1`. Per tornare indietro serve la regressione TZEN completa (che fa un mass erase della flash):
```bash
 
STM32_Programmer_CLI -c port=SWD mode=HotPlug -tzenreg
 
```