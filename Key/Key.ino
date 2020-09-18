/*
 * Future City Project
 *
 * Copyright (C) 2020 Sergey Denisov. GPLv3
 *
 * Written by Sergey Denisov aka LittleBuster (DenisovS21@gmail.com)
 */

#define BLYNK_PRINT Serial
#include <ESP8266WiFi.h>
#include <BlynkSimpleEsp8266.h>

/*
 * APPLICATION DEFINES
 */
#define DEVICE_KEY  ""
#define DEVICE_NAME "ВЕРАНДА"

#define WIFI_SSID   ""
#define WIFI_PASSWD ""

#define REEDS_COUNT 2

#define SERVER_ADDR ""
#define SERVER_PORT 8080

#define MAIN_TMR_DELAY  1000
#define BRIDGES_COUNT   2

#define LED_ON          255
#define LED_OFF         0
#define TRUE            1
#define FALSE           0
#define SERIAL_SPEED    9600
#define LED_COLOR_RED   "#D3435C"
#define LED_COLOR_YEL   "#ED9D00"

/*
 * VIRTUAL PINS DEFINES
 */
#define VP_STATUS_SWITCH    V6
#define VP_STATUS_LED_0     V0

/*
 * GPIO DEFINES
 */
#define KEY_PIN         D1
#define LED_ON_PIN      D2
#define LED_OFF_PIN     D3

/*
 * APPLICATION CLASSES
 */
SimpleTimer MainTimer;

WidgetBridge Bridge0(V10);
WidgetBridge Bridge1(V11);
WidgetBridge Bridge2(V12);
WidgetBridge Bridge3(V13);

/*
 * APPLICATION VARIABLES
 */
WidgetBridge *Bridges[] = {
    &Bridge0,
    &Bridge1,
    &Bridge2,
    &Bridge3
};

/*
 * BLYNK CALLBACKS
 */
BLYNK_CONNECTED()
{
    Bridge0.setAuthToken("");
    Bridge1.setAuthToken("");
    Bridge2.setAuthToken("");
    Bridge3.setAuthToken("");
    Blynk.syncAll();
}

BLYNK_WRITE(VP_STATUS_ON)
{
    if (param.asInt() == TRUE) {
    }
}

/*
 * OTHER FUNCTIONS
 */
void MainTimerCallback()
{
}

void ReadKey()
{
    if (digitalRead(KEY_PIN) == LOW) {
        for (uint8_t i = 0; i < BRIDGES_COUNT; i++) {
            Bridges[i]->virtualWrite(VP_STATUS_SWITCH, TRUE); 
        }
        MainTimerCallback();
        delay(1000);
    }
}

/*
 * APPLICATION MAIN FUNCTIONS
 */
void setup()
{
    Serial.begin(SERIAL_SPEED);
    Blynk.begin(DEVICE_KEY, WIFI_SSID, WIFI_PASSWD, SERVER_ADDR, SERVER_PORT);

    pinMode(KEY_PIN, INPUT_PULLUP);
    pinMode(LED_ON_PIN, OUTPUT);
    pinMode(LED_OFF_PIN, OUTPUT);

    MainTimer.setInterval(MAIN_TMR_DELAY, MainTimerCallback);
}

void loop()
{
    Blynk.run();

    MainTimer.run();

    ReadKey();
}
