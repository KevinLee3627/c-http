#!/bin/bash
#This should not be hardcoded
cd /home/kevin/c-http
make
sudo --preserve-env=SERVER_CERT_CHAIN_FILE_PATH,SERVER_PRIVATE_KEY_FILE_PATH bin/server 443
