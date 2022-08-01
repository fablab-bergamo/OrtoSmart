#if defined(ESP32)
#include <WiFiMulti.h>
WiFiMulti wifiMulti;
#define DEVICE "ESP32"
#elif defined(ESP8266)
#include <ESP8266WiFiMulti.h>
ESP8266WiFiMulti wifiMulti;
#define DEVICE "ESP8266"
#endif

#include <ESP32Ping.h>
#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "secrets.h"

// Set timezone string according to https://www.gnu.org/software/libc/manual/html_node/TZ-Variable.html
// Examples:
//  Pacific Time: "PST8PDT"
//  Eastern: "EST5EDT"
//  Japanesse: "JST-9"
//  Central Europe: "CET-1CEST,M3.5.0,M10.5.0/3"
#define TZ_INFO "CET-1CEST,M3.5.0,M10.5.0/3"

// InfluxDB client instance with preconfigured InfluxCloud certificate
InfluxDBClient client = InfluxDBClient(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);;

// Data point
Point sensor("wifi_status");

void setup() {
  Serial.begin(115200);
  // Wifi connect to get WiFi.SSID()
  wifiConnect();
  // Add tags
  sensor.addTag("device", DEVICE);
  sensor.addTag("SSID", WiFi.SSID());

  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  
}

void loop() {
  Serial.println("***************************");
  // Clear fields for reusing the point. Tags will remain untouched
  sensor.clearFields();

  // Execute here wifiConnect to get WiFi.RSSI()
  wifiConnect();
  // Store measured value into point
  // Report RSSI of currently connected network
  sensor.addField("rssi", WiFi.RSSI());

  // Print what are we exactly writing
  Serial.print("Writing: ");
  Serial.println(sensor.toLineProtocol());

  
  
  // Write point
  
  influxDBconnect();
  if (!client.writePoint(sensor)) {
    Serial.print("InfluxDB write failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  Serial.println("Wait 1s");
  WiFi.disconnect();
  WiFi.mode(WIFI_OFF);
  //delay(10 * 1000);
  esp_sleep_enable_timer_wakeup(10 * 1e6); // 10 second * 1e6 = 1000000 microseconds
  esp_light_sleep_start();
}

void wifiConnect(){
  // Setup wifi
  Serial.print("Connecting to wifi");
  WiFi.mode(WIFI_STA);
  WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
  while (WiFi.status() != WL_CONNECTED) {
    Serial.print('.');
    delay(100);
  }
  Serial.println();
}

void influxDBconnect(){
  Serial.println(WiFi.status() != WL_CONNECTED);
  // Reconnect to WiFi
  /*
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Reconnecting to wifi");
    unsigned long int dt = millis();
    WiFi.reconnect();
    Serial.print(" --> Elapsed ");
    Serial.print(millis() - dt);
    Serial.print(" ms");
    Serial.println();
    if (WiFi.status() != WL_CONNECTED) Serial.println("Connected");
  }
  */
  // Time sync
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");
  
  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connected to InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("InfluxDB connection failed: ");
    Serial.println(client.getLastErrorMessage());
  }

  HTTPOptions httpOptions = HTTPOptions();
  client.setHTTPOptions(httpOptions.connectionReuse(true));
  client.setHTTPOptions(httpOptions.httpReadTimeout(3600000));
}
