//https://fritz.com/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf
//https://www.instructables.com/ESP32-E-Paper-Thermometer/
//https://www.reddit.com/r/esp32/comments/1boaxjn/esp32c3_super_mini_with_i2c_and_spi_connections/?tl=de

#include <WiFi.h>
#include <WiFiMulti.h>
#include <HTTPClient.h>
WiFiMulti WiFiMulti;
#include <tr064.h>

//-------------------------------------------------------------------------------------
// Put your router settings here
//-------------------------------------------------------------------------------------
// Wifi network name (SSID)
const char* wifi_ssid = "NETZWERK"; 
// Wifi network password
const char* wifi_password = "Passwort";
// The username if you created an account, "admin" otherwise
const char* fuser = "USER";
// The password for the aforementioned account.
const char* fpass = "PasswortUser";
// IP address of your router. This should be "192.168.179.1" for most FRITZ!Boxes
const char* IP = "192.168.0.1";
// Port of the API of your router. This should be 49000 for all TR-064 devices.
const int PORT = 49000;
// Boot Button als Switch:
#define BOOT_BUTTON_PIN 9
#define DEBOUNCE_DELAY 50  // Entprellzeit in Millisekunden
#define LED_PIN 8  // Definiere den Pin für die LED (GPIO0)
#define TIMEOUT_MS 300000  // 5 Minute in Millisekunden für deep sleep zeit
#define STATUS_CHECK_INTERVAL 10000  // alle 30 Sekunden
// -------------------------------------------------------------------------------------
 
// TR-064 connection
TR064 connection(PORT, IP, fuser, fpass);
 
unsigned long lastDebounceTime = 0;
int lastButtonState = HIGH;   // Letzter stabiler Zustand
int currentButtonState;
int stableState = HIGH;       // Entprellter Zustand

unsigned long lastActivityTime = 0;
unsigned long lastStatusCheckTime = 0;  // Zeitpunkt der letzten Statusabfrage

bool guestWifiEnabled = false;  // wird bei toggleGuestWifi gesetzt
bool ledState = false;
unsigned long previousMillis = 0;
const long blinkInterval = 500;  // z.B. 500ms

int getWifiStatus() {
    String params[][2] = {{}};
    String req[][2] = {
        {"NewEnable", ""},  // WLAN aktiviert?
        {"NewStatus", ""},  // "Up"/"Down"
        {"NewSSID", ""}     // SSID (zur Kontrolle)
    };

    connection.action("urn:dslforum-org:service:WLANConfiguration:1", "GetInfo", params, 0, req, 3);
    
    String enabled = req[0][1];
    String status = req[1][1];
    String ssid = req[2][1];

    Serial.println("WLAN SSID: " + ssid);
    Serial.println("WLAN aktiviert: " + enabled);
    Serial.println("WLAN Status: " + status);

    return (enabled == "1" && status == "Up") ? 1 : 0;
}

int getGuestWifiStatus() {
    // Erstes TR-064-Request für Status, SSID etc.
    String params1[][2] = {{}};
    String req1[][2] = {
        {"NewEnable", ""},  // WLAN aktiviert?
        {"NewStatus", ""},  // "Up"/"Down"
        {"NewSSID", ""}     // SSID (zur Kontrolle)
    };

    connection.action("urn:dslforum-org:service:WLANConfiguration:3", "GetInfo", params1, 0, req1, 3);
    
    String enabled = req1[0][1];
    String status  = req1[1][1];
    String ssid    = req1[2][1];

    // Zweites TR-064-Request für das WLAN-Passwort
    String params2[][2] = {{}};
    String req2[][2] = {{"NewKeyPassphrase", ""}};

    connection.action("urn:dslforum-org:service:WLANConfiguration:3", "GetSecurityKeys", params2, 0, req2, 1);

    String passkey = req2[0][1];

    // Ausgabe
    Serial.println("Gast-WLAN SSID      : " + ssid);
    Serial.println("Gast-WLAN aktiviert : " + enabled);
    Serial.println("Gast-WLAN Status    : " + status);
    Serial.println("Gast-WLAN Passwort  : " + passkey);

    return (enabled == "1" && status == "Up") ? 1 : 0;
}

void toggleGuestWifi(bool toggle) {
    int status = getGuestWifiStatus();  // 1 = an, 0 = aus
    int newStatus = status ? 1 : 0;     // newStatus initialisieren analog status

    if (toggle) {
        newStatus = status ? 0 : 1;     // Status invertieren weil es soll umgeschalten werden
        String params[][2] = {
            {"NewEnable", String(newStatus)}
        };

        bool success = connection.action(
            "urn:dslforum-org:service:WLANConfiguration:3",
            "SetEnable",
            params, 1,
            nullptr, 0
        );

        if (Serial) {
            //Serial.printf("%d\n", newStatus);
            Serial.printf("Gast-WLAN wurde %s.\n", newStatus ? "eingeschaltet" : "ausgeschaltet");
        }

        if (!success && Serial) {
            Serial.println("Fehler beim Senden der WLAN Umschalt-Anfrage.");
        }

    }

    guestWifiEnabled = newStatus == 1;
}

void ensureWIFIConnection() {
    if((WiFiMulti.run() != WL_CONNECTED)) {
        WiFiMulti.addAP(wifi_ssid, wifi_password);
        while ((WiFiMulti.run() != WL_CONNECTED)) {
            delay(100);
        }
        Serial.println("Verbindung zum WLAN wird hergestellt...");
    }
}

void setup() {
    Serial.begin(115200);
    Serial.println("boot...");
    pinMode(BOOT_BUTTON_PIN, INPUT_PULLUP); // Der Button zieht normalerweise gegen GND
    pinMode(LED_PIN, OUTPUT);  // Setze den LED-Pin als Ausgang
    digitalWrite(LED_PIN, HIGH);    // LED aus

    if (esp_sleep_get_wakeup_cause() == ESP_SLEEP_WAKEUP_EXT0) {
        Serial.println("Aufgewacht durch Boot-Button (GPIO9)");
    } else {
        Serial.println("Starte erstmalig oder durch Reset");
    }

    lastActivityTime = millis();  // Startzeit setzen

    // Konfiguriere Wakeup durch eine fallende Flanke auf GPIO9
      // GPIO9 als Wakeup konfigurieren bei LOW
    esp_sleep_enable_gpio_wakeup();
    gpio_wakeup_enable((gpio_num_t)BOOT_BUTTON_PIN, GPIO_INTR_LOW_LEVEL);

    // Connect to wifi
    ensureWIFIConnection();
    Serial.println("WIFI connected...");

    // Bei Problemen kann hier die Debug Ausgabe aktiviert werden
    //connection.debug_level = connection.DEBUG_VERBOSE;
    connection.init();
    toggleGuestWifi(false);
}
 
void loop() {

  unsigned long currentMillis = millis(); 

  if (!guestWifiEnabled) {
        // Blinken nur wenn Gast-WLAN aus ist
        if (currentMillis - previousMillis >= blinkInterval) {
            previousMillis = currentMillis;
            ledState = !ledState;
            digitalWrite(LED_PIN, ledState ? LOW : HIGH);  // LOW = an, HIGH = aus
        }
    } else {
        digitalWrite(LED_PIN, LOW); // LED dauerhaft an, wenn WLAN an ist
    }

  currentButtonState = digitalRead(BOOT_BUTTON_PIN);

  if (currentButtonState != lastButtonState) {
    // Zustand hat sich geändert – Timer zurücksetzen
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > DEBOUNCE_DELAY) {
    // Wenn sich der Zustand stabilisiert hat
    if (currentButtonState != stableState) {
      stableState = currentButtonState;

      if (stableState == LOW) {
        Serial.println("BOOT Button gedrückt Toggle Gast Wlan!");
        toggleGuestWifi(true);
        lastActivityTime = millis();  // Aktivität registrieren
      } 
    }
  }

  lastButtonState = currentButtonState;

    // Status alle 30 Sekunden abfragen
    if (millis() - lastStatusCheckTime > STATUS_CHECK_INTERVAL) {
        Serial.println("\nStatusabfrage alle " + String(STATUS_CHECK_INTERVAL / 1000) + " Sekunden:");
        toggleGuestWifi(false);
        lastStatusCheckTime = millis();
      }

  // Prüfen, ob 60 Sekunden inaktiv waren
  if (millis() - lastActivityTime > TIMEOUT_MS) {
    Serial.println(String(TIMEOUT_MS / 1000) + " Sekunden ohne Aktivität. Gehe in Deep Sleep...");
    delay(500);  // Noch Zeit für Serial-Ausgabe

    esp_deep_sleep_start();
  }

}

