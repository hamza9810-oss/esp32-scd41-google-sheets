#include <WiFi.h>
#include <WiFiClientSecure.h>
#include <HTTPClient.h>
#include <Wire.h>
#include <SensirionI2cScd4x.h>

// ---------- WiFi ----------
const char* ssid     = "Glide-2795";
const char* password = "8586507859";

// ✅ Your Apps Script URL (ends with /exec)
const char* GOOGLE_SCRIPT_URL =
  "https://script.google.com/macros/s/AKfycbxsbHhlKsz2R32YzLV0eZ5ERSAfQkos5LDEwt86KbusPVt6mxux8uGQaJVmA5cdmk2V/exec";

// ---------- SCD4x ----------
SensirionI2cScd4x co2Sensor;
#define I2C_SDA 8
#define I2C_SCL 9
#define SCD4X_I2C_ADDR 0x62

void connectWiFi() {
  WiFi.mode(WIFI_STA);
  WiFi.setSleep(false);
  WiFi.begin(ssid, password);

  Serial.print("Connecting to WiFi");
  unsigned long start = millis();
  while (WiFi.status() != WL_CONNECTED && millis() - start < 20000) {
    delay(500);
    Serial.print(".");
  }

  if (WiFi.status() == WL_CONNECTED) {
    Serial.println("\nWiFi connected!");
    Serial.print("IP address: ");
    Serial.println(WiFi.localIP());
  } else {
    Serial.println("\nWiFi NOT connected (timeout).");
  }
}

// ---- GET helper with redirect handling ----
bool httpGetFollowRedirect(String url, String &bodyOut, int &codeOut) {
  WiFiClientSecure client;
  client.setInsecure();

  HTTPClient http;

  // IMPORTANT: tell HTTPClient to collect Location header
  const char* keys[] = {"Location"};
  http.collectHeaders(keys, 1);

  if (!http.begin(client, url)) return false;

  int code = http.GET();

  // Grab body (sometimes useful for debug)
  String body = http.getString();

  // If redirect, read Location header
  if (code == 301 || code == 302 || code == 303 || code == 307 || code == 308) {
    String location = http.header("Location");
    http.end();

    Serial.print("HTTP redirect code: ");
    Serial.println(code);
    Serial.print("Location header: ");
    Serial.println(location);

    if (location.length() == 0) {
      // No location: return debug info
      codeOut = code;
      bodyOut = body.length() ? body : "Redirect but Location header empty";
      return false;
    }

    // Follow redirect
    WiFiClientSecure client2;
    client2.setInsecure();

    HTTPClient http2;
    // Collect again for safety (some redirects chain)
    http2.collectHeaders(keys, 1);

    if (!http2.begin(client2, location)) return false;

    int code2 = http2.GET();
    String body2 = http2.getString();

    // Handle a second redirect if it happens
    if (code2 == 301 || code2 == 302 || code2 == 303 || code2 == 307 || code2 == 308) {
      String location2 = http2.header("Location");
      http2.end();

      Serial.print("Second redirect code: ");
      Serial.println(code2);
      Serial.print("Second Location: ");
      Serial.println(location2);

      if (location2.length() == 0) {
        codeOut = code2;
        bodyOut = body2.length() ? body2 : "Second redirect but Location empty";
        return false;
      }

      WiFiClientSecure client3;
      client3.setInsecure();
      HTTPClient http3;

      if (!http3.begin(client3, location2)) return false;

      int code3 = http3.GET();
      bodyOut = http3.getString();
      codeOut = code3;
      http3.end();
      return true;
    }

    bodyOut = body2;
    codeOut = code2;
    http2.end();
    return true;
  }

  // Normal response (200 etc)
  http.end();
  bodyOut = body;
  codeOut = code;
  return true;
}

// ---------- Send to Google Sheets ----------
bool sendToGoogleSheets(uint16_t co2, float temp, float rh) {
  if (WiFi.status() != WL_CONNECTED) return false;

  String url = String(GOOGLE_SCRIPT_URL);
  url += "?co2=" + String(co2);
  url += "&temp=" + String(temp, 2);
  url += "&rh=" + String(rh, 2);

  Serial.print("Sending to Google: ");
  Serial.println(url);

  String body;
  int code = -1;
  bool ok = httpGetFollowRedirect(url, body, code);

  Serial.print("Final HTTP code: ");
  Serial.println(code);
  Serial.print("Final body: ");
  Serial.println(body);

  // Your Apps Script should return "OK"
  return ok && (code == 200) && (body.indexOf("OK") >= 0);
}

void setup() {
  Serial.begin(115200);
  delay(2000);

  connectWiFi();

  Wire.begin(I2C_SDA, I2C_SCL);
  co2Sensor.begin(Wire, SCD4X_I2C_ADDR);

  int16_t error;
  char errMsg[256];

  error = co2Sensor.stopPeriodicMeasurement();
  if (error) {
    errorToString(error, errMsg, 256);
    Serial.print("stopPeriodicMeasurement error: ");
    Serial.println(errMsg);
  }
  delay(500);

  error = co2Sensor.startPeriodicMeasurement();
  if (error) {
    errorToString(error, errMsg, 256);
    Serial.print("startPeriodicMeasurement error: ");
    Serial.println(errMsg);
  } else {
    Serial.println("SCD4x periodic measurement started!");
  }
}

void loop() {
  // Reconnect WiFi if dropped
  static unsigned long lastWifiCheck = 0;
  if (millis() - lastWifiCheck > 10000) {
    lastWifiCheck = millis();
    if (WiFi.status() != WL_CONNECTED) {
      Serial.println("WiFi lost, reconnecting...");
      connectWiFi();
    }
  }

  int16_t error;
  char errMsg[256];

  bool dataReady = false;
  error = co2Sensor.getDataReadyStatus(dataReady);
  if (error) {
    errorToString(error, errMsg, 256);
    Serial.print("getDataReadyStatus error: ");
    Serial.println(errMsg);
    delay(1000);
    return;
  }

  if (dataReady) {
    uint16_t co2;
    float temp, rh;

    error = co2Sensor.readMeasurement(co2, temp, rh);
    if (error) {
      errorToString(error, errMsg, 256);
      Serial.print("readMeasurement error: ");
      Serial.println(errMsg);
    } else if (co2 > 0) {
      Serial.print("CO2: ");
      Serial.print(co2);
      Serial.print(" ppm, T: ");
      Serial.print(temp, 2);
      Serial.print(" °C, RH: ");
      Serial.print(rh, 2);
      Serial.println(" %");

      bool sent = sendToGoogleSheets(co2, temp, rh);
      Serial.println(sent ? "Sent OK ✅" : "Send FAILED ❌");
    }
  }

  delay(5000);
}
