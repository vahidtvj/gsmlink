#include "main.h"

void setBaud(uint32_t baud)
{
    modem.setBaud(baud);
    SerialAT.end();
    delay(1000);
    SerialAT.begin(baud, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);
}

bool wakeModem(uint8_t retry = 3)
{
    for (int i = 0; i < retry; i++)
    {
        modem.sleepEnable(false); // Attempt to wake the modem
        delay(500);               // Give it time to wake up

        if (modem.testAT(100))
        {
            return true;
        }

        DEBUG_PRINTLN("Modem not awake, retrying...");
    }
    return false;
}

void setupDevice()
{
#ifdef LILYGO_T_A7670
    pinMode(BOARD_POWERON_PIN, OUTPUT);
    digitalWrite(BOARD_POWERON_PIN, HIGH);

    // Set modem reset
    pinMode(MODEM_RESET_PIN, OUTPUT);
    digitalWrite(MODEM_RESET_PIN, !MODEM_RESET_LEVEL);
    // Turn on modem
    pinMode(BOARD_PWRKEY_PIN, OUTPUT);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);
    delay(100);
    digitalWrite(BOARD_PWRKEY_PIN, HIGH);
    delay(1000);
    digitalWrite(BOARD_PWRKEY_PIN, LOW);

    // Set modem baud
    SerialAT.begin(115200, SERIAL_8N1, MODEM_RX_PIN, MODEM_TX_PIN);

    DEBUG_PRINTLN("Start modem...");
    delay(3000);
    TinyGsmAutoBaud(SerialAT);

    // Restart takes quite some time
    // To skip it, call init() instead of restart()
    DEBUG_PRINTLN("Initializing modem...");
    if (!modem.init())
    {
        DEBUG_PRINTLN("Failed to restart modem, delaying 10s and retrying");
        return;
    }
    setBaud(9600);

    if (!modem.setNetworkMode(MODEM_NETWORK_AUTO))
    {
        DEBUG_PRINTLN("Set network mode failed!");
    }
    String mode = modem.getNetworkModes();
    DEBUG_PRINT("Current network mode : ");
    DEBUG_PRINTLN(mode);

    DEBUG_PRINTLN("Waiting for network...");
    if (!modem.waitForNetwork())
    {
        DEBUG_PRINTLN(" fail");
    }
    DEBUG_PRINTLN(" success");
#endif
}