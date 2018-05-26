/*
 *  This sketch demonstrates how to scan WiFi networks.
 *  The API is almost the same as with the WiFi Shield library,
 *  the most obvious difference being the different file you need to include:
 */
#include <Adafruit_DotStarMatrix.h>
#include "WiFi.h"

#define DATAPIN  27
#define CLOCKPIN 13

Adafruit_DotStarMatrix matrix = Adafruit_DotStarMatrix(
        12, 6, DATAPIN, CLOCKPIN,
        DS_MATRIX_BOTTOM     + DS_MATRIX_LEFT +
        DS_MATRIX_ROWS + DS_MATRIX_PROGRESSIVE,
        DOTSTAR_BGR);

void setup()
{
    Serial.begin(115200);

    // Set WiFi to station mode and disconnect from an AP if it was previously connected
    WiFi.mode(WIFI_STA);
    WiFi.disconnect();
    delay(100);

    matrix.begin();

#ifdef HIDE_THIS_A_BiT
    WiFi.softAP("LittleTeapot", "short-stout");
    WiFi.beginSmartConfig();
    while (!WiFi.smartConfigDone()) {
        delay(500);
        Serial.print(".");
    }
    while (WiFi.status() != WL_CONNECTED) {
        delay(500);
        Serial.print(":");
    }
    Serial.println("WiFi Connected.");
    Serial.print("IP Address: ");
    Serial.println(WiFi.localIP());
#endif

    Serial.println("Setup done");

}

char *preferred_ssids[] = {
    "Hanenfast",
    "The Fellow Guest",
};

uint32_t signal_colors[] = {
    0xFF0000, // red
    0xFF7F00, // orange
    0xFFFF00, // yellow
    0x7FFF00, // lime green
    0x00FF00, // green
    0x00FF7F, // turquoise
    0x00FFFF, // cyan
    0x007FFF, // cerulean
    0x0000FF, // blue
    0x7F00FF, // purple,
    0xFF00FF, // magenta
    0xFFFFFF, // white
};

uint16_t black = matrix.Color(0, 0, 0);

int signal_values[] = {
    -100, // awful
    -94,  // terrible
    -88,  // dismal
    -82,  // pathetic
    -76,  // meh
    -70,  // good
    -64,  // great
    -58,  // superb
    -52,  // fantastic
    -46,  // awesome
    -40,  // amazing
    -34,  // astonishing
};

void loop()
{
    Serial.println("scan start");

    int best_rssi = -1000;

    // WiFi.scanNetworks will return the number of networks found
    int n = WiFi.scanNetworks();
    Serial.println("scan done");
    if (n == 0) {
        Serial.println("no networks found");
    } else {
        Serial.print(n);
        Serial.println(" networks found");
        for (int i = 0; i < n; ++i) {
            for (int j = 0; j < 2; ++j) {
                if (WiFi.SSID(i) == preferred_ssids[j] && WiFi.RSSI(i) > best_rssi) {
                    best_rssi = WiFi.RSSI(i);
                }
            }

            // Print SSID and RSSI for each network found
            Serial.print(i + 1);
            Serial.print(": ");
            Serial.print(WiFi.SSID(i));
            Serial.print(" (");
            Serial.print(WiFi.RSSI(i));
            Serial.print(")");
            Serial.println((WiFi.encryptionType(i) == WIFI_AUTH_OPEN)?" ":"*");
            delay(10);
        }
    }
    Serial.println("");

    matrix.setBrightness(40);
    matrix.fillScreen(black);
    for (int i = 0; i < 12; ++i) {
        if (signal_values[i] > best_rssi) break;
        matrix.setPassThruColor(signal_colors[i]);
        matrix.drawFastVLine(i, 0, 6, signal_colors[i]);
    }
    // for (int i = 0; i < 12; ++i) {
    //     matrix.setPassThruColor(signal_colors[i]);
    //     matrix.drawFastVLine(i, 0, 6, signal_colors[i]);
    // }
    matrix.setPassThruColor();
    matrix.show();

    // Wait a bit before scanning again
    //delay(5000);
}
