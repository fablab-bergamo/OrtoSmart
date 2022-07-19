# OrtoSmart
Repository per il workshop di monitoraggio ambientale di Place Me Now
![locandina](locandina.jpg)
## Setup ESP32
Per questo workshop utilizziamo ESP32 con il framework Arduino. Questi sono gli step da seguire:

### Preparazione dell'ambiente di sviluppo
1. Scarica e installa Arduino IDE. Apri l'IDE al termine dell'installazione
1. Aggiungi il repository di ESP32: `File -> Impostazioni -> URL aggiuntive per il gestore schede` e incolla questa stringa: `https://raw.githubusercontent.com/espressif/arduino-esp32/gh-pages/package_esp32_index.json` 
1. Installa la toolchain di ESP32: `Strumenti -> Scheda -> Gestore schede...`, cerca ESP32 e installa l'ultima versione (2.0.4 al momento della scrittura)
Richiede un po' di tempo e una connessione a internet
1. Installazione delle librerie: sono necessarie diverse librerie. 
1. Seleziona la scheda ESP32: `Strumenti -> Scheda -> ESP32 Arduino -> ESP32 Dev Module`
1. Seleziona la porta COM corretta: `Strumenti -> Scheda -> Porta -> COMxx` (sostituisci xx col numer corretto. Se ce ne sono diverse e non sai quale sia, scollega la scheda e vedi quale scompare)
1. Testa che tutto funzioni correttamente: carichiamo l'esempio Blink. `File -> Esempi -> 01.Basics -> Blink`. Definisci il pin del led aggiungendo `#define LED_BUILTIN 2` prima del setup e carica il codice. La compilazione richiede un po' di tempo. Quando ha finito di compilare, vedrai comparire nel terminale in basso la scritta `Connecting......`, a questo punto premi il pulsante `BOOT` sulla scheda per iniziare il caricamento. Se è andato tutto bene, dovresti vedere il led lampeggiare sulla scheda.
1. Se sei a questo punto, tutto il setup è stato fatto correttamente, e siamo pronti a partire!

### Installazione delle librerie
#### WiFiManager
Questa è una libreria in fase di sviluppo e va quindi installata manualmente. 




## Caricamento Firmware
1. Seleziona la scheda ESP32: `Strumenti -> Scheda -> ESP32 Arduino -> ESP32 Dev Module`
1. Seleziona la porta COM corretta: `Strumenti -> Scheda -> Porta -> COMxx` (sostituisci xx col numer corretto. Se ce ne sono diverse e non sai quale sia, scollega la scheda e vedi quale scompare)
