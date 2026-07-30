#!/bin/sh

set -e

echo "[*] Installing TornadoInTui..."

mkdir -p "$HOME/.local/bin"

TMP_DIR=$(mktemp -d)

echo "[*] Downloading source..."

curl -L \
https://raw.githubusercontent.com/davidykmisha2015-prog/TornadoInTui/main/Tornado.cpp \
-o "$TMP_DIR/Tornado.cpp"

echo "[*] Compiling..."

c++ -O2 "$TMP_DIR/Tornado.cpp" -o "$HOME/.local/bin/tornado"

chmod +x "$HOME/.local/bin/tornado"

rm -rf "$TMP_DIR"

echo
echo "Installed!"
echo "Run:"
echo "tornado"
