#!/bin/bash

# Generate a 256-bit (32 bytes) key
key=$(openssl rand -hex 32)
# Generate a 128-bit (16 bytes) IV
iv=$(openssl rand -hex 16)

echo "key=$key"
echo "iv=$iv"

# Format the key and IV as C arrays
formatted_key=$(echo $key | sed 's/\(..\)/0x\1, /g' | sed 's/, $//')
formatted_iv=$(echo $iv | sed 's/\(..\)/0x\1, /g' | sed 's/, $//')

# Print the formatted key and IV
echo "uint8_t key[32] = {$formatted_key};"
echo "uint8_t iv[16] = {$formatted_iv};"