#!/bin/bash
set -e

cd build-web/client && python3 -m http.server 8080
