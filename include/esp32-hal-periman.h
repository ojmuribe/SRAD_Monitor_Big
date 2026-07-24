// Stub vacio intencional.
//
// Arduino_GFX_Library.h incluye Arduino_ESP32SPI.h (el databus por SPI),
// que a su vez incluye "esp32-hal-periman.h" -- un header que solo existe
// en el core 3.x de arduino-esp32. Nosotros usamos el core 2.x (necesario
// para que funcione Arduino_ESP32RGBPanel, la clase del panel RGB de la
// ESP32-8048S070C) y NUNCA instanciamos Arduino_ESP32SPI, asi que este
// stub vacio es seguro: solo evita el error de compilacion "file not
// found", sin que se necesite ninguna funcionalidad real de dentro.
//
// Funciona porque el proyecto ya tiene "-I include" en build_flags, y el
// #include con comillas dobles busca aqui antes que en la carpeta real
// de la libreria.
