/*
This is an example of how to use the WiFiManager library, customized for the OrtoSmart project at Fablab Bergamo.
*/

#include <WiFiManager.h> // https://github.com/tzapu/WiFiManager @ 7d498ed
#include "secrets.h"

void setup() {
    WiFi.mode(WIFI_STA); // explicitly set mode, esp defaults to STA+AP
    // it is a good practice to make sure your code sets wifi mode how you want it.

    // put your setup code here, to run once:
    Serial.begin(115200);
    
    //WiFiManager, Local intialization. Once its business is done, there is no need to keep it around
    WiFiManager wifiManager;

    // preload wifi credentials if known
    // wifiManager.preloadWiFi(WIFI_SSID, WIFI_PASS);

    // password protected ap
    wifiManager.setConfigPortalTimeout(30); 
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

}

void loop() {
    // put your main code here, to run repeatedly:   
}
