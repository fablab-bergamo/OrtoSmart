/**
 * Secure Write Example code for InfluxDBClient library for Arduino
 * This example code is modified for the OrtoSmart project at Fablab Bergamo.
 **/


#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager @ 7d498ed

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
InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN, InfluxDbCloud2CACert);
// InfluxDB client instance without preconfigured InfluxCloud certificate for insecure connection 
//InfluxDBClient client(INFLUXDB_URL, INFLUXDB_ORG, INFLUXDB_BUCKET, INFLUXDB_TOKEN);

// Data point
Point datapoint("wifi_status");

void setup() {
  Serial.begin(115200);

  WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
  // it is a good practice to make sure your code sets wifi mode how you want it.  
  //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
  WiFiManager wifiManager;

  // preload wifi credentials if known
  wifiManager.preloadWiFi(WIFI_SSID, WIFI_PASS);

  // password protected ap    
  bool result;    
  result = wifiManager.autoConnect("OrtoSmart-1","password"); 

  if(!result) {
      Serial.println("Non connesso :(");
      while(1);
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Connesso :)");
  }


  // Add tags
  datapoint.addTag("device", "sensore-1");
  datapoint.addTag("SSID", WiFi.SSID());

  // Accurate time is necessary for certificate validation and writing in batches
  // For the fastest time sync find NTP servers in your area: https://www.pool.ntp.org/zone/
  // Syncing progress and the time will be printed to Serial.
  timeSync(TZ_INFO, "pool.ntp.org", "time.nis.gov");

  // Check server connection
  if (client.validateConnection()) {
    Serial.print("Connesso a InfluxDB: ");
    Serial.println(client.getServerUrl());
  } else {
    Serial.print("Connessione fallita a InfluxDB: ");
    Serial.println(client.getLastErrorMessage());
  }
}

void loop() {
  // Store measured value into point
  datapoint.clearFields();
  // Report RSSI of currently connected network
  datapoint.addField("rssi", WiFi.RSSI());
  // Print what are we exactly writing
  Serial.print("Scrittura: ");
  Serial.println(client.pointToLineProtocol(datapoint));
  // If no Wifi signal, try to reconnect it
  if (WiFi.status() != WL_CONNECTED) {
    Serial.println("Persa connessione WiFi");
  }
  // Write point
  if (!client.writePoint(datapoint)) {
    Serial.print("Scrittura InfluxDB fallita: ");
    Serial.println(client.getLastErrorMessage());
  }

  //Wait 1s
  Serial.println("delay 1s");
  delay(1000);
}
