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

#define MAIN_TMR_DELAY      1000
#define SENSOR_TMR_DELAY    1000
#define ALARM_TMR_DELAY     200

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
#define VP_STATUS_LED_0     V0
#define VP_STATUS_LED_1     V1
#define VP_STATUS_LED_2     V2
#define VP_STATUS_LED_3     V3

#define VP_STATUS_OFF       V4
#define VP_STATUS_ON        V5
#define VP_STATUS_SWItCH    V6

#define VP_ALARM            V7

/*
 * GPIO DEFINES
 */
#define REED_PIN_0  D1
#define REED_PIN_1  D2
#define REED_PIN_2  D3
#define REED_PIN_3  D4
#define ALARM_PIN   D5

/*
 * APPLICATION CLASSES
 */
SimpleTimer MainTimer;
SimpleTimer SensorTimer;
SimpleTimer AlarmTimer;

WidgetLED LedStatus0(VP_STATUS_LED_0);
WidgetLED LedStatus1(VP_STATUS_LED_1);
WidgetLED LedStatus2(VP_STATUS_LED_2);
WidgetLED LedStatus3(VP_STATUS_LED_3);

/*
 * APPLICATION VARIABLES
 */
bool Status = false;
bool Alarm = false;
uint8_t LastAlarm = LOW;
uint8_t Speaker = ALARM_PIN;

uint8_t Reeds[] = {
    REED_PIN_0,
    REED_PIN_1,
    REED_PIN_2,
    REED_PIN_3
};

WidgetLED *Leds[] = {
    &LedStatus0,
    &LedStatus1,
    &LedStatus2,
    &LedStatus3
};

/*
 * BLYNK CALLBACKS
 */
BLYNK_CONNECTED()
{
    Blynk.syncAll();
}

BLYNK_WRITE(VP_STATUS_ON)
{
    if (param.asInt() == TRUE) {
        Status = true;
    }
}

BLYNK_WRITE(VP_STATUS_OFF)
{
    if (param.asInt() == TRUE) {
        Status = false;
    }
}

BLYNK_WRITE(VP_STATUS_SWItCH)
{
    if (param.asInt() == TRUE) {
        Status = !Status;
    }
}

BLYNK_WRITE(VP_ALARM)
{
    if (param.asInt() == TRUE) {
        Alarm = true;
    }
}

/*
 * OTHER FUNCTIONS
 */
void MainTimerCallback()
{
    if (Status) {
        for (uint8_t i = 0; i < REEDS_COUNT; i++) {
            Leds[i]->on();
            if (!Alarm) {
                Leds[i]->setColor(LED_COLOR_RED);
            }
        }
    } else {
        Alarm = false;
        Blynk.virtualWrite(VP_ALARM, FALSE);
        for (uint8_t i = 0; i < REEDS_COUNT; i++) {
            Leds[i]->off();
            Leds[i]->setColor(LED_COLOR_RED);
        }
    }
}

void SensorTimerCallback()
{
    if (Status && !Alarm) {
        for (uint8_t i = 0; i < REEDS_COUNT; i++) {
            if (digitalRead(Reeds[i]) != LOW) {
                if (!Alarm) {
                    Blynk.notify("ОХРАНА: Внимание! Проникновение! " DEVICE_NAME);
                }
                Alarm = true;
                Blynk.virtualWrite(VP_ALARM, TRUE);
                delay(500);
                Leds[i]->setColor(LED_COLOR_YEL);
                break;
            }
        }
    }
}

void AlarmTimerCallback()
{
    if (Alarm) {
        LastAlarm = !LastAlarm;
        digitalWrite(Speaker, LastAlarm);
    } else {
        digitalWrite(Speaker, LOW);
    }
}

/*
 * APPLICATION MAIN FUNCTIONS
 */
void setup()
{
    Serial.begin(SERIAL_SPEED);
    Blynk.begin(DEVICE_KEY, WIFI_SSID, WIFI_PASSWD, SERVER_ADDR, SERVER_PORT);

    MainTimer.setInterval(MAIN_TMR_DELAY, MainTimerCallback);
    SensorTimer.setInterval(SENSOR_TMR_DELAY, SensorTimerCallback);
    AlarmTimer.setInterval(ALARM_TMR_DELAY, AlarmTimerCallback);

    for (uint8_t i = 0; i < REEDS_COUNT; i++) {
        pinMode(Reeds[i], INPUT_PULLUP);
    }
    pinMode(Speaker, OUTPUT);
}

void loop()
{
    Blynk.run();

    MainTimer.run();
    SensorTimer.run();
    AlarmTimer.run();
}
