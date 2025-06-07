#include "main.h"
#include <aes/esp_aes.h>
bool doOTA()
{
    DEBUG_PRINTLN("OTA check");
    if (WiFi.status() != WL_CONNECTED)
        return false;
    IPAddress ip;
    if (WiFi.hostByName("www.google.com", ip) != 1)
        return false;

    const uint8_t retry = 10;
    http.begin(client, ENV_GIT_URL);
    http.addHeader("Accept", "application/vnd.github.v3+json");
    // http.addHeader("Authorization", ENV_GIT_PAT);
    http.addHeader("X-GitHub-Api-Version", "2022-11-28");

    int httpResponseCode;
    for (uint8_t i = 0; i < retry; i++)
    {
        httpResponseCode = http.GET();

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

    String data = http.getString();
    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, data);
    if (error)
    {
        DEBUG_PRINTLN("Parsing error");
        return false;
    }
    http.end();

    String tag = doc["tag_name"];
    if (tag == version)
    {
        DEBUG_PRINTLN("No update");
        return true;
    }

    String body = doc["body"];
    String url;
    String md5;
    int fileSize = 0;

    int startIndex = 0;
    int endIndex = body.indexOf('\n');
    DEBUG_PRINTLN("Body content:");
    DEBUG_PRINTLN(body);

    int binNameIndex = body.indexOf(BIN_NAME);
    if (binNameIndex != -1)
    {
        int lineEndIndex = body.indexOf('\n', binNameIndex);
        if (lineEndIndex == -1)
        {
            lineEndIndex = body.length();
        }

        String currentLine = body.substring(binNameIndex, lineEndIndex);
        DEBUG_PRINTLN("Current line:");
        DEBUG_PRINTLN(currentLine);

        int firstSpaceIndex = currentLine.indexOf(' ');
        int secondSpaceIndex = currentLine.indexOf(' ', firstSpaceIndex + 1);

        if (firstSpaceIndex != -1 && secondSpaceIndex != -1)
        {
            md5 = currentLine.substring(firstSpaceIndex + 1, secondSpaceIndex); // Extract MD5 hash
            fileSize = currentLine.substring(secondSpaceIndex + 1).toInt();     // Extract file size
        }
        else
        {
            DEBUG_PRINTLN("Invalid line format");
        }
    }
    else
    {
        DEBUG_PRINTLN("Invalid body format");
        return false;
    }
    bool synced = body.indexOf("synced") != -1;
    DEBUG_PRINT("Load from S3: ");
    DEBUG_PRINTLN(synced);
    if (synced == true)
        url = BIN_ALT_URL;
    else
    {

        JsonArray assets = doc["assets"].as<JsonArray>();
        for (JsonObject asset : assets)
        {
            String name = asset["name"];
            if (name == BIN_NAME)
            {
                url = asset["browser_download_url"].as<String>();
                break;
            }
        }
    }

    DEBUG_PRINT("tag: ");
    DEBUG_PRINTLN(tag);
    DEBUG_PRINT("url: ");
    DEBUG_PRINTLN(url);
    DEBUG_PRINT("md5: ");
    DEBUG_PRINTLN(md5);
    DEBUG_PRINT("size: ");
    DEBUG_PRINTLN(fileSize);

    preferences.begin("esp", true);
    uint8_t key[32];
    preferences.getBytes("aes_key", key, sizeof(key));
    uint8_t iv[16];
    preferences.getBytes("aes_iv", iv, sizeof(iv));
    preferences.end();

    // url = "http://192.168.1.3:8000/GSMV.bin";
    DEBUG_PRINTLN("\nStart update");
    http.begin(client, url);
    // http.begin(url);
    http.setFollowRedirects(HTTPC_STRICT_FOLLOW_REDIRECTS);
    http.setFollowRedirects(HTTPC_FORCE_FOLLOW_REDIRECTS);
    http.addHeader("Accept", "application/octet-stream");
    // http.addHeader("Authorization", ENV_GIT_PAT);
    // http.addHeader("X-GitHub-Api-Version", "2022-11-28");

    DEBUG_PRINTLN(esp_get_free_heap_size());
    for (uint8_t i = 0; i < retry; i++)
    {
        httpResponseCode = http.GET();
        DEBUG_PRINT("HTTP Response code: ");
        DEBUG_PRINTLN(httpResponseCode);
        if (httpResponseCode != HTTP_CODE_OK)
            continue;

        int len = http.getSize();

        esp_aes_context ctx;
        esp_aes_init(&ctx);
        esp_aes_setkey(&ctx, key, 256);

        if (!Update.begin(fileSize, U_FLASH))
        {
            DEBUG_PRINTLN("Cannot do the update");
            return false;
        }
        Update.setMD5(md5.c_str());

        uint8_t buffer[1024];
        int bytesRead;
        size_t totalBytesWritten = 0;
        while ((bytesRead = http.getStream().readBytes((char *)buffer, sizeof(buffer))) > 0)
        {
            uint8_t decrypted[1024];
            esp_aes_crypt_cbc(&ctx, ESP_AES_DECRYPT, bytesRead, iv, buffer, decrypted);

            int bytesToWrite = bytesRead;
            if (bytesRead < sizeof(buffer))
            {
                // Remove padding
                uint8_t padding = decrypted[bytesRead - 1];
                if (padding > 0 && padding <= 16)
                {
                    bytesToWrite -= padding;
                }
            }

            if (Update.write(decrypted, bytesToWrite) != bytesToWrite)
            {
                DEBUG_PRINTLN("Update write failed");
                return false;
            }

            totalBytesWritten += bytesToWrite;
            delay(5);
        }
        DEBUG_PRINT("Total bytes written: ");
        DEBUG_PRINTLN(totalBytesWritten);
        esp_aes_free(&ctx);
        // Update.writeStream(http.getStream());

        if (Update.end())
        {
            DEBUG_PRINTLN("Successful update");
            preferences.begin("esp", false);
            preferences.putString("ver", tag);
            preferences.end();
            ESP.restart();
            return true;
        }
        else
        {
            uint8_t updateError = Update.getError();
            DEBUG_PRINTLN("Error Occurred:" + String(updateError));
#ifdef DEBUG
            Update.printError(SerialMon);
#endif
            if (updateError != UPDATE_ERROR_MD5)
                return false;
        }
    }
    return false;
}