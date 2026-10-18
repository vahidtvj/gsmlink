# GSMLink

Forward incoming SMS to Telegram from an ESP32 and a SIMCOM GSM module.

Some services only send SMS codes to local numbers, or limit
international SMS. Keep a local SIM in a GSMLink device and get every
message in Telegram, wherever you are.

## Features

- Forwards incoming SMS (including Unicode and multipart) to a Telegram chat
- TLS-secured communication with the Telegram Bot API
- Encrypted OTA firmware updates
- LTE fallback when [Wi-Fi is unavailable]

## Hardware

Designed for the [LILYGO T-A7670](https://lilygo.cc/products/t-sim-a7670e).
It can also run on an ESP32 connected to any SIMCOM GSM module with
proper modifications.

## Built with

PlatformIO, [TinyGSM](https://github.com/vshymanskyy/TinyGSM),
[TinySMS](https://github.com/vahidtvj/TinySMS)

## Setup

1. Install PlatformIO in your IDE
2. Get the TLS certificate bundle from https://curl.se/ca/cacert.pem
3. Convert it: `python extras/gen_crt_bundle.py -i cacert.pem`
4. Copy the result to `data/cert/x509_crt_bundle.bin`
5. [Set your Telegram bot token and chat ID in ___]
6. Comment out the OTA code if you don't need it
7. Flash the ESP

## License

[MIT](https://github.com/vahidtvj/gsmlink/blob/master/LICENCE) © [vahidtvj](https://github.com/vahidtvj)
