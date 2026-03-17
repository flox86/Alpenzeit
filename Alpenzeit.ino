#include <WiFi.h>
#include <WebServer.h>
#include <WebSocketsServer.h>
#include "index_html.h"

// ACHTUNG bei "WebSocketsServer.h" nicht gefunden... 
// Über die Bibliothek nachinstallieren. --> WebSockets by Markus Sattler 
// Taster am PIN 27 siehe Zeile 21 -->  const int BUTTON_PIN = 27;
// Verdrahtung: PIN 3V3 ---> TASTER ---> PIN 27

const char* AP_SSID = "AlpenZeit";
const char* AP_PASS = "alpenzeit2026";

WebServer server(80);
WebSocketsServer webSocket = WebSocketsServer(81);

// ---------------- Timer ----------------
bool timerRunning = false;
uint32_t baseElapsedMs = 0;
uint32_t startedAtMs = 0;

// ---------------- Taster ----------------
const int BUTTON_PIN = 27;

bool buttonStableState = false;        // entprellter Zustand
bool lastButtonReading = false;        // letzter Rohwert
uint32_t lastDebounceTime = 0;
const uint32_t debounceMs = 30;

bool pressInProgress = false;
bool longPressHandled = false;
uint32_t pressStartTime = 0;
const uint32_t longPressMs = 3000;

// ---------------- HTML ----------------
// --- siehe index_html.h ---

uint32_t getElapsedMs() {
  if (timerRunning) {
    return baseElapsedMs + (millis() - startedAtMs);
  }
  return baseElapsedMs;
}

void broadcastState() {
  String msg = "{\"running\":";
  msg += (timerRunning ? "true" : "false");
  msg += ",\"elapsed\":";
  msg += String(getElapsedMs());
  msg += "}";
  webSocket.broadcastTXT(msg);
}

void handleRoot() {
  server.send_P(200, "text/html", INDEX_HTML);
}

void onWebSocketEvent(uint8_t num, WStype_t type, uint8_t* payload, size_t length) {
  switch (type) {
    case WStype_CONNECTED:
      webSocket.sendTXT(
        num,
        String("{\"running\":") +
        (timerRunning ? "true" : "false") +
        ",\"elapsed\":" + String(getElapsedMs()) + "}"
      );
      break;

    case WStype_DISCONNECTED:
      break;

    default:
      break;
  }
}

void startTimer() {
  if (!timerRunning) {
    startedAtMs = millis();
    timerRunning = true;
    Serial.println("Timer gestartet");
    broadcastState();
  }
}

void stopTimer() {
  if (timerRunning) {
    baseElapsedMs += (millis() - startedAtMs);
    timerRunning = false;
    Serial.println("Timer gestoppt");
    broadcastState();
  }
}

void resetTimer() {
  timerRunning = false;
  baseElapsedMs = 0;
  startedAtMs = 0;
  Serial.println("Timer zurückgesetzt");
  broadcastState();
}

void toggleTimer() {
  if (timerRunning) {
    stopTimer();
  } else {
    startTimer();
  }
}

void handleButton() {
  bool reading = digitalRead(BUTTON_PIN);  // HIGH wenn gedrückt

  if (reading != lastButtonReading) {
    lastDebounceTime = millis();
    lastButtonReading = reading;
  }

  if ((millis() - lastDebounceTime) > debounceMs) {
    if (reading != buttonStableState) {
      buttonStableState = reading;

      if (buttonStableState) {
        // Taster wurde gedrückt
        pressInProgress = true;
        longPressHandled = false;
        pressStartTime = millis();
      } else {
        // Taster wurde losgelassen
        if (pressInProgress && !longPressHandled) {
          toggleTimer();  // kurzer Tastendruck
        }
        pressInProgress = false;
      }
    }
  }

  if (pressInProgress && buttonStableState && !longPressHandled) {
    if ((millis() - pressStartTime) >= longPressMs) {
      resetTimer();
      longPressHandled = true;
    }
  }
}

void handleSerial() {
  while (Serial.available()) {
    char c = Serial.read();

    if (c == '1') {
      startTimer();
    } else if (c == '0') {
      stopTimer();
    } else if (c == 'r' || c == 'R') {
      resetTimer();
    }
  }
}

void setup() {
  Serial.begin(115200);
  delay(300);

  pinMode(BUTTON_PIN, INPUT_PULLDOWN);

  WiFi.mode(WIFI_AP);
  bool ok = WiFi.softAP(AP_SSID, AP_PASS);

  Serial.println();
  if (ok) {
    Serial.println("Access Point gestartet");
    Serial.print("SSID: ");
    Serial.println(AP_SSID);
    Serial.print("Passwort: ");
    Serial.println(AP_PASS);
    Serial.print("IP: ");
    Serial.println(WiFi.softAPIP());
  } else {
    Serial.println("Fehler beim Starten des Access Points");
  }

  server.on("/", handleRoot);
  server.begin();

  webSocket.begin();
  webSocket.onEvent(onWebSocketEvent);

  Serial.println("HTTP-Server auf Port 80 gestartet");
  Serial.println("WebSocket-Server auf Port 81 gestartet");
  Serial.println("Taster an GPIO27: kurz = Start/Stop, 3 s = Reset");
  Serial.println("Seriell: 1 = Start, 0 = Stop, r = Reset");
}

void loop() {
  server.handleClient();
  webSocket.loop();

  handleSerial();
  handleButton();
}