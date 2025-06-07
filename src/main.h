#pragma once

#include "secrets.h"
#include <Arduino.h>
#include <Update.h>
#include <Preferences.h>

#include <esp_system.h>
#include <Ticker.h>
#include <esp_sleep.h>

#include <WiFi.h>
#include <HTTPClient.h>
#include <ArduinoOTA.h>

#define SerialMon Serial
// #define SerialAT Serial0

#include <ArduinoJson.h>
#include <WiFiClientSecure.h>

#ifdef DEBUG
#define DEBUG_PRINT(x) SerialMon.print(x)
#define DEBUG_PRINTLN(x) SerialMon.println(x)
#else
#define DEBUG_PRINT(x)
#define DEBUG_PRINTLN(x)
#endif

WiFiClientSecure client;
extern const uint8_t rootca_crt_bundle_start[] asm("_binary_data_cert_x509_crt_bundle_bin_start");
HTTPClient http;

Preferences preferences;
String version;
IPAddress googleDNS(8, 8, 8, 8);
volatile bool isNTPSet = false;

#ifdef LILYGO_T_A7670

#define MODEM_BAUDRATE (115200)
#define MODEM_DTR_PIN (25)
#define MODEM_TX_PIN (26)
#define MODEM_RX_PIN (27)
// The modem boot pin needs to follow the startup sequence.
#define BOARD_PWRKEY_PIN (4)
#define BOARD_ADC_PIN (35)
// The modem power switch must be set to HIGH for the modem to supply power.
#define BOARD_POWERON_PIN (12)
#define MODEM_RING_PIN (33)
#define MODEM_RESET_PIN (5)
#define BOARD_MISO_PIN (2)
#define BOARD_MOSI_PIN (15)
#define BOARD_SCK_PIN (14)
#define BOARD_SD_CS_PIN (13)
#define BOARD_BAT_ADC_PIN (35)
#define MODEM_RESET_LEVEL HIGH
#define SerialAT Serial1

#define MODEM_GPS_ENABLE_GPIO (-1)

#define TINY_GSM_MODEM_A7670

// It is only available in V1.4 version. In other versions, IO36 is not connected.
#define BOARD_SOLAR_ADC_PIN (36)

#ifdef DEBUG
#define TINY_GSM_DEBUG SerialMon
#include <StreamDebugger.h>
#include <TinyGsmClient.h>
StreamDebugger debugger(SerialAT, SerialMon);
TinyGsm modem(debugger);
#else
#include <TinyGsmClient.h>
TinyGsm modem(SerialAT);
#endif
#include <TinySMS.h>
TinySMS modemSMS(modem);
#endif
