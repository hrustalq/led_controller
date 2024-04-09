#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <FastLEDHub.h>

#define LED_PIN     2
#define NUM_LEDS    30
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MAX_RETRIES 5
#define RETRY_DELAY 2000
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define LIGHTSTRIP_PIN 2

// structures setup

struct {
  char id[30];
  char wiFiSsid[30];
  char wiFiPassword[30];
  char targetId[30];
  char ipAddress[30];
  char port[30];
  char ledStripOnColor[30];
  char ledStripOffColor[30];
  char active[1];
} deviceSetup;

// common entities

ESP8266WebServer  server(80);
WiFiClient client;
HTTPClient http;
CRGB leds[NUM_LEDS];

// basic functions
void pinoutSetup() {
  pinMode(LED_PIN, OUTPUT);
}

void startSerialCommunication() {
  Serial.begin(115200);
  delay(300);
}

void blinkLed(int interval) {
  digitalWrite(LED_PIN, HIGH);
  delay(interval);
  digitalWrite(LED_PIN, LOW);
  delay(interval);
}

// led setup
void setupLed() {
  FastLEDHub.addLeds<LED_TYPE, LIGHTSTRIP_PIN, GRB>(leds, NUM_LEDS);
}

void paintLedStrip() {
  Serial.println(bool(deviceSetup.active));
  if (bool(deviceSetup.active)) {
    fill_solid(leds, NUM_LEDS, CRGB::Green);
  } else {
    fill_solid(leds, NUM_LEDS, CRGB::Red);
  }
  FastLEDHub.show();
  delay(1000);
}

// eeprom workaround

void initEeprom() {
  EEPROM.begin(sizeof(deviceSetup));
  EEPROM.get(0, deviceSetup);

  Serial.println("Read from EEPROM:");

  Serial.print("WiFi SSID: ");
  Serial.println(deviceSetup.wiFiSsid);
  Serial.print("WiFi password: ");
  Serial.println(deviceSetup.wiFiPassword);

  Serial.print("ID: ");
  Serial.println(deviceSetup.id);

  Serial.print("Server ip: ");
  Serial.println(deviceSetup.ipAddress);
  Serial.print("Server port: ");
  Serial.println(deviceSetup.port);

  Serial.print("Target id: ");
  Serial.println(deviceSetup.targetId);

  Serial.print("Led strip on color: ");
  Serial.println(deviceSetup.ledStripOnColor);
  Serial.print("Led strip off color: ");
  Serial.println(deviceSetup.ledStripOffColor);
}

// wifi setup

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  delay(100);

  Serial.print("Connecting to WiFi ");

  for (int attempt = 0; attempt < MAX_RETRIES; ++attempt) {
    WiFi.begin(deviceSetup.wiFiSsid, deviceSetup.wiFiPassword);
    Serial.print(".");

    unsigned long startAttemptTime = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - startAttemptTime < RETRY_DELAY) {
      delay(100);
    }

    if (WiFi.status() == WL_CONNECTED) {
      Serial.println();
      Serial.println("WiFi connected successfully!");
      Serial.print("IP Address: ");
      Serial.println(WiFi.localIP());
      return;
    } else {
      Serial.print(" (Attempt ");
      Serial.print(attempt + 1);
      Serial.print("/");
      Serial.print(MAX_RETRIES);
      Serial.print(" failed)");

      if (attempt < MAX_RETRIES - 1) {
        Serial.print(", retrying in ");
        Serial.print(RETRY_DELAY / 1000);
        Serial.println(" seconds...");
        delay(RETRY_DELAY);
      } else {
        Serial.println();
        Serial.println("Failed to connect to WiFi.");

        // Create WiFi network with unique ID (MAC address)
        Serial.println("Falling back to AP mode.");
        WiFi.disconnect();
        WiFi.mode(WIFI_AP);
        delay(100);

        // Check if unique_id exists in user_settings
        String uniqueSSID = "ESP32_";
        if (sizeof(deviceSetup.id) > 2) {
          uniqueSSID += deviceSetup.id;
        } else {
          uniqueSSID += WiFi.macAddress();
        }
        WiFi.softAP(uniqueSSID.c_str());
        Serial.print("WiFi access point network created. SSID: ");
        Serial.println(uniqueSSID);
        Serial.print("IP Address (AP mode): ");
        Serial.println(WiFi.softAPIP());
        return;
      }
    }
  }
}

// web ui for initial setup

String settingsHtmlForm = R"(
<!DOCTYPE html>
<html lang='ru'>
  <head>
    <meta charset='UTF-8'>
    <meta name='viewport' content='width=device-width, initial-scale=1.0'>
    <title>Configuration Form</title>
  </head>
  <body>
    <h2>Configuration Form</h2>
    <form id='configForm' method='post' action='/save-settings'>
      <label for='wifiSSID'>WiFi SSID:</label><br>
      <input type='text' id='wifiSSID' name='wifiSSID' required><br>

      <label for='wifiPassword'>WiFi Password:</label><br>
      <input type='password' id='wifiPassword' name='wifiPassword' required><br>

      <label for='id'>ID:</label><br>
      <input type='text' id='id' name='id' required><br>

      <label for='targetId'>Target host ID:</label><br>
      <input type='text' id='targetId' name='targetId' required><br>

      <label for='serverIP'>Server IP Address:</label><br>
      <input type='text' id='serverIP' name='serverIP' required><br>

      <label for='serverPort'>Server Port:</label><br>
      <input type='number' id='serverPort' name='serverPort' required><br><br>

      <input type='submit' value='Submit'>
    </form>
    <button id="blink-button">Blink led</button>
  </body>
  <script>
    const button = document.getElementById('blink-button');
    button.addEventListener('click', (e) => {
      e.preventDefault();
      fetch('/blink', {
        method: 'POST',
      });
    });
  </script>
</html>
)";

// http client
void httpHostStatusRequest() {
  if (deviceSetup.targetId[0] == '\0' || deviceSetup.ipAddress[0] == '\0') {
    return;
  }

  String URL = "http://" + String(deviceSetup.ipAddress) + ":" + String(deviceSetup.port) + "/hosts/" + String(deviceSetup.targetId) + "/status";

  http.begin(client, URL);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    payload.toCharArray(deviceSetup.active, sizeof(deviceSetup.active));
    Serial.print("Status:");
    Serial.print(payload);
    Serial.print(" Target:");
    Serial.print(deviceSetup.targetId);
    Serial.print(" active:");
    Serial.print(deviceSetup.active);
  } else {
    Serial.printf("[HTTP] ... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();

  delay(1000);
}

// web server

void fetchActiveStatus() {

}

void handesSettingsPageVisit() {
  server.send(200, "text/html", settingsHtmlForm);
}

void handleBlinkButtonCall() {
  Serial.println("Blink request");
  for (int i = 0; i < 10; i++) {
    blinkLed(300);
  }
  server.send(200);
}

void handleSaveSettingsReq() {
  String ssid = server.arg("wifiSSID");
  String password = server.arg("wifiPassword");

  String uniqueId = server.arg("id");
  String targetId = server.arg("targetId");

  String serverIpAddress = server.arg("serverIP");
  String serverPort = server.arg("serverPort");

  uniqueId.toCharArray(deviceSetup.id, sizeof(deviceSetup.id));
  targetId.toCharArray(deviceSetup.targetId, sizeof(deviceSetup.targetId));

  ssid.toCharArray(deviceSetup.wiFiSsid, sizeof(deviceSetup.wiFiSsid));
  password.toCharArray(deviceSetup.wiFiPassword, sizeof(deviceSetup.wiFiPassword));

  serverIpAddress.toCharArray(deviceSetup.ipAddress, sizeof(deviceSetup.ipAddress));
  serverPort.toCharArray(deviceSetup.port, sizeof(deviceSetup.port));

  EEPROM.put(0, deviceSetup);
  EEPROM.commit();

  server.send(200, "text/plain", "Settings saved");
  delay(200);

  ESP.restart();
}

void setupWebServer() {
  server.on("/", HTTP_GET, handesSettingsPageVisit);
  server.on("/save-settings", HTTP_POST, handleSaveSettingsReq);
  server.on("/blink", HTTP_POST, handleBlinkButtonCall);
  server.begin();
}


void setup() {
  http.addHeader("Content-Type", "application/json");
  pinoutSetup();
  startSerialCommunication();
  setupLed();

  for (int i = 0; i < 3; i++) {
    blinkLed(300);
  }

  initEeprom();

  connectWiFi();
  setupWebServer();
}

void loop() {
  server.handleClient();
  httpHostStatusRequest();
  paintLedStrip();
  FastLEDHub.handle();
}
