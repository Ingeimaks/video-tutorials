/*
  Progetto: ESP32 + Gemini API
  Autore: Ingeimaks
  Data: Agosto 2025
  Descrizione:
    Questo programma permette di inviare domande dal Serial Monitor
    a Gemini (Google Generative Language API) e ricevere risposte.
    L’ESP32 si connette al Wi-Fi e utilizza HTTPS per la comunicazione.

  Funzionamento:
    1. L’ESP32 si connette alla rete Wi-Fi.
    2. L’utente scrive una domanda nel Serial Monitor.
    3. La domanda viene inviata tramite POST request all’API Gemini.
    4. La risposta JSON viene parsata e stampata sul Serial Monitor.

  Note di sicurezza:
    - Le credenziali Wi-Fi e l’API key NON vanno mai pubblicate su GitHub.
    - Qui sono lasciate vuote, sostituirle in locale prima dell’uso.
    - Attualmente il certificato SSL viene ignorato (setInsecure), quindi
      NON usare questo codice in produzione senza certificati validi.

  Licenza: MIT
*/

// Librerie necessarie per Wi-Fi, HTTPS e JSON
#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <ArduinoJson.h>

// 🔐 Inserisci i tuoi dati Wi-Fi
const char* ssid = "";
const char* password = "";

// 🔑 Chiave API Gemini
const char* apiKey = "";  // 🔁 Sostituisci con la tua

// 🔗 Endpoint Gemini 2.5 Flash
const String endpoint = "https://generativelanguage.googleapis.com/v1beta/models/gemini-2.0-flash:generateContent?key=" + String(apiKey);

// Variabili globali
WiFiClientSecure client;
String domanda = "";

void setup() {
  Serial.begin(115200);
  delay(1000);

  WiFi.begin(ssid, password);
  Serial.print("🔌 Connessione a ");
  Serial.print(ssid);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }

  Serial.println("\n✅ Wi-Fi connesso!");
  Serial.println("📡 IP: " + WiFi.localIP().toString());

  // ⚠️ Accetta tutti i certificati SSL (non sicuro per produzione)
  client.setInsecure();

  Serial.println("\n✍️  Scrivi una domanda nel Serial Monitor e premi INVIO:");
}

void loop() {
  while (Serial.available()) {
    char c = Serial.read();
    if (c == '\n') {
      domanda.trim();
      if (domanda.length() > 0) {
        inviaDomandaAGemini(domanda);
      }
      domanda = "";
      Serial.println("\n✍️  Scrivi un'altra domanda:");
    } else {
      domanda += c;
    }
  }
}

void inviaDomandaAGemini(String domanda) {
  HTTPClient https;
  https.begin(client, endpoint);
  https.addHeader("Content-Type", "application/json");
  https.setTimeout(10000);  // ⏱️ aumentato

  // Costruisci il payload JSON
  String body = "{\"contents\": [{\"parts\": [{\"text\": \"" + domanda + "\"}]}]}";

  int httpCode = https.POST(body);

  if (httpCode == 200) {
    String response = https.getString();

    // Parsing della risposta con ArduinoJson
    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, response);

    if (error) {
      Serial.println("❌ Errore parsing JSON:");
      Serial.println(error.c_str());
    } else {
      String risposta = doc["candidates"][0]["content"]["parts"][0]["text"] | "⚠️ Nessuna risposta trovata.";
      Serial.println("\n🧠 Risposta:");
      Serial.println(risposta);
    }
  } else {
    Serial.printf("❌ Errore HTTP: %s\n", https.errorToString(httpCode).c_str());
  }

  https.end();
}
