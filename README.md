# 📡 ESP32 Gast-WLAN Toggler mit E-Paper-Display und QR-Code

Dieses Projekt ermöglicht es dir, das **Gast-WLAN deiner FRITZ!Box über einen ESP32** zu steuern. Mit einem E-Paper-Display wird der Status des Gast-WLANs angezeigt, und ein QR-Code wird generiert, um schnelle Verbindungen zu ermöglichen. Das Projekt nutzt die TR-064 Schnittstelle und beinhaltet auch eine Entprellung für den Schalter.

![Screenshot 2025-04-20 160552](https://github.com/user-attachments/assets/5e919455-f3b9-4efb-87c2-bab78b203d82)
![20250421_123442](https://github.com/user-attachments/assets/1b8b5ad5-4168-4586-a256-7677fb535a4d)
![20250421_125029](https://github.com/user-attachments/assets/93f49078-7761-43c6-b8d1-a146edc5a804)

---

## 🔧 Funktionen

- 🔁 Umschalten des Gast-WLANs per Knopfdruck
- 💡 Anzeige des Gast-WLAN-Status auf einem E-Paper-Display
- 🔒 TR-064 Authentifizierung mit der FRITZ!Box
- 📱 QR-Code zur schnellen WLAN-Verbindung
- 🔄 Automatische Statusabfrage alle 10 Sekunden

---

## 🧰 Benötigte Hardware

- ✅ ESP32-C3 Super Mini Modul
- ✅ E-Paper-Display (GDEY0154D67, 200x200 Pixel)
- ✅ Taster (verbunden mit GPIO9)
- ✅ Zugangsdaten zu deiner FRITZ!Box (Benutzer, Passwort, IP)

---

## 📦 Gehäuse (3D-Druck)

Für das Projekt steht ein zweiteiliges Gehäuse zur Verfügung, das speziell für das ESP32-Gast-WLAN-Modul mit E-Paper-Display entworfen wurde. Es schützt die Elektronik, sorgt für eine ordentliche Montage und bietet Öffnungen für das Display sowie den Taster.

### 🧩 Details

- Besteht aus **zwei passgenauen Teilen** (Ober- und Unterseite)
- Wird mit **vier Schrauben** fixiert (M2 oder M2.5 empfohlen)
- Aussparungen für das **E-Paper-Display** und den **Taster**
- Optional mit Halterung zur **Wandmontage**
- STL-Dateien befinden sich hier zum Download: https://cults3d.com/de/modell-3d/haus/fritzbox-gast-wlan-switch-fritz-box

## 🔌 Pinbelegung des E-Paper Displays

### EPD Pins | ESP32 C3 GPIO

| EPD Pin   | ESP32 C3 GPIO |
|-----------|---------------|
| **Busy**  | GPIO 10       |
| **RST**   | GPIO 9        |
| **DC**    | GPIO 8        |
| **CS**    | GPIO 7        |
| **CLK (SCK)** | GPIO 4    |
| **DIN/SDI (MOSI)** | GPIO 20 |

### Stromversorgung
| EPD Pin | ESP32 C3 GPIO |
|---------|---------------|
| **GND** | GND           |
| **3.3V** | 3.3V          |

### Button
| Button Pin | ESP32 C3 GPIO |
|---------|---------------|
| Button Arm 1 | GND           |
| Button Arm 2 | GPIO 3        |

---

## 💻 Abhängigkeiten

Folgende Libraries werden benötigt:

```cpp
#include <WiFi.h> 
#include <tr064.h> 
#include <SPI.h> 
#include <GxEPD2_BW.h> 
#include <Fonts/FreeSansBold12pt7b.h> 
#include <Bounce2.h>\#include <qrcode.h>
```

---

## ⚙️ Konfiguration

Im Code kannst du folgende Parameter anpassen:

```cpp
// WLAN-Zugangsdaten
const char* wifi_ssid = "NETZWERK";
const char* wifi_password = "Passwort";

// FRITZ!Box-Zugang
const char* fuser = "USER";            // FRITZ!Box-Benutzername
const char* fpass = "PasswortUser";    // FRITZ!Box-Passwort

// IP-Adresse und Port der FRITZ!Box
const char* IP = "192.168.0.1";       // Standard-IP
const int PORT = 49000;               // Standardport für TR-064
```

---

## ▶️ Inbetriebnahme

1. 🔧 **Zugangsdaten und IP-Adresse im Sketch eintragen**:
   - WLAN: `wifi_ssid`, `wifi_password`
   - FRITZ!Box: `fuser`, `fpass`, `IP`, `PORT`

2. ⬆️ **Sketch auf den ESP32-C3 flashen**:
   - Verwende dazu z. B. **PlatformIO** oder **Arduino IDE**.

3. 🔌 **ESP32-C3 starten**:
   - Der ESP32 verbindet sich automatisch mit deinem WLAN.

4. 🟢 **Drücke den Boot-Button (GPIO9)**:
   - Dies schaltet das **Gast-WLAN der FRITZ!Box** ein oder aus.

---

## 📝 Code-Beschreibung

Der Code für das **ESP32-C3 Projekt** hat folgende Funktionen und Hauptbestandteile:

1. **WLAN-Verbindung**:
   - Der ESP32-C3 verbindet sich mit dem WLAN-Netzwerk, dessen SSID und Passwort im Sketch hinterlegt sind.
   - Nach erfolgreicher Verbindung wird eine feste IP-Adresse in Verbindung mit der FRITZ!Box eingestellt.

2. **FRITZ!Box API**:
   - Der ESP32-C3 greift auf die FRITZ!Box API zu, um verschiedene Operationen auszuführen.
   - Nutzeranmeldedaten (Benutzername und Passwort) sowie die IP-Adresse und Port der FRITZ!Box sind ebenfalls im Sketch hinterlegt.

3. **Gast-WLAN Steuerung**:
   - Der ESP32-C3 ist so konzipiert, dass er das Gast-WLAN der FRITZ!Box ein- oder ausschaltet, wenn der **Boot-Button (GPIO9)** gedrückt wird.
   - Der Status des Gast-WLANs wird über die FRITZ!Box API kontrolliert.

5. **Fehlerbehandlung**:
   - Wenn die WLAN-Verbindung oder die Kommunikation mit der FRITZ!Box fehlschlägt, gibt es entsprechende Fehlermeldungen auf der seriellen Konsole.

6. **Logging**:
   - Der ESP32 gibt detaillierte Logs auf der seriellen Konsole aus, um den Status der Verbindung, den Zustand des Gast-WLANs und andere relevante Ereignisse anzuzeigen.

---

## 💡 Inspiration & Quellen

Dieses Projekt wurde inspiriert durch:

- [AVM TR-064 First Steps PDF](https://fritz.com/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf) – Einführung in die TR-064-Schnittstelle von AVM.
- [ESP32 E-Paper Thermometer](https://www.instructables.com/ESP32-E-Paper-Thermometer/) – Schritt-für-Schritt-Anleitung zur Erstellung eines E-Paper Thermometers mit einem ESP32.
- [ESP32-C3 Super Mini mit I2C und SPI](https://www.reddit.com/r/esp32/comments/1boaxjn/esp32c3_super_mini_with_i2c_and_spi_connections/?tl=de) – Diskussion und Informationen zu den Verbindungen von I2C und SPI auf dem ESP32-C3.
- [ESP32 und E-Paper mit gxEPD2](https://forum.arduino.cc/t/esp32-cannot-connect-to-wifi-when-e-paper-inited-gxepd2/565925/2) – Lösung für Verbindungsprobleme mit Wi-Fi und E-Paper-Modulen auf dem ESP32.

### Verwendete Bibliotheken und Tools:

- [gxEPD2](https://github.com/ZinggJM/gxEPD2) – für die Steuerung von E-Paper Displays auf dem ESP32.
- [ESP32 Wi-Fi Bibliothek](https://github.com/espressif/arduino-esp32) – für die Wi-Fi-Funktionalitäten auf dem ESP32.
- [Arduino IDE](https://www.arduino.cc/en/software) – Entwicklungsumgebung zur Programmierung des ESP32.

Ein besonderer Dank geht an die Entwickler und die Community, die diese großartigen Tools und Bibliotheken erstellt haben!


## 📄 Lizenz

Dieses Projekt steht unter der **MIT-Lizenz** – das bedeutet:

- ✅ Du darfst den Code frei verwenden, verändern und weiterverbreiten.
- ❌ Es besteht **keine Garantie** – die Nutzung erfolgt auf eigene Verantwortung.
- 📜 Bitte behalte den ursprünglichen Copyright-Hinweis.

👉 Details siehe [LICENSE](LICENSE)
