#!/bin/sh

set -e

echo "[*] Installing TornadoInTui..."

BIN_DIR="$HOME/.local/bin"
CONFIG_DIR="$HOME/.config/tornadointui"
TMP_DIR=$(mktemp -d)

mkdir -p "$BIN_DIR"
mkdir -p "$CONFIG_DIR"

echo "[*] Downloading source..."

curl -fsSL \
https://raw.githubusercontent.com/davidykmisha2015-prog/TornadoInTui/main/src/Tornado.cpp \
-o "$TMP_DIR/Tornado.cpp"

echo "[*] Downloading default configuration..."

curl -fsSL \
https://raw.githubusercontent.com/davidykmisha2015-prog/TornadoInTui/main/config.example.ini \
-o "$TMP_DIR/config.example.ini"

echo "[*] Compiling..."

c++ -O2 "$TMP_DIR/Tornado.cpp" \
-o "$BIN_DIR/tornado"

chmod +x "$BIN_DIR/tornado"

if [ ! -f "$CONFIG_DIR/config.ini" ]; then
    cp "$TMP_DIR/config.example.ini" \
       "$CONFIG_DIR/config.ini"

    echo "[+] Created:"
    echo "    $CONFIG_DIR/config.ini"
else
    echo "[*] Existing configuration found."
    echo "    Keeping current config."
fi

cp "$TMP_DIR/config.example.ini" \
   "$CONFIG_DIR/config.example.ini"

rm -rf "$TMP_DIR"

echo
echo "[✓] TornadoInTui installed successfully!"
echo
echo "Run:"
echo "  tornado"
echo
echo "Configuration:"
echo "  $CONFIG_DIR/config.ini"
