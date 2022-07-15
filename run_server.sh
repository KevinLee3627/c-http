#!/bin/sh
make
sudo --preserve-env=SERVER_CERT_CHAIN_FILE_PATH,SERVER_PRIVATE_KEY_FILE_PATH bin/server 443
