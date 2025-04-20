//https://fritz.com/fileadmin/user_upload/Global/Service/Schnittstellen/AVM_TR-064_first_steps.pdf
//https://www.instructables.com/ESP32-E-Paper-Thermometer/
//https://www.reddit.com/r/esp32/comments/1boaxjn/esp32c3_super_mini_with_i2c_and_spi_connections/?tl=de
//https://forum.arduino.cc/t/esp32-cannot-connect-to-wifi-when-e-paper-inited-gxepd2/565925/2

#include <WiFi.h>
#include <tr064.h>
#include <SPI.h>
#include <GxEPD2_BW.h>
#include <Fonts/FreeSansBold12pt7b.h>
#include <Bounce2.h>
#include <qrcode.h>

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
#define ONOFF_BUTTON_PIN 3 //ON OFF Button
#define DEBOUNCE_DELAY 50  // Entprellzeit in Millisekunden
#define UPDATE_INTERVAL 10000  // Update-Intervall in Millisekunden

//DISPLAY
/*
EPD Pins	  | ESP32 C3 GPIO
___________________________
Busy	      | 10
RST	        | 9
DC	        | 8
CS	        | 7
CLK	        | SCK = 4
DIN/SDI	    | MOSI = 20
___________________________
GND	        | GND
3.3V	      | 3.3V
*/

// PIN-Zuweisungen
#define EPD_BUSY 10
#define EPD_RST  9
#define EPD_DC   8
#define EPD_CS   7
#define SPI_SCK  4  // SCLK
#define SPI_MISO 20  // MISO --> nicht -1 da Dummy Pin notwendig, sonst error:  #define SPI_MISO -1  // MISO
#define SPI_MOSI 21  // MOSI --> nicht 6 um keinen ADC Pin zu nehmen wegen Wlan problemen 

// -------------------------------------------------------------------------------------
 
// TR-064 connection
TR064 connection(PORT, IP, fuser, fpass);

GxEPD2_BW<GxEPD2_154_GDEY0154D67, GxEPD2_154_GDEY0154D67::HEIGHT> display(GxEPD2_154_GDEY0154D67(/*CS=5*/ EPD_CS, /*DC=*/ EPD_DC, /*RST=*/ EPD_RST, /*BUSY=*/ EPD_BUSY)); // GDEY0154D67 200x200, SSD1681, (FPC-B001 20.05.21)
#define GxEPD2_DRIVER_CLASS GxEPD2_154_GDEY0154D67 // GDEY0154D67 200x200, SSD1681, (FPC-B001 20.05.21)

unsigned long lastUpdateTime = 0;   // Variable zur Speicherung der letzten Aktualisierungszeit
bool previousStatus = false; // false bedeutet, dass das Gast-WLAN inaktiv war

struct GuestWifiInfo {
    String enabled;
    String status;
    String ssid;
    String passkey;
};

GuestWifiInfo wifiInfo;
Bounce debouncer = Bounce(); // Bounce-Objekt für Entprellung
QRCode qrcode;

// QR-Code auf dem Display anzeigen (mittig, mit einstellbarem Rand)
void displayQRCode(String content, int border) {
  const uint8_t qrVersion = 3; // 3 = 29x29 Module
  const uint8_t qrSize = qrcode_getBufferSize(qrVersion);
  uint8_t qrcodeData[qrSize];

  qrcode_initText(&qrcode, qrcodeData, qrVersion, ECC_LOW, content.c_str());  

  display.setRotation(1);                 // Bildschirmrotation festlegen
  display.setTextColor(GxEPD_BLACK);      // Textfarbe festlegen
  display.setFullWindow();                // Volles Fenster verwenden

  int16_t screenWidth  = display.width();
  int16_t screenHeight = display.height();

  // Verfügbare Breite und Höhe abzüglich des gewünschten Rands
  int16_t availableWidth  = screenWidth  - 2 * border;
  int16_t availableHeight = screenHeight - 2 * border;

  // Bestmöglichen Skalierungsfaktor berechnen (ganzzahlig)
  int scale = min(availableWidth / qrcode.size, availableHeight / qrcode.size);

  // QR-Code-Größe in Pixeln
  int qrPixelSize = qrcode.size * scale;

  // Position mittig innerhalb des verfügbaren Bereichs
  int16_t x = (screenWidth  - qrPixelSize) / 2;
  int16_t y = (screenHeight - qrPixelSize) / 2;

  display.firstPage();
  do {
    display.fillScreen(GxEPD_WHITE);

    for (uint8_t iy = 0; iy < qrcode.size; iy++) {
      for (uint8_t ix = 0; ix < qrcode.size; ix++) {
        if (qrcode_getModule(&qrcode, ix, iy)) {
          display.fillRect(x + ix * scale, y + iy * scale, scale, scale, GxEPD_BLACK);
        }
      }
    }

  } while (display.nextPage());
}

/*void showMessage(const char* msg, const GFXfont* font) {
    display.setRotation(1);                 // Bildschirmrotation festlegen
    display.setFont(font);                   // Schriftart festlegen
    display.setTextColor(GxEPD_BLACK);       // Textfarbe festlegen
    display.setFullWindow();                 // Volles Fenster verwenden
    
    display.fillScreen(GxEPD_WHITE);         // Bildschirm löschen
    display.firstPage();

    do {
        int16_t screenWidth = display.width();
        int16_t screenHeight = display.height();

        // Berechnung der Textgröße
        int16_t x1 = 0, y1 = 0;
        uint16_t w = 0, h = 0;
        
        // Berechnung der Textbreite und -höhe des gesamten Textes
        display.getTextBounds(msg, 0, 0, &x1, &y1, &w, &h);

        // Den Text horizontal und vertikal zentrieren
        int16_t x = (screenWidth - w) / 2 - x1;
        int16_t y = (screenHeight - h) / 2 - y1;

        // Setzen des Cursors und Text drucken
        display.setCursor(x, y);
        display.println(msg);

    } while (display.nextPage());  // Nächste Seite aktualisieren, falls nötig
}*/

void showMessage_two_lines(const char* line1, const char* line2, const GFXfont* font) {
  display.setRotation(1);                 // Bildschirmrotation festlegen
  display.setFont(font);                  // Schriftart festlegen
  display.setTextColor(GxEPD_BLACK);      // Textfarbe festlegen
  display.setFullWindow();                // Volles Fenster verwenden
  
  display.fillScreen(GxEPD_WHITE);        // Bildschirm löschen
  display.firstPage();

  do {
      int16_t screenWidth = display.width();
      int16_t screenHeight = display.height();

      // Berechnung der Größe von Zeile 1
      int16_t x1_1, y1_1;
      uint16_t w1, h1;
      display.getTextBounds(line1, 0, 0, &x1_1, &y1_1, &w1, &h1);
      
      // Berechnung der Größe von Zeile 2
      int16_t x1_2, y1_2;
      uint16_t w2, h2;
      display.getTextBounds(line2, 0, 0, &x1_2, &y1_2, &w2, &h2);
      
      // Gesamthöhe für beide Zeilen inkl. Zwischenraum
      int16_t spacing = 4; // Abstand zwischen den Zeilen
      int16_t totalHeight = h1 + spacing + h2;
      
      // Y-Startpunkt zum vertikalen Zentrieren beider Zeilen
      int16_t startY = (screenHeight - totalHeight) / 2;

      // X-Positionen zum horizontalen Zentrieren je Zeile
      int16_t x1 = (screenWidth - w1) / 2 - x1_1;
      int16_t y1 = startY - y1_1;

      int16_t x2 = (screenWidth - w2) / 2 - x1_2;
      int16_t y2 = y1 + h1 + spacing;

      // Ausgabe beider Zeilen
      display.setCursor(x1, y1);
      display.println(line1);

      display.setCursor(x2, y2);
      display.println(line2);

  } while (display.nextPage());  // Nächste Seite aktualisieren, falls nötig
}

void updateGuestWifiInfo(bool forceUpdate = false) {
    String params1[][2] = {{}};
    String req1[][2] = {
        {"NewEnable", ""},
        {"NewStatus", ""},
        {"NewSSID", ""}
    };

    connection.action("urn:dslforum-org:service:WLANConfiguration:3", "GetInfo", params1, 0, req1, 3);

    wifiInfo.enabled = req1[0][1];
    wifiInfo.status  = req1[1][1];
    wifiInfo.ssid    = req1[2][1];

    String params2[][2] = {{}};
    String req2[][2] = {{"NewKeyPassphrase", ""}};

    connection.action("urn:dslforum-org:service:WLANConfiguration:3", "GetSecurityKeys", params2, 0, req2, 1);

    wifiInfo.passkey = req2[0][1];

    // Debug-Ausgabe
    Serial.println("Gast-WLAN SSID      : " + wifiInfo.ssid);
    Serial.println("Gast-WLAN aktiviert : " + wifiInfo.enabled);
    Serial.println("Gast-WLAN Status    : " + wifiInfo.status);
    Serial.println("Gast-WLAN Passwort  : " + wifiInfo.passkey);

      // Überprüfen, ob sich der Status geändert hat
  bool currentStatus = (wifiInfo.enabled == "1" && wifiInfo.status == "Up");

  // Prüfen, ob sich der Zustand geändert hat
  if (forceUpdate || currentStatus != previousStatus) {
    if (currentStatus) {
      // GASTWLAN ist jetzt aktiv
      String qrContent = "WIFI:S:" + wifiInfo.ssid + ";T:WPA;P:" + wifiInfo.passkey + ";;";
      displayQRCode(qrContent, 5); // Rand mit angeben
    } else {
      // GASTWLAN ist jetzt inaktiv
      showMessage_two_lines("Gast-WLAN", "ist nicht aktiv", &FreeSansBold12pt7b);
    }

    // Aktualisieren des vorherigen Status
    previousStatus = currentStatus;
  }

}

void initWiFi() {

    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);
    WiFi.begin(wifi_ssid, wifi_password);

    Serial.print("Connecting to WiFi ..");
    //showMessage("Verbinde\nWLAN...", &FreeSansBold12pt7b);
    showMessage_two_lines("Connect zu", "FRITZBOX", &FreeSansBold12pt7b);
    while (WiFi.status() != WL_CONNECTED) {
      Serial.print('.');
      delay(1000);
    }
    Serial.println('.');
    showMessage_two_lines("Schalter IP:", WiFi.localIP().toString().c_str(), &FreeSansBold12pt7b);
    Serial.println(WiFi.localIP());
  }

  void toggleGuestWifi() {
    // Aktuellen Status checken (1 = aktiv, 0 = inaktiv)
    bool currentStatus = (wifiInfo.enabled == "1" && wifiInfo.status == "Up");
    bool newStatus = !currentStatus;  // Umschalten
  
    String params[][2] = {
      {"NewEnable", newStatus ? "1" : "0"}
    };
  
    bool success = connection.action(
      "urn:dslforum-org:service:WLANConfiguration:3",
      "SetEnable",
      params, 1,
      nullptr, 0
    );
  
    if (Serial) {
      Serial.printf("Gast-WLAN wurde %s.\n", newStatus ? "eingeschaltet" : "ausgeschaltet");
      if (!success) {
        Serial.println("Fehler beim Senden der WLAN-Umschalt-Anfrage.");
      }
    }
  }
//----------------------------------------------------------------------

void setup() {
    delay(5000);
    Serial.begin(115200);
    delay(3000);
    Serial.println("boot...");
    //Button
    pinMode(ONOFF_BUTTON_PIN, INPUT_PULLUP); // Der Button zieht normalerweise gegen GND
    debouncer.attach(ONOFF_BUTTON_PIN);  // Den Pin an das Bounce-Objekt anhängen
    debouncer.interval(50);       // Entprellzeit auf 50ms setzen
    //Dispaly
    SPI.begin(SPI_SCK, SPI_MISO, SPI_MOSI);
    display.init();
    //display.init(115200, true, 2, false); // USE THIS for Waveshare boards with "clever" reset circuit, 2ms reset pulse
    delay(100);
    // Connect to wifi
    initWiFi();
    delay(10);
    //Fritzbox connection
    // Bei Problemen kann hier die Debug Ausgabe aktiviert werden
    //connection.debug_level = connection.DEBUG_VERBOSE;
    connection.init();
    //initialwert abrufen
    updateGuestWifiInfo(true);
    delay(20);
}
 
void loop() {
  debouncer.update();  // Entprellung regelmäßig aktualisieren

   // Überprüfen, ob eine fallende Flanke erkannt wurde
    if (debouncer.fell()) {
        Serial.println("Button wurde losgelassen");
        toggleGuestWifi();
        delay(50);
        // Hier kannst du deinen Code einfügen, der bei einer fallenden Flanke ausgeführt wird
        updateGuestWifiInfo();      // Ruft die Funktion auf
      }

  if (millis() - lastUpdateTime >= UPDATE_INTERVAL) {
    lastUpdateTime = millis();  // Speichert die aktuelle Zeit
    updateGuestWifiInfo();      // Ruft die Funktion auf
  }

}

