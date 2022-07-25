/**
 * Secure Write Example code for InfluxDBClient library for Arduino
 * This example code is modified for the OrtoSmart project at Fablab Bergamo.
 **/


#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager @ 7d498ed

#include <InfluxDbClient.h>
#include <InfluxDbCloud.h>
#include "secrets.h"


#include <DHTStable.h>
DHTStable DHT;
#define DHT11_PIN 4

#include <BH1750.h>
#include <Wire.h>

BH1750 lightMeter;

#include <BMP280_DEV.h>                           // Include the BMP280_DEV.h library
float bmp_temperature, bmp_pressure, bmp_altitude;            // Create the temperature, pressure and altitude variables
BMP280_DEV bmp280;                                // Instantiate (create) a BMP280_DEV object and set-up for I2C operation (address 0x77)


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

  // DEBUG ONLY
  // wifiManager.preloadWiFi(WIFI_SSID, WIFI_PASS);

  bmp280.begin(0x76);  
  Wire.begin();
  lightMeter.begin();
  bmp280.setTimeStandby(TIME_STANDBY_05MS);     // Set the standby time to 2 seconds
  bmp280.startNormalConversion();                 // Start BMP280 continuous conversion in NORMAL_MODE   
  // password protected ap    
  wifiManager.setConfigPortalTimeout(30);
  bool result;    
  result = wifiManager.autoConnect("OrtoSmart-1","password"); 

  if(!result) {
      Serial.println("Non connesso :(");
      esp_sleep_enable_timer_wakeup(6e7); // sleep for 60 seconds and reset
      esp_deep_sleep_start();
  } 
  else {
      //if you get here you have connected to the WiFi    
      Serial.println("Connesso :)");
  }

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
    esp_sleep_enable_timer_wakeup(6e7); // sleep for 60 seconds
    esp_deep_sleep_start();
  }
}

void loop() {
  // Store measured value into point
  datapoint.clearFields();
  // Report RSSI of currently connected network
  datapoint.addField("rssi", WiFi.RSSI());
  
  int chk = DHT.read11(DHT11_PIN);
  if (chk == DHTLIB_OK){
      // datapoint.addField("temperature", DHT.temperature());
      datapoint.addField("humidity", DHT.getHumidity());
  } else {
      Serial.print("DHT error\n"); 
  }

  if (lightMeter.measurementReady()) {
    float lux = lightMeter.readLightLevel();
    Serial.print("Light: ");
    Serial.print(lux);
    Serial.println(" lx\n");
    datapoint.addField("light", lux);
  }
  else{}

  // orribile readout bloccante 
  int bmp_timeout = millis();
  bool bmp_ready = false;
  while(!bmp_ready && (millis() - bmp_timeout) < 1000) {
    bmp_ready = bmp280.getMeasurements(bmp_temperature, bmp_pressure, bmp_altitude);
    delay(1);
  }
  if (bmp_ready){
    Serial.print(bmp_temperature);                    // Display the results    
    Serial.print(F("*C   "));
    datapoint.addField("temperature", bmp_temperature);
    Serial.print(bmp_pressure);    
    Serial.print(F("hPa   "));
    datapoint.addField("pressure", bmp_pressure);
    Serial.print(bmp_altitude);
    Serial.println(F("m"));  
  }
  else {
    Serial.println("BMP280 timeout");
    // piuttosto che niente è meglio piuttosto
    datapoint.addField("temperature", DHT.getTemperature());
  }
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

  esp_sleep_enable_timer_wakeup(1 * 1e6); // 10 second * 1e6 = 1000000 microseconds
  esp_deep_sleep_start();
}
