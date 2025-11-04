/*
 * ShrikeFlash.cpp
 * Implementation file for ShrikeFlash library
 */

#include "Shrike.h"

// Constructor
ShrikeFlash::ShrikeFlash(uint8_t en_pin, uint8_t pwr_pin, 
                         uint8_t ss_pin, uint8_t sck_pin, 
                         uint8_t mosi_pin, uint8_t miso_pin) {
  _en_pin = en_pin;
  _pwr_pin = pwr_pin;
  _ss_pin = ss_pin;
  _sck_pin = sck_pin;
  _mosi_pin = mosi_pin;
  _miso_pin = miso_pin;
  _spi_settings = nullptr;
  _last_flash_time = 0;
  _transfer_rate = 0.0;
}

// Initialize
bool ShrikeFlash::begin(uint32_t spi_speed) {
  _spi_speed = spi_speed;

  // Initialize pins
  pinMode(_en_pin, OUTPUT);
  pinMode(_pwr_pin, OUTPUT);
  pinMode(_ss_pin, OUTPUT);

  digitalWrite(_ss_pin, HIGH);
  digitalWrite(_en_pin, LOW);
  digitalWrite(_pwr_pin, LOW);

  // Initialize SPI with custom pins
  SPI.setSCK(_sck_pin);
  SPI.setTX(_mosi_pin);
  SPI.setRX(_miso_pin);
  SPI.setCS(_ss_pin);
  SPI.begin();

  // Create SPI settings
  _spi_settings = new SPISettings(_spi_speed, MSBFIRST, SPI_MODE0);

  // Initialize LittleFS
  if (!LittleFS.begin()) {
    Serial.println("[ShrikeFlash] ERROR: LittleFS mount failed!");
    return false;
  }

  Serial.println("[ShrikeFlash] Initialized successfully");
  return true;
}

// Flash FPGA
bool ShrikeFlash::flash(const char* filename, uint32_t word_size) {
  reset();

  Serial.println("[ShrikeFlash] Starting FPGA flash...");
  Serial.print("[ShrikeFlash] Flashing: ");
  Serial.println(filename);

  if (!LittleFS.exists(filename)) {
    Serial.println("[ShrikeFlash] ERROR: File not found");
    return false;
  }

  File file = LittleFS.open(filename, "r");
  if (!file) {
    Serial.println("[ShrikeFlash] ERROR: Failed to open file");
    return false;
  }

  size_t fileSize = file.size();
  Serial.print("[ShrikeFlash] File size: ");
  Serial.print(fileSize);
  Serial.println(" bytes");

  delay(500);

  digitalWrite(_ss_pin, HIGH);
  delayMicroseconds(2000);
  digitalWrite(_ss_pin, LOW);

  SPI.beginTransaction(*_spi_settings);

  unsigned long startTime = millis();

  uint8_t* buffer = new uint8_t[word_size];
  size_t bytesRead;
  size_t totalSent = 0;

  while ((bytesRead = file.read(buffer, word_size)) > 0) {
    SPI.transfer(buffer, bytesRead);
    totalSent += bytesRead;
  }

  _last_flash_time = millis() - startTime;
  _transfer_rate = (_last_flash_time > 0) ? (totalSent * 1000.0 / _last_flash_time / 1024.0) : 0;

  delete[] buffer;
  SPI.endTransaction();
  digitalWrite(_ss_pin, HIGH);

  file.close();

  delay(100);

  Serial.println("[ShrikeFlash] FPGA programming done.");
  Serial.print("[ShrikeFlash] Time: ");
  Serial.print(_last_flash_time);
  Serial.print(" ms, Rate: ");
  Serial.print(_transfer_rate, 2);
  Serial.println(" KB/s");

  return true;
}

// Reset FPGA
void ShrikeFlash::reset() {
  digitalWrite(_ss_pin, LOW);
  digitalWrite(_en_pin, LOW);
  digitalWrite(_pwr_pin, LOW);
  delay(100);
  digitalWrite(_en_pin, HIGH);
  digitalWrite(_pwr_pin, HIGH);
  delay(100);
}

// Timing & Stats
unsigned long ShrikeFlash::getLastFlashTime() {
  return _last_flash_time;
}

float ShrikeFlash::getTransferRate() {
  return _transfer_rate;
}

void ShrikeFlash::printStats() {
  Serial.println("\n[ShrikeFlash] === Flash Statistics ===");
  Serial.print("Time taken: ");
  Serial.print(_last_flash_time);
  Serial.println(" ms");
  Serial.print("Transfer rate: ");
  Serial.print(_transfer_rate, 2);
  Serial.println(" KB/s");
  Serial.println("===========================\n");
}

// File Management
bool ShrikeFlash::fileExists(const char* filename) {
  return LittleFS.exists(filename);
}

size_t ShrikeFlash::getFileSize(const char* filename) {
  if (!LittleFS.exists(filename)) {
    return 0;
  }
  File file = LittleFS.open(filename, "r");
  if (!file) {
    return 0;
  }
  size_t size = file.size();
  file.close();
  return size;
}

void ShrikeFlash::listFiles() {
  Serial.println("\n[ShrikeFlash] === Files in LittleFS ===");
  File root = LittleFS.open("/", "r");
  if (!root) {
    Serial.println("[ShrikeFlash] ERROR: Failed to open root directory");
    return;
  }

  File file = root.openNextFile();
  int count = 0;
  while (file) {
    Serial.print("  ");
    Serial.print(file.name());
    Serial.print(" - ");
    Serial.print(file.size());
    Serial.println(" bytes");
    file.close();
    file = root.openNextFile();
    count++;
  }
  root.close();

  if (count == 0) {
    Serial.println("  (No files found)");
  }
  Serial.println("===========================\n");
}

bool ShrikeFlash::deleteFile(const char* filename) {
  if (!LittleFS.exists(filename)) {
    Serial.print("[ShrikeFlash] ERROR: File not found: ");
    Serial.println(filename);
    return false;
  }

  if (LittleFS.remove(filename)) {
    Serial.print("[ShrikeFlash] File deleted: ");
    Serial.println(filename);
    return true;
  } else {
    Serial.print("[ShrikeFlash] ERROR: Failed to delete: ");
    Serial.println(filename);
    return false;
  }
}

void ShrikeFlash::printFSInfo() {
  Serial.println("\n[ShrikeFlash] === Filesystem Info ===");

  FSInfo fs_info;
  LittleFS.info(fs_info);

  Serial.print("Total space: ");
  Serial.print(fs_info.totalBytes / 1024);
  Serial.println(" KB");
  Serial.print("Used space: ");
  Serial.print(fs_info.usedBytes / 1024);
  Serial.println(" KB");
  Serial.print("Free space: ");
  Serial.print((fs_info.totalBytes - fs_info.usedBytes) / 1024);
  Serial.println(" KB");
  Serial.println("===========================\n");
}
