#!/bin/bash
openssl sha384 -binary -out ../src/firmware/main.bin.sig ../src/firmware/main.bin
printf "%08x" `cat ../src/firmware/main.bin | wc -c` | xxd -r -p >> ../src/firmware/main.bin.sig
