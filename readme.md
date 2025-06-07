# GSMLink

Designed to run on a [LILYGO® T-A7670](https://lilygo.cc/products/t-sim-a7670e) board. It can also run on an ESP32 connected to any SIMCOM gsm module with proper modifications.

## Usage
- install PlatformIO on your IDE
- get TSL certificate bundle from `https://curl.se/ca/cacert.pem`
- convert the bundle to binary `python extras/gen_crt_bundle.py -i cacert.pem`
- copy bundle to `data/cert/x509_crt_bundle.bin`
- comment the code for OTA if not needed
- flash the ESP