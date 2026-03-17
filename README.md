# Alpenzeit
ESP32 timer with Wi-Fi access point, push button control and live web interface via WebSocket.

# AlpenZeit

![Platform](https://img.shields.io/badge/platform-ESP32-blue)
![Arduino](https://img.shields.io/badge/framework-Arduino-green)
![Status](https://img.shields.io/badge/status-stable-brightgreen)
![License](https://img.shields.io/badge/license-non--commercial-lightgrey)

ESP32-basierter Timer mit lokalem WLAN (Access Point), Tastersteuerung und moderner Weboberfläche mit Echtzeit-Anzeige via WebSockets.

---

## 📸 Demo

### Webinterface

![Web UI](docs/webui.png)

### Live-Timer (GIF)

![Demo](docs/demo.gif)

👉 *(Screenshots kannst du später einfach in den `docs/`-Ordner legen)*

---

## ✨ Features

* ⏱️ Timer / Stoppuhr mit Millisekunden-Anzeige
* 📡 Eigenes WLAN (kein Router nötig)
* 🌐 Webinterface im Browser (Smartphone & PC)
* 🔄 Echtzeit-Updates via WebSocket
* 🔘 Steuerung über Hardware-Taster
* 💻 Steuerung über serielle Schnittstelle

---

## 🧰 Hardware

* ESP32
* Taster

### 🔌 Verdrahtung

```
3V3 ─── Taster ─── GPIO27
```

👉 GPIO27 ist als `INPUT_PULLDOWN` konfiguriert

---

## 🎮 Bedienung

### 🔘 Taster

* **Kurz drücken** → Start / Stop
* **3 Sekunden halten** → Reset

### 💻 Seriell (115200 Baud)

| Befehl | Funktion |
| ------ | -------- |
| `1`    | Start    |
| `0`    | Stop     |
| `r`    | Reset    |

---

## 🌐 Webinterface

Nach dem Start erstellt der ESP32 ein WLAN:

* **SSID:** `AlpenZeit`
* **Passwort:** `alpenzeit2026`

👉 Danach im Browser öffnen:

```
http://192.168.4.1
```

---

## ⚙️ Funktionsweise

* ESP32 läuft im **Access Point Mode**
* HTTP-Server liefert die HTML-Oberfläche
* WebSocket-Server sendet Live-Daten
* Frontend interpoliert Zeit für flüssige Anzeige
* Button wird **entprellt + Long-Press erkannt**

---

## 📁 Projektstruktur

```
Alpenzeit/
├── Alpenzeit.ino
├── index_html.h
├── docs/
│   ├── webui.png
│   └── demo.gif
└── README.md
```

---

## 🚀 Installation

1. Projekt in Arduino IDE öffnen
2. ESP32 Board auswählen
3. Hochladen
4. Mit WLAN `AlpenZeit` verbinden
5. Browser öffnen → `192.168.4.1`

---

## 📦 Abhängigkeiten

Arduino Libraries:

* `WiFi.h`
* `WebServer.h`
* `WebSocketsServer.h`
  👉 **WebSockets by Markus Sattler installieren**

---

## 📄 Lizenz

Dieses Projekt ist nur für private und nicht-kommerzielle Nutzung bestimmt.

---

## 👤 Autor

flox86
