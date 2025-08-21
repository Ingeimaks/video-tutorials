/*
  =====================================================
   Progetto: ESP32 + Gemini API (con parametri dinamici)
   Autore: Ingeimaks
   Data: Agosto 2025
  =====================================================

  📌 Descrizione:
    Questo sketch permette di interagire con i modelli Gemini
    direttamente da ESP32. L’utente può scrivere una domanda 
    nel Serial Monitor e ricevere la risposta del modello AI.  

    Oltre all’invio di prompt, il programma supporta comandi
    speciali per modificare i parametri di generazione senza
    dover ricompilare il firmware:
      - /tokens N   → imposta il numero massimo di token
      - /temp X.Y   → imposta la "creatività" (temperature)
      - /topP X.Y   → imposta la probabilità cumulativa
      - /model nome → cambia modello in runtime
      - /heap       → mostra memoria libera
      - /help       → mostra lista comandi

  ⚠️ Note importanti:
    - Non pubblicare SSID, PASSWORD e API_KEY su GitHub.
    - Questo codice accetta tutti i certificati SSL con 
      `tls.setInsecure()`. Sicuro solo per test, NON per produzione.
    - Il parsing JSON usa buffer grandi → attenzione alla RAM.

  🔧 Dipendenze:
    - Libreria WiFi (ESP32)
    - WiFiClientSecure
    - HTTPClient
    - ArduinoJson

  📄 Licenza:
    MIT License
*/

#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// ====== CONFIGURAZIONE UTENTE ======
const char* SSID     = "";
const char* PASSWORD = "";

// Chiave API Gemini (NON PUBBLICARLA!)
const char* API_KEY  = "";

// Modello Gemini
const char* MODEL    = "gemini-2.5-flash"; // puoi cambiare modello se serve

// Parametri di generazione (regola qui la "lunghezza")
const uint32_t MAX_OUTPUT_TOKENS = 2048;   // prova 1024 -> 2048 -> 4096
const float    TEMPERATURE        = 0.7;   // creatività
const float    TOP_P              = 0.9;

// Timeout HTTP (ms) per risposte lunghe
const uint32_t HTTP_TIMEOUT_MS    = 25000;

// Numero tentativi in caso di errore temporaneo/rete
const uint8_t  MAX_RETRIES        = 2;

// ====== VARIABILI GLOBALI ======
WiFiClientSecure tls;
String domandaBuffer;

// Costruzione endpoint
String buildEndpoint() {
  String url = "https://generativelanguage.googleapis.com/v1beta/models/";
  url += MODEL;
  url += ":generateContent?key=";
  url += API_KEY;
  return url;
}

// Funzioni utili
String cleanText(const String& in) {
  // Rimuove asterischi ripetuti, backtick, code block, trim spazi doppi
  String out = in;

  // Rimuovi blocchi ```...```
  int start = -1;
  while ((start = out.indexOf("```")) != -1) {
    int end = out.indexOf("```", start + 3);
    if (end == -1) { break; }
    out.remove(start, (end + 3) - start);
  }

  // Rimuovi backtick singoli `
  out.replace("`", "");

  // Collassa asterischi multipli e rimuovi * isolati decorativi
  while (out.indexOf("**") != -1) out.replace("**", "");
  while (out.indexOf("*")  != -1) out.replace("*", "");

  // Rimuovi sequenze di spazi e nuove linee eccessive
  // (semplice normalizzazione)
  out.replace("\r", "");
  // sostituisci triple newline con doppie
  while (out.indexOf("\n\n\n") != -1) out.replace("\n\n\n", "\n\n");

  // Trim bordi
  out.trim();
  return out;
}

void printHelp() {
  Serial.println("\n📌 Istruzioni:");
  Serial.println("- Digita la domanda e premi INVIO");
  Serial.println("- Comandi speciali:");
  Serial.println("  /tokens N      -> imposta maxOutputTokens (es. /tokens 4096)");
  Serial.println("  /temp X.Y      -> imposta temperature (es. /temp 0.3)");
  Serial.println("  /topP X.Y      -> imposta topP (es. /topP 0.95)");
  Serial.println("  /model nome    -> cambia modello (es. /model gemini-1.5-flash)");
  Serial.println("  /heap          -> mostra memoria libera");
  Serial.println("  /help          -> mostra questo aiuto");
  Serial.println();
}

// Parametri runtime modificabili
uint32_t maxOutputTokens = MAX_OUTPUT_TOKENS;
float    temperature     = TEMPERATURE;
float    topP            = TOP_P;
String   modelRuntime    = MODEL;

bool handleCommand(const String& line) {
  if (!line.startsWith("/")) return false;
  if (line.startsWith("/tokens ")) {
    long v = line.substring(8).toInt();
    if (v > 0) {
      maxOutputTokens = (uint32_t)v;
      Serial.printf("✅ maxOutputTokens impostato a %lu\n", (unsigned long)maxOutputTokens);
    } else {
      Serial.println("⚠️ Valore non valido.");
    }
  } else if (line.startsWith("/temp ")) {
    float v = line.substring(6).toFloat();
    if (v >= 0.0f && v <= 2.0f) {
      temperature = v;
      Serial.printf("✅ temperature impostata a %.2f\n", temperature);
    } else {
      Serial.println("⚠️ Valore non valido (0.0 - 2.0).");
    }
  } else if (line.startsWith("/topP ")) {
    float v = line.substring(6).toFloat();
    if (v > 0.0f && v <= 1.0f) {
      topP = v;
      Serial.printf("✅ topP impostata a %.2f\n", topP);
    } else {
      Serial.println("⚠️ Valore non valido (0.0 - 1.0].");
    }
  } else if (line.startsWith("/model ")) {
    modelRuntime = line.substring(7);
    modelRuntime.trim();
    if (modelRuntime.length() > 0) {
      Serial.printf("✅ Modello impostato a: %s\n", modelRuntime.c_str());
    } else {
      Serial.println("⚠️ Nome modello non valido.");
    }
  } else if (line.startsWith("/heap")) {
    Serial.printf("💾 Free heap: %u bytes\n", ESP.getFreeHeap());
  } else if (line.startsWith("/help")) {
    printHelp();
  } else {
    Serial.println("❓ Comando non riconosciuto. Usa /help");
  }
  return true;
}

bool postToGemini(const String& domanda, String& answerOut, String& errorOut) {
  HTTPClient http;
  String endpoint = "https://generativelanguage.googleapis.com/v1beta/models/";
  endpoint += modelRuntime;
  endpoint += ":generateContent?key=";
  endpoint += API_KEY;

  http.begin(tls, endpoint);
  http.addHeader("Content-Type", "application/json");
  http.setTimeout(HTTP_TIMEOUT_MS);

  // Costruiamo payload JSON in modo sicuro
  // Nota: aumentare dimensione se aggiungi altre opzioni
  DynamicJsonDocument payload(2048);

  JsonObject gen = payload.createNestedObject("generationConfig");
  gen["maxOutputTokens"] = maxOutputTokens;
  gen["temperature"]     = temperature;
  gen["topP"]            = topP;

  JsonArray contents = payload.createNestedArray("contents");
  JsonObject item    = contents.createNestedObject();
  JsonArray parts    = item.createNestedArray("parts");
  JsonObject p0      = parts.createNestedObject();
  p0["text"]         = domanda;

  String body;
  serializeJson(payload, body);

  int httpCode = http.POST(body);

  if (httpCode != 200) {
    errorOut = String("HTTP ") + httpCode + " - " + http.errorToString(httpCode);
    http.end();
    return false;
  }

  // Otteniamo la risposta
  String response = http.getString();
  http.end();

  // Parsing: buffer grande per risposte lunghe
  DynamicJsonDocument doc(24576);
  DeserializationError err = deserializeJson(doc, response);
  if (err) {
    errorOut = String("Errore parsing JSON: ") + err.c_str();
    return false;
  }

  // Possibili percorsi della risposta
  // 1) Risposta classica
  if (doc.containsKey("candidates")) {
    JsonVariant textVar = doc["candidates"][0]["content"]["parts"][0]["text"];
    if (!textVar.isNull()) {
      answerOut = cleanText(textVar.as<String>());
      if (answerOut.length() == 0) {
        answerOut = "⚠️ Risposta vuota dopo pulizia.";
      }
      return true;
    }
  }

  // 2) Feedback/sicurezza/errore
  if (doc.containsKey("promptFeedback")) {
    String fb = doc["promptFeedback"].as<String>();
    errorOut = "Prompt feedback: " + fb;
    return false;
  }

  // 3) Messaggio generico
  errorOut = "Risposta inattesa dal modello.";
  return false;
}

void setup() {
  Serial.begin(115200);
  delay(300);

  Serial.println("🔌 Connessione Wi-Fi…");
  WiFi.mode(WIFI_STA);
  WiFi.begin(SSID, PASSWORD);

  uint32_t start = millis();
  while (WiFi.status() != WL_CONNECTED) {
    delay(400);
    Serial.print(".");
    if (millis() - start > 20000) break; // 20s timeout Wi-Fi
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.printf("\n✅ Connesso a %s  |  IP: %s\n", SSID, WiFi.localIP().toString().c_str());
  } else {
    Serial.println("\n❌ Wi-Fi non connesso (controlla SSID/PASSWORD). Provo comunque.");
  }

  // ⚠️ In demo accettiamo tutti i certificati (NON usare in produzione!)
  tls.setInsecure();

  printHelp();
  Serial.println("✍️  Scrivi una domanda e premi INVIO:");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\r') continue;            // ignora carriage return
    if (c == '\n') {
      domandaBuffer.trim();
      if (domandaBuffer.length() > 0) {
        // Se è un comando, gestiscilo e non inviare a Gemini
        if (!handleCommand(domandaBuffer)) {
          // Invia domanda a Gemini
          String answer, err;
          bool ok = false;
          for (uint8_t attempt = 0; attempt <= MAX_RETRIES; ++attempt) {
            Serial.printf("🚀 Invio domanda (tentativo %u/%u): %s\n",
                          attempt + 1, MAX_RETRIES + 1, domandaBuffer.c_str());
            ok = postToGemini(domandaBuffer, answer, err);
            if (ok) break;
            Serial.printf("⚠️ Errore: %s\n", err.c_str());
            delay(500 + attempt * 1000); // backoff semplice
          }

          if (ok) {
            Serial.println("\n🧠 Risposta:");
            Serial.println(answer);
          } else {
            Serial.println("\n❌ Non sono riuscito a ottenere una risposta.");
          }
        }
      }
      domandaBuffer = "";
      Serial.println("\n✍️  Scrivi un'altra domanda (oppure /help):");
    } else {
      domandaBuffer += c;
    }
  }
}
