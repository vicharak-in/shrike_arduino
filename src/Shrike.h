/*
 * ShrikeFlash.h
 *
 * Usage:
 *   #include "ShrikeFlash.h"
 *   ShrikeFlash fpga;
 *   fpga.begin();
 *   fpga.flash("/led_blink.bin");
 */
#ifndef SHRIKE_FLASH_H
#define SHRIKE_FLASH_H

#include <Arduino.h>
#include <SPI.h>
#include <LittleFS.h>

// ── Default pin assignments ──────────────────────────────────────────────────
#if defined(ARDUINO_ARCH_RP2040)
  #define SHRIKE_DEFAULT_EN_PIN    13
  #define SHRIKE_DEFAULT_PWR_PIN   12
  #define SHRIKE_DEFAULT_SS_PIN     1
  #define SHRIKE_DEFAULT_SCK_PIN    2
  #define SHRIKE_DEFAULT_MOSI_PIN   3
  #define SHRIKE_DEFAULT_MISO_PIN   0
#elif defined(ARDUINO_ARCH_ESP32)
  #define SHRIKE_DEFAULT_EN_PIN     9
  #define SHRIKE_DEFAULT_PWR_PIN    8
  #define SHRIKE_DEFAULT_SS_PIN    10
  #define SHRIKE_DEFAULT_SCK_PIN   12
  #define SHRIKE_DEFAULT_MOSI_PIN  11
  #define SHRIKE_DEFAULT_MISO_PIN  13
#else
  #error "Unsupported architecture. Only RP2040 and ESP32 are supported."
#endif
// ─────────────────────────────────────────────────────────────────────────────

class ShrikeFlash {
private:
  uint8_t _en_pin;
  uint8_t _pwr_pin;
  uint8_t _ss_pin;
  uint8_t _sck_pin;
  uint8_t _mosi_pin;
  uint8_t _miso_pin;
  uint32_t _spi_speed;
  SPISettings* _spi_settings;
  unsigned long _last_flash_time;
  float _transfer_rate;

#if defined(ARDUINO_ARCH_ESP32)
  SPIClass _hspi;
#endif

public:
  // Constructor — defaults are arch-specific (see macros above)
  ShrikeFlash(uint8_t en_pin   = SHRIKE_DEFAULT_EN_PIN,
              uint8_t pwr_pin  = SHRIKE_DEFAULT_PWR_PIN,
              uint8_t ss_pin   = SHRIKE_DEFAULT_SS_PIN,
              uint8_t sck_pin  = SHRIKE_DEFAULT_SCK_PIN,
              uint8_t mosi_pin = SHRIKE_DEFAULT_MOSI_PIN,
              uint8_t miso_pin = SHRIKE_DEFAULT_MISO_PIN);

  // Initialize the library (call in setup())
  bool begin(uint32_t spi_speed = 1600000);

  // Flash FPGA with bitstream file
  bool flash(const char* filename, uint32_t word_size = 46408);

  // Reset FPGA
  void reset();

  // Timing & Stats
  unsigned long getLastFlashTime();
  float getTransferRate();
  void printStats();

  // File Management
  bool fileExists(const char* filename);
  size_t getFileSize(const char* filename);
  void listFiles();
  bool deleteFile(const char* filename);
  void printFSInfo();
};

#endif // SHRIKE_FLASH_H