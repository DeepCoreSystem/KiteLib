#include "Module.h"

Module::Module(int cs, int int0, int int1) {
  _cs = cs;
  _tx = -1;
  _rx = -1;
  _int0 = int0;
  _int1 = int1;
}

Module::Module(int cs, int tx, int rx, int int0, int int1) {
  _cs = cs;
  _tx = tx;
  _rx = rx;
  _int0 = int0;
  _int1 = int1;
  
  ModuleSerial = new SoftwareSerial(_tx, _rx);
}

void Module::init(uint8_t interface, uint8_t gpio) {
  DEBUG_BEGIN(9600);
  DEBUG_PRINTLN();
  
  switch(interface) {
    case USE_SPI:
      pinMode(_cs, OUTPUT);
      digitalWrite(_cs, HIGH);
      SPI.begin();
      break;
    case USE_UART:
      ModuleSerial->begin(baudrate);
      break;
    case USE_I2C:
      break;
  }
  
  switch(gpio) {
    case INT_NONE:
      break;
    case INT_0:
      pinMode(_int0, INPUT);
      break;
    case INT_1:
      pinMode(_int1, INPUT);
      break;
    case INT_BOTH:
      pinMode(_int0, INPUT);
      pinMode(_int1, INPUT);
      break;
  }
}

void Module::ATemptyBuffer() {
  while(ModuleSerial->available() > 0) {
    ModuleSerial->read();
  }
}

bool Module::ATsendCommand(const char* cmd) {
  ATemptyBuffer();
  ModuleSerial->print(cmd);
  ModuleSerial->print(AtLineFeed);
  return(ATgetResponse());
}

bool Module::ATsendData(uint8_t* data, uint32_t len) {
  ATemptyBuffer();
  for(uint32_t i = 0; i < len; i++) {
    ModuleSerial->write(data[i]);
  }
  
  ModuleSerial->print(AtLineFeed);
  return(ATgetResponse());
}

bool Module::ATgetResponse() {
  String data = "";
  uint32_t start = millis(); 
  while (millis() - start < _ATtimeout) {
    while(ModuleSerial->available() > 0) {
      char c = ModuleSerial->read();
      DEBUG_PRINT(c);
      data += c;
    }
    
    if(data.indexOf("OK") != -1) {
      DEBUG_PRINTLN();
      return(true);
    } else if (data.indexOf("ERROR") != -1) {
      DEBUG_PRINTLN();
      return(false);
    }
    
  }
  DEBUG_PRINTLN();
  return(false);
}

int16_t Module::SPIgetRegValue(uint8_t reg, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(ERR_INVALID_BIT_RANGE);
  }
  
  uint8_t rawValue = SPIreadRegister(reg);
  uint8_t maskedValue = rawValue & ((0b11111111 << lsb) & (0b11111111 >> (7 - msb)));
  return(maskedValue);
}

int16_t Module::SPIsetRegValue(uint8_t reg, uint8_t value, uint8_t msb, uint8_t lsb) {
  if((msb > 7) || (lsb > 7) || (lsb > msb)) {
    return(ERR_INVALID_BIT_RANGE);
  }
  
  uint8_t currentValue = SPIreadRegister(reg);
  uint8_t mask = ~((0b11111111 << (msb + 1)) | (0b11111111 >> (8 - lsb)));
  uint8_t newValue = (currentValue & ~mask) | (value & mask);
  SPIwriteRegister(reg, newValue);
  
  // some registers need a bit of time to process the change
  // e.g. SX127X_REG_OP_MODE
  delay(20);
  
  // check if the write was successful
  uint8_t readValue = SPIreadRegister(reg);
  if(readValue != newValue) {
    DEBUG_PRINTLN();
    DEBUG_PRINT_STR("address:\t0x");
    DEBUG_PRINTLN_HEX(reg);
    DEBUG_PRINT_STR("bits:\t\t");
    DEBUG_PRINT(msb);
    DEBUG_PRINT(' ');
    DEBUG_PRINTLN(lsb);
    DEBUG_PRINT_STR("value:\t\t0b");
    DEBUG_PRINTLN_BIN(value);
    DEBUG_PRINT_STR("current:\t0b");
    DEBUG_PRINTLN_BIN(currentValue);
    DEBUG_PRINT_STR("mask:\t\t0b");
    DEBUG_PRINTLN_BIN(mask);
    DEBUG_PRINT_STR("new:\t\t0b");
    DEBUG_PRINTLN_BIN(newValue);
    DEBUG_PRINT_STR("read:\t\t0b");
    DEBUG_PRINTLN_BIN(readValue);
    DEBUG_PRINTLN();
    
    return(ERR_SPI_WRITE_FAILED);
  }
  
  return(ERR_NONE);
}

void Module::SPIreadRegisterBurst(uint8_t reg, uint8_t numBytes, uint8_t* inBytes) {
  digitalWrite(_cs, LOW);
  SPI.transfer(reg | SPI_READ);
  for(uint8_t i = 0; i < numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_cs, HIGH);
}

void Module::SPIreadRegisterBurstStr(uint8_t reg, uint8_t numBytes, char* inBytes) {
  digitalWrite(_cs, LOW);
  SPI.transfer(reg | SPI_READ);
  for(uint8_t i = 0; i < numBytes; i++) {
    inBytes[i] = SPI.transfer(reg);
  }
  digitalWrite(_cs, HIGH);
}

uint8_t Module::SPIreadRegister(uint8_t reg) {
  uint8_t inByte;
  digitalWrite(_cs, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg | SPI_READ);
  SPI.endTransaction();
  inByte = SPI.transfer(0x00);
  digitalWrite(_cs, HIGH);
  return(inByte);
}

void Module::SPIwriteRegisterBurst(uint8_t reg, uint8_t* data, uint8_t numBytes) {
  digitalWrite(_cs, LOW);
  SPI.transfer(reg | SPI_WRITE);
  for(uint8_t i = 0; i < numBytes; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_cs, HIGH);
}

void Module::SPIwriteRegisterBurstStr(uint8_t reg, const char* data, uint8_t numBytes) {
  digitalWrite(_cs, LOW);
  SPI.transfer(reg | SPI_WRITE);
  for(uint8_t i = 0; i < numBytes; i++) {
    SPI.transfer(data[i]);
  }
  digitalWrite(_cs, HIGH);
}

void Module::SPIwriteRegister(uint8_t reg, uint8_t data) {
  digitalWrite(_cs, LOW);
  SPI.beginTransaction(SPISettings(2000000, MSBFIRST, SPI_MODE0));
  SPI.transfer(reg | SPI_WRITE);
  SPI.transfer(data);
  SPI.endTransaction();
  digitalWrite(_cs, HIGH);
}
