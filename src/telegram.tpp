#include "main.h"

bool sendTelegramMessage(String message, String chatId = CHAT_ID, String url = BOT_SEND_URL)
{
    JsonDocument doc;
    String data;
    doc["chat_id"] = chatId;
    doc["url"] = url;
    doc["text"] = message;

    // StreamDebugger debugger(client, SerialMon);
    serializeJson(doc, data);

    if (!sendHTTP(PROXY_URL, data, data))
        return false;
    DeserializationError error = deserializeJson(doc, data);
    if (error)
    {
        DEBUG_PRINT(F("deserializeJson() failed: "));
        DEBUG_PRINTLN(error.f_str());
        client.stop();
        return false;
    }
    bool ok = doc["ok"].as<bool>();
    doc.clear();
    if (!ok)
        return false;
    return true;
}