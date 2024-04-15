#include <ESP8266WiFi.h>
#include <WiFiClient.h>
#include <ESP8266WebServer.h>
#include <ESP8266HTTPClient.h>
#include <EEPROM.h>
#include <FastLEDHub.h>

#define LED_PIN     2
#define NUM_LEDS    30
#define MAX_RETRIES 5
#define LED_TYPE    WS2812B
#define COLOR_ORDER GRB
#define MAX_RETRIES 5
#define RETRY_DELAY 2000
#define FASTLED_ESP8266_RAW_PIN_ORDER
#define FASTLED_ESP8266_NODEMCU_PIN_ORDER
#define LIGHTSTRIP_PIN 2

// structures setup

struct DeviceSetup {
  char id[30];
  char wiFiSsid[30];
  char wiFiPassword[30];
  char targetId[30];
  char ipAddress[30];
  char port[30];
  char ledStripOnColor[30];
  char ledStripOffColor[30];
  char active[10];

  DeviceSetup() {
    strcpy(id, "");
    strcpy(wiFiSsid, "");
    strcpy(wiFiPassword, "");
    strcpy(targetId, "");
    strcpy(ipAddress, "");
    strcpy(port, "3000");
    strcpy(ledStripOnColor, "");
    strcpy(ledStripOffColor, "");
    strcpy(active, "3");
  }
};

DeviceSetup deviceSetup;

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
  CRGB color = CRGB::Yellow;

  switch (atoi(deviceSetup.active)) {
    case 0:
        color = CRGB::Green;
        break;
    case 1:
        color = CRGB::Red;
        break;
    default:
        color = CRGB::Yellow;
        break;
  }

  fill_solid(leds, NUM_LEDS, color);
  FastLEDHub.show();
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

  Serial.println("Connecting to WiFi ");

  for (int attempt = 0; attempt < MAX_RETRIES; ++attempt) {
    WiFi.begin(deviceSetup.wiFiSsid, deviceSetup.wiFiPassword);

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
    }

    Serial.printf(" (Attempt %d/%d failed), retrying in %d seconds...\n", attempt + 1, MAX_RETRIES, RETRY_DELAY / 1000);
    delay(RETRY_DELAY);
  }

  // If connection fails after max retries, fall back to AP mode
  Serial.println("Failed to connect to WiFi. Falling back to AP mode.");
  WiFi.disconnect();
  WiFi.mode(WIFI_AP);
  delay(100);

  // Create WiFi access point network
  String uniqueSSID = "ESP32_" + String(deviceSetup.id);
  WiFi.softAP(uniqueSSID.c_str());

  Serial.print("WiFi access point network created. SSID: ");
  Serial.println(uniqueSSID);
  Serial.print("IP Address (AP mode): ");
  Serial.println(WiFi.softAPIP());
}

// web ui for initial setup

String settingsHtmlForm = R"(
<!DOCTYPE html>
<html lang="ru">
<head>
    <meta charset="UTF-8">
    <meta name="viewport" content="width=device-width, initial-scale=1.0">
    <title>Led controller</title>
    <link href="https://cdn.jsdelivr.net/npm/bootstrap@5.3.3/dist/css/bootstrap.min.css" rel="stylesheet" integrity="sha384-QWTKZyjpPEjISv5WaRU9OFeRpok6YctnYmDr5pNlyT2bRjXh0JMhjY6hW+ALEwIH" crossorigin="anonymous">
</head>
<body style="min-height: 100dvh; width: 100%;">
    <div class="container my-4">
        <h2 style="text-align: center;">Настройки</h2>
        <div class="mb-3">
            <form class="d-flex flex-column" id='configForm' method='post' action='/save-settings'>
                <label class="form-label" for='wifiSSID'>WiFi SSID:</label>
                <input class="form-control" type='text' id='wifiSSID' name='wifiSSID' required><br>
          
                <label class="form-label" for='wifiPassword'>WiFi Password:</label>
                <input class="form-control" type='password' id='wifiPassword' name='wifiPassword' required><br>
          
                <label class="form-label" for='id'>ID:</label>
                <input class="form-control" type='text' id='id' name='id' required><br>
          
                <label class="form-label" for='targetId'>Target host ID:</label>
                <input class="form-control" type='text' id='targetId' name='targetId' required><br>
          
                <label class="form-label" for='serverIP'>Server IP Address:</label>
                <input class="form-control" type='text' id='serverIP' name='serverIP' required><br>
          
                <label class="form-label" for='serverPort'>Server Port:</label>
                <input class="form-control" type='number' id='serverPort' name='serverPort' required><br><br>
          
                <input class="btn btn-primary" type='submit' value='Сохранить'>
              </form>
        </div>
        <div class="mb-3">
            <button class="btn btn-secondary" id="blink-button">Blink Led</button>
        </div>
    </div>
</body>
</html>
)";

// http client
unsigned long lastRequestTime = 0;
const unsigned long requestInterval = 5000;

void httpHostStatusRequest() {
  if (deviceSetup.targetId[0] == '\0' || deviceSetup.ipAddress[0] == '\0') {
    return;
  }

  unsigned long currentTime = millis();
  if (currentTime - lastRequestTime < requestInterval) {
    return;
  }

  lastRequestTime = currentTime;

  String URL = "http://" + String(deviceSetup.ipAddress) + ":" + String(deviceSetup.port) + "/hosts/" + String(deviceSetup.targetId) + "/status";

  http.begin(client, URL);

  int httpCode = http.GET();

  if (httpCode == HTTP_CODE_OK) {
    String payload = http.getString();
    payload.toCharArray(deviceSetup.active, sizeof(deviceSetup.active));
  } else {
    strcpy(deviceSetup.active, "3");
    Serial.printf("[HTTP] ... failed, error: %s\n", http.errorToString(httpCode).c_str());
  }

  http.end();
}

// web server

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
  delay(1000);
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
