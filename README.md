# 📶 ESP32 TR-064 Gast-WLAN Toggler für FRITZ!Box

Dieses Projekt ermöglicht es dir, das **Gast-WLAN deiner FRITZ!Box über einen ESP32 (C3)** per Knopfdruck ein- und auszuschalten. Dabei wird die [TR-064 Schnittstelle](https://fritz.com/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf) verwendet. Eine LED zeigt den aktuellen Status des Gast-WLANs durch Blinken (aus) oder Dauerlicht (ein) an. Nach 5 Minuten Inaktivität versetzt sich der ESP32 in den Deep-Sleep-Modus, um Strom zu sparen.

---

## 🔧 Funktionen

- 🔁 Umschalten des Gast-WLANs per Knopfdruck (Boot-Button)
- 💡 LED-Statusanzeige für das Gast-WLAN
- 🔌 Automatischer Verbindungsaufbau mit WLAN
- 🔒 Authentifizierung mit FRITZ!Box über Benutzer & Passwort
- 💤 Deep-Sleep nach 5 Minuten Inaktivität
- 📡 Periodische Statusabfrage alle 10 Sekunden

---

## 🧰 Benötigte Hardware

- ✅ ESP32 oder ESP32-C3 Modul (z. B. [ESP32-C3 Super Mini](https://www.reddit.com/r/esp32/comments/1boaxjn/esp32c3_super_mini_with_i2c_and_spi_connections/?tl=de))
- ✅ Taster (verbunden mit GPIO9)
- ✅ LED (verbunden mit GPIO8)
- ✅ Zugangsdaten zu deiner FRITZ!Box (Benutzer, Passwort, IP)

---

## 🔌 Pinbelegung

| Komponente   | Pin       |
|--------------|-----------|
| LED          | GPIO8     |
| Button       | GPIO9     |

---

## 💻 Abhängigkeiten

Folgende Libraries werden benötigt:

```
#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
#include <tr064.h>  // TR-064 Library von [aaboy](https://github.com/aaboy/TR-064)

## ⚙️ Konfiguration

Im Code kannst du folgende Parameter anpassen:

// WLAN-Zugangsdaten
const char* wifi_ssid = "NETZWERK";
const char* wifi_password = "Passwort";

// FRITZ!Box-Zugang
const char* fuser = "USER";            // FRITZ!Box-Benutzername
const char* fpass = "PasswortUser";    // FRITZ!Box-Passwort

// IP-Adresse und Port der FRITZ!Box
const char* IP = "192.168.178.1";      // Standard-IP, ggf. anpassen
const int PORT = 49000;                // Standardport für TR-064

```

Wenn du willst, kann ich dir auch den oberen Teil der README mit Titel, Projektbeschreibung und Icons gestalten – sag einfach Bescheid!

---

## ▶️ Inbetriebnahme

1. 🔧 Zugangsdaten und IP-Adresse im Sketch eintragen:
   - WLAN: `wifi_ssid`, `wifi_password`
   - FRITZ!Box: `fuser`, `fpass`, `IP`, `PORT`

2. ⬆️ Sketch auf den ESP32-C3 flashen (z. B. via PlatformIO oder Arduino IDE)

3. 🔌 ESP32-C3 starten – er verbindet sich automatisch mit deinem WLAN

4. 🟢 Drücke den **Boot-Button** (GPIO9), um das **FRITZ!Box Gast-WLAN** ein- oder auszuschalten

5. 💤 Der ESP32-C3 geht nach **5 Minuten Inaktivität** automatisch in den Deep-Sleep-Modus


---

## 🧠 Inspiration & Quellen

- 📘 [AVM TR-064 Schnittstellen-Dokumentation (PDF)](https://fritz.com/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf)  

---

## 📄 Lizenz

Dieses Projekt steht unter der **MIT-Lizenz** – das bedeutet:

- ✅ Du darfst den Code frei verwenden, verändern und weiterverbreiten.
- ❌ Es besteht **keine Garantie** – die Nutzung erfolgt auf eigene Verantwortung.
- 📜 Bitte behalte den ursprünglichen Copyright-Hinweis.

👉 Details siehe [LICENSE](LICENSE)


---

## 🛈 Hinweise

- 🔌 Dieses Projekt wurde für den **ESP32-C3 Super Mini** entwickelt, funktioniert aber grundsätzlich mit jedem ESP32 (Pinbelegung ggf. anpassen).
- 📶 Voraussetzung ist eine **FRITZ!Box mit aktivierter TR-064-Schnittstelle**.
- 🛠 Bei Problemen mit der TR-064 Verbindung: Stelle sicher, dass in der FRITZ!Box unter **Heimnetz > Netzwerk > Netzwerkeinstellungen > Zugang für Anwendungen** der Haken bei *Zugriff für Anwendungen zulassen* gesetzt ist.
- 📡 Das Projekt nutzt aktiv **Wake on Button (GPIO9)** zum Aufwachen aus dem Deep Sleep – Button korrekt verkabeln!
- 🧪 Der Code wurde mit einer **FRITZ!Box 7590 AX** und **Firmware 7.5x** getestet.
- 🔐 Benutzerdaten und Netzwerkpasswörter sind im Sketch einzutragen – verwende dafür am besten sichere Passphrasen und/oder speichere den Code nicht öffentlich mit deinen Zugangsdaten.
