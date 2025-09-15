# 🧠 ESP32 + Gemini AI - Guida Completa ai Progetti

Questa guida spiega in dettaglio come funzionano i due progetti ESP32 + Gemini AI presenti in questo repository, le loro differenze e come utilizzarli.

---

## 📋 Panoramica dei Progetti

### 🔹 ESP32 Gemini 1.0 (Versione Base)
**Cartella:** `Esp32_Gemini_1_0/`  
**File principale:** `Esp32_Gemini_1_0.ino`

### 🔹 ESP32 Gemini 2.0 (Versione Avanzata)
**Cartella:** `Esp32 Gemini 2.0/GeminiAi2.0/`  
**File principale:** `GeminiAi2.0.ino`

---

## 🚀 ESP32 Gemini 1.0 - Versione Base

### 📌 Caratteristiche Principali
- **Semplicità**: Implementazione base per iniziare con Gemini AI
- **Funzionalità**: Invio di domande tramite Serial Monitor e ricezione risposte
- **Modello**: Utilizza `gemini-2.0-flash`
- **Configurazione**: Parametri fissi nel codice

### 🔧 Come Funziona

#### 1. **Connessione Wi-Fi**
```cpp
const char* ssid = "";        // Inserire il nome della rete Wi-Fi
const char* password = "";    // Inserire la password Wi-Fi
```

#### 2. **Configurazione API Gemini**
```cpp
const char* apiKey = "";      // Inserire la chiave API di Google Gemini
```

#### 3. **Endpoint API**
```cpp
const String endpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + String(apiKey);
```

#### 4. **Flusso di Funzionamento**
1. **Setup**: Connessione Wi-Fi e inizializzazione SSL
2. **Loop**: Lettura input dal Serial Monitor
3. **Invio**: POST request all'API Gemini con la domanda
4. **Parsing**: Estrazione della risposta dal JSON
5. **Output**: Visualizzazione della risposta sul Serial Monitor

#### 5. **Struttura JSON della Richiesta**
```json
{
  "contents": [
    {
      "parts": [
        {
          "text": "La tua domanda qui"
        }
      ]
    }
  ]
}
```

### ⚙️ Configurazione e Utilizzo

1. **Prerequisiti**:
   - Scheda ESP32
   - Arduino IDE con librerie: `WiFi`, `WiFiClientSecure`, `HTTPClient`, `ArduinoJson`
   - Chiave API Google Gemini

2. **Setup**:
   - Inserire credenziali Wi-Fi nelle variabili `ssid` e `password`
   - Inserire la chiave API Gemini nella variabile `apiKey`
   - Caricare il codice sull'ESP32

3. **Utilizzo**:
   - Aprire il Serial Monitor (115200 baud)
   - Scrivere una domanda e premere INVIO
   - Attendere la risposta di Gemini

---

## 🚀 ESP32 Gemini 2.0 - Versione Avanzata

### 📌 Caratteristiche Principali
- **Parametri Dinamici**: Modifica dei parametri senza ricompilare
- **Comandi Speciali**: Sistema di comandi per configurazione runtime
- **Gestione Errori**: Retry automatico e gestione errori migliorata
- **Pulizia Testo**: Rimozione automatica di formattazione Markdown
- **Monitoraggio**: Controllo memoria e diagnostica

### 🔧 Come Funziona

#### 1. **Parametri Configurabili**
```cpp
uint32_t maxOutputTokens = 2048;    // Lunghezza massima risposta
float    temperature     = 0.7;     // Creatività (0.0-2.0)
float    topP            = 0.9;     // Probabilità cumulativa
String   modelRuntime    = "gemini-2.5-flash";  // Modello AI
```

#### 2. **Sistema di Comandi Speciali**
- `/tokens N` - Imposta numero massimo di token (es. `/tokens 4096`)
- `/temp X.Y` - Imposta creatività (es. `/temp 0.3`)
- `/topP X.Y` - Imposta probabilità (es. `/topP 0.95`)
- `/model nome` - Cambia modello (es. `/model gemini-1.5-flash`)
- `/heap` - Mostra memoria libera
- `/help` - Mostra lista comandi

#### 3. **Gestione Avanzata degli Errori**
```cpp
const uint8_t MAX_RETRIES = 2;        // Numero tentativi
const uint32_t HTTP_TIMEOUT_MS = 25000;  // Timeout esteso
```

#### 4. **Pulizia Automatica del Testo**
La funzione `cleanText()` rimuove:
- Blocchi di codice ```...```
- Backtick singoli `
- Asterischi decorativi * e **
- Spazi e newline eccessive

#### 5. **Struttura JSON Avanzata**
```json
{
  "generationConfig": {
    "maxOutputTokens": 2048,
    "temperature": 0.7,
    "topP": 0.9
  },
  "contents": [
    {
      "parts": [
        {
          "text": "La tua domanda qui"
        }
      ]
    }
  ]
}
```

### ⚙️ Configurazione e Utilizzo

1. **Setup Iniziale**:
   - Stesse librerie della versione 1.0
   - Configurare `SSID`, `PASSWORD`, `API_KEY`

2. **Utilizzo Base**:
   - Scrivere domande normalmente nel Serial Monitor
   - Le risposte sono automaticamente pulite dalla formattazione

3. **Utilizzo Avanzato**:
   ```
   /tokens 4096          # Aumenta lunghezza risposta
   /temp 0.3             # Riduce creatività per risposte più precise
   /model gemini-1.5-pro # Cambia a modello più potente
   Spiegami la fisica quantistica
   ```

---

## 🔄 Confronto tra le Versioni

| Caratteristica | Gemini 1.0 | Gemini 2.0 |
|----------------|------------|------------|
| **Complessità** | Base | Avanzata |
| **Parametri** | Fissi | Dinamici |
| **Comandi** | Solo domande | Comandi speciali |
| **Gestione Errori** | Base | Avanzata con retry |
| **Pulizia Testo** | No | Sì |
| **Monitoraggio** | No | Memoria e diagnostica |
| **Timeout** | 10 secondi | 25 secondi |
| **Modelli** | Solo gemini-2.0-flash | Cambiabili runtime |

---

## 🛠️ Risoluzione Problemi Comuni

### ❌ Errori di Connessione
- Verificare credenziali Wi-Fi
- Controllare la connessione internet
- Verificare che l'API key sia valida

### ❌ Errori HTTP
- **HTTP 400**: Richiesta malformata, controllare il JSON
- **HTTP 401**: API key non valida o scaduta
- **HTTP 429**: Troppi request, attendere
- **HTTP 500**: Errore server Google, riprovare

### ❌ Errori di Parsing JSON
- Aumentare la dimensione del buffer `DynamicJsonDocument`
- Verificare che la risposta sia JSON valido

### ❌ Memoria Insufficiente
- Usare `/heap` per monitorare la memoria
- Ridurre `maxOutputTokens`
- Riavviare l'ESP32 se necessario

---

## 🔐 Note di Sicurezza

⚠️ **IMPORTANTE**: 
- **MAI** pubblicare le credenziali Wi-Fi o l'API key su GitHub
- Il codice usa `setInsecure()` per i certificati SSL - **NON** usare in produzione
- Per uso produttivo, implementare validazione certificati SSL appropriata

---

## 📚 Risorse Utili

- [Documentazione Google Gemini API](https://ai.google.dev/docs)
- [Arduino ESP32 Core](https://github.com/espressif/arduino-esp32)
- [Libreria ArduinoJson](https://arduinojson.org/)
- [Canale YouTube Ingeimaks](https://www.youtube.com/@Ingeimaks)

---

## 📄 Licenza

Entrambi i progetti sono rilasciati sotto licenza MIT. Puoi liberamente modificare, distribuire e utilizzare il codice per i tuoi progetti.

---

*Creato da Ingeimaks - Agosto 2025*

✍️ Segui il canale [Ingeimaks](https://www.youtube.com/@Ingeimaks) per nuovi progetti ESP32 e altri contenuti di elettronica, Arduino e stampa 3D!
