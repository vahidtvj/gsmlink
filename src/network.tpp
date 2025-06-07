#include "main.h"
#include <ESP32Ping.h>

bool initWiFi(uint32_t timeout = 20000)
{
    // WiFi.disconnect();
    // delay(1000);
    WiFi.setAutoReconnect(true);
    // WiFi.persistent(true);
    ulong t = millis();
    DEBUG_PRINT("Connecting to WiFi");
    WiFi.begin(ENV_SSID, ENV_SSID_PASSWORD);
    while (WiFi.status() != WL_CONNECTED)
    {
        if (millis() - t >= timeout)
            return false;
        delay(500);
        DEBUG_PRINT(".");
    }
    return true;
}

/**
 * @param method 0 for GET, 1 for post
 */
bool _sendHTTPWiFi(String url, String data, String &response, uint8_t method, uint8_t retry)
{
    // http.useHTTP10(true);
    http.begin(client, url);
    if (method == 1)
        http.addHeader("Content-Type", "application/json");

    DEBUG_PRINTLN("Start send");
    int httpResponseCode;
    for (uint8_t i = 0; i < retry; i++)
    {
        httpResponseCode = method == 0 ? http.GET() : http.POST(data);

        DEBUG_PRINT("HTTP Response code: ");
        DEBUG_PRINTLN(httpResponseCode);

        if (httpResponseCode == HTTP_CODE_OK)
            break;
    }
    if (httpResponseCode != HTTP_CODE_OK)
    {
        DEBUG_PRINTLN("send fail");
        http.end();
        return false;
    }

    response = http.getString();
    http.end();
    return true;
}

/**
 * Post request
 */
bool sendHTTPWiFi(String url, String data, String &response, uint8_t retry = 10)
{
    return _sendHTTPWiFi(url, data, response, 1, retry);
}
/**
 * Get request
 */
bool sendHTTPWiFi(String url, String &response, uint8_t retry = 10)
{
    return _sendHTTPWiFi(url, "", response, 0, retry);
}

bool sendHTTP(String url, String &response, uint8_t retry = 10)
{
    if (!isNTPSet)
        return sendHTTPGPRS(url, response, retry);
    if (WiFi.status() != WL_CONNECTED)
        return sendHTTPGPRS(url, response, retry);
    // IPAddress ip;
    // if (WiFi.hostByName("www.google.com", ip) != 1)
    //     return sendHTTPGPRS(url, response, retry);
    if (!Ping.ping(googleDNS, 3))
        return sendHTTPGPRS(url, response, retry);
    return sendHTTPWiFi(url, response, retry);
}

bool sendHTTP(String url, String data, String &response, uint8_t retry = 10)
{
    if (!isNTPSet)
        return sendHTTPGPRS(url, data, response, retry);
    if (WiFi.status() != WL_CONNECTED)
        return sendHTTPGPRS(url, data, response, retry);
    // IPAddress ip;
    // if (WiFi.hostByName("www.google.com", ip) != 1)
    //     return sendHTTPGPRS(url, data, response, retry);
    if (!Ping.ping(googleDNS, 3))
        return sendHTTPGPRS(url, data, response, retry);
    return sendHTTPWiFi(url, data, response, retry);
}