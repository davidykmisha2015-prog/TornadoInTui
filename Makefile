CXX := g++
CXXFLAGS := -std=c++17 -O2 -Wall -Wextra -Wpedantic
TARGET := tornado
SOURCE := Tornado.cpp

PREFIX ?= /usr/local
BINDIR := $(PREFIX)/bin

.PHONY: all clean run install uninstall

all: $(TARGET)

$(TARGET): $(SOURCE)
	$(CXX) $(CXXFLAGS) $(SOURCE) -o $(TARGET)

run: $(TARGET)
	./$(TARGET)

install: $(TARGET)
	install -Dm755 $(TARGET) "$(DESTDIR)$(BINDIR)/$(TARGET)"
	install -Dm644 config.example.ini \
		"$(DESTDIR)/etc/tornadointui/config.example.ini"

uninstall:
	rm -f "$(DESTDIR)$(BINDIR)/$(TARGET)"
	rm -f "$(DESTDIR)/etc/tornadointui/config.example.ini"

clean:
	rm -f $(TARGET)
