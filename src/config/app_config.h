#pragma once
#include <Arduino.h>

#define DEBUG 1

// Nombre del archivo de configuración
//const char *CONFIG_FILE = "/config.json";

// Macros de Depuración
#if DEBUG
#define debugPrint(fmt, ...) Serial.printf(fmt, ##__VA_ARGS__)
#define debugPrintln(fmt, ...) Serial.printf(fmt "\n", ##__VA_ARGS__)
#else
#define debugPrint(fmt, ...)
#define debugPrintln(fmt, ...)
#endif

