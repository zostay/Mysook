#include <Serial.h>
#include <SPI.h>
#include <DoorlightSPI.h>
#include <MC_Network.h>

void setup() {
    // Start up the serial connection for debugging
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    Serial.println("Setup done");
}

void loop() {
}
