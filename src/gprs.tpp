#include "main.h"
#include <SSLClient.h>
#include <ArduinoHttpClient.h>
TinyGsmClient gprsClient(modem);
SSLClient ssl(&gprsClient);
// HttpClient httpGPRS(ssl, server, port);

/**
 * @param method 0 for GET, 1 for post
 */
bool _sendHTTPGPRS(String url, String data, String &response, uint8_t method, uint8_t retry)
{
    uint32_t port = 443;
    String host;
    String path;
    int protocolPos = url.indexOf("://");
    int start = (protocolPos != -1) ? protocolPos + 3 : 0;
    int slashPos = url.indexOf('/', start);
    if (slashPos != -1)
    {
        host = url.substring(start, slashPos);
        path = url.substring(slashPos);
    }
    else
    {
        host = url.substring(start);
        path = "/";
    }
    // ----------------
    HttpClient httpGPRS(ssl, host, port);
    httpGPRS.connectionKeepAlive(); // Currently, this is needed for HTTPS
    int httpResponseCode;
    int err;
    for (uint8_t i = 0; i < retry; i++)
    {
        err = method == 0 ? httpGPRS.get(path) : httpGPRS.post(path, "application/json", data);
        if (err != 0)
        {
            DEBUG_PRINT("Error: ");
            DEBUG_PRINTLN(err);
            continue;
        }
        httpResponseCode = httpGPRS.responseStatusCode();
        DEBUG_PRINT("HTTP Response code: ");
        DEBUG_PRINTLN(httpResponseCode);

        if (httpResponseCode == HTTP_CODE_OK)
            break;
    }
    if (httpResponseCode != 200)
    {
        DEBUG_PRINTLN("send fail");
        http.end();
        return false;
    }

    response = httpGPRS.responseBody();
    httpGPRS.stop();
    return true;
}

bool setupGPRS()
{
    if (!wakeModem())
        return false;
    setBaud(115200);
    if (!modem.waitForNetwork())
    {
        DEBUG_PRINTLN("No network");
        setBaud(9600);
        return false;
    }

    if (!modem.gprsConnect(GPRS_APN, GPRS_USER, GPRS_PASS))
    {
        DEBUG_PRINTLN("Failed to connect GPRS");
        setBaud(9600);
        return false;
    }
    return true;
}

void endGPRS()
{
    modem.gprsDisconnect();
    setBaud(9600);
}

/**
 * Post request
 */
bool sendHTTPGPRS(String url, String data, String &response, uint8_t retry = 10)
{
    if (!setupGPRS())
        return false;

    const bool ret = _sendHTTPGPRS(url, data, response, 1, retry);
    endGPRS();
    return ret;
}
/**
 * Get request
 */
bool sendHTTPGPRS(String url, String &response, uint8_t retry = 10)
{
    if (!setupGPRS())
        return false;

    const bool ret = _sendHTTPGPRS(url, "", response, 0, retry);
    endGPRS();
    return ret;
}
