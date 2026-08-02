# TornadoInTui

<img width="1220" height="1212" alt="image" src="https://github.com/user-attachments/assets/5069f450-1abf-4438-a3b1-036b48196b29" />

TornadoInTui is a lightweight C++ terminal application that renders an animated tornado using ANSI escape sequences.

## Features

- 🌪️ Multiple tornado types
- 🎨 Custom RGB colors
- ⚙️ Configurable tornado shape
- 📄 Configuration file support
- 🛠️ Simple Makefile
- 🚀 Lightweight and fast
- 🖥️ Works in most ANSI-compatible terminals

## Requirements

- Linux
- C++ compiler (g++ or clang++)
- make
- curl

## Install

```bash
curl -fsSL https://raw.githubusercontent.com/davidykmisha2015-prog/TornadoInTui/main/install.sh | sh
```

## Build

```bash
make
```

## Run

```bash
tornado
```

## Configuration

TornadoInTui automatically loads the configuration from:

```text
~/.config/tornadointui/config.ini
```

Generate the default configuration:

```bash
mkdir -p ~/.config/tornadointui
cp config.example.ini ~/.config/tornadointui/config.ini
```

Example:

```ini
[tornado]
type = custom
top_width = 0.24
bottom_width = 0.05
curve = 0.80
sway = 1.00
speed = 1.00
breathing = 0.004
color = 124,199,214
shades = ░▒▓█
```

## Makefile

Available commands:

```bash
make          # Build
make run      # Build and run
make clean    # Remove binary
sudo make install
```

## License

MIT License

---

Made with ❤️ for terminal enthusiasts.

GoodUsing :3
