/*
   KiteLib SX127x Settings Example

   This example shows how to change all the properties of LoRa transmission.
   KiteLib currently supports the following settings:
    - pins (SPI slave select, digital IO 0, digital IO 1)
    - carrier frequency
    - bandwidth
    - spreading factor
    - coding rate
    - sync word
    - output power during transmission
*/

// include the library
#include <KiteLib.h>

// SX1278 module is in slot A on the shield
SX1278 loraSX1278 = Kite.ModuleA;

// create another instance of LoRa class using SX1272 module and user-specified pinout
// NSS pin:   6
// DIO0 pin:  4
// DIO1 pin:  5
SX1272 loraSX1272 = new Module(6, 4, 5);

// create third instance of LoRa class using SX1276 module and user-specified pinout
// we ran out of Uno digital pins, so here we use analog ones
// NSS pin:   A0
// DIO0 pin:  A1
// DIO1 pin:  A2
SX1276 loraSX1276 = new Module(A0, A1, A2);

void setup() {
  Serial.begin(9600);

  // initialize SX1278 with default settings
  Serial.print(F("[SX1278] Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   125.0 kHz
  // spreading factor:            9
  // coding rate:                 7
  // sync word:                   0x12
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  int state = loraSX1278.begin();
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // initialize the second LoRa instance with non-default settings
  // this LoRa link will have maximum range, but very low data rate
  Serial.print(F("[SX1276] Initializing ... "));
  // carrier frequency:           434.0 MHz
  // bandwidth:                   7.8 kHz
  // spreading factor:            12
  // coding rate:                 8
  // sync word:                   0x13
  // output power:                17 dBm
  // current limit:               100 mA
  // preamble length:             8 symbols
  // amplifier gain:              0 (automatic gain control)
  state = loraSX1276.begin(434.0, 7.8, 12, 8, 0x13);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // initialize the third LoRa instance with
  // non-default settings
  // this LoRa link will have high data rate, 
  // but lower range
  // NOTE: when using spreading factor 6, the total packet
  //       length has to be known in advance!
  //       you have to pass the number of expected bytes
  //       to the receive() method
  Serial.print(F("[SX1272] Initializing ... "));
  // carrier frequency:           915.0 MHz
  // bandwidth:                   500.0 kHz
  // spreading factor:            6
  // coding rate:                 5
  // sync word:                   0x14
  // output power:                2 dBm
  // current limit:               50 mA
  // preamble length:             20 symbols
  // amplifier gain:              1 (maximum gain)
  state = loraSX1272.begin(915.0, 500.0, 6, 5, 0x14, 2);
  if (state == ERR_NONE) {
    Serial.println(F("success!"));
  } else {
    Serial.print(F("failed, code "));
    Serial.println(state);
    while (true);
  }

  // you can also change the settings at runtime
  // and check if the configuration was changed successfully

  // different modules accept different parameters
  // see https://github.com/jgromes/LoRaLib/wiki/Supported-LoRa-modules

  // set carrier frequency to 433.5 MHz
  if (loraSX1278.setFrequency(433.5) == ERR_INVALID_FREQUENCY) {
    Serial.println("Selected frequency is invalid for this module!");
    while (true);
  }

  // set bandwidth to 250 kHz
  if (loraSX1278.setBandwidth(250.0) == ERR_INVALID_BANDWIDTH) {
    Serial.println("Selected bandwidth is invalid for this module!");
    while (true);
  }

  // set spreading factor to 10
  if (loraSX1278.setSpreadingFactor(10) == ERR_INVALID_SPREADING_FACTOR) {
    Serial.println("Selected spreading factor is invalid for this module!");
    while (true);
  }

  // set coding rate to 6
  if (loraSX1278.setCodingRate(6) == ERR_INVALID_CODING_RATE) {
    Serial.println("Selected coding rate is invalid for this module!");
    while (true);
  }

  // set LoRa sync word to 0x14
  // NOTE: value 0x34 is reserved for LoRaWAN networks and should not be used
  if (loraSX1278.setSyncWord(0x14) != ERR_NONE) {
    Serial.println("Unable to set sync word!");
    while (true);
  }

  // set output power to 10 dBm (accepted range is -3 - 17 dBm)
  // NOTE: 20 dBm value allows high power operation, but transmission
  //       duty cycle MUST NOT exceed 1%
  if (loraSX1278.setOutputPower(10) == ERR_INVALID_OUTPUT_POWER) {
    Serial.println("Selected output power is invalid for this module!");
    while (true);
  }

  // set over current protection limit to 80 mA (accepted range is 45 - 240 mA)
  // NOTE: set value to 0 to disable overcurrent protection
  if (loraSX1278.setCurrentLimit(80) == ERR_INVALID_CURRENT_LIMIT) {
    Serial.println("Selected current limit is invalid for this module!");
    while (true);
  }

  // set LoRa preamble length to 15 symbols (accepted range is 6 - 65535)
  if (loraSX1278.setPreambleLength(15) == ERR_INVALID_PREAMBLE_LENGTH) {
    Serial.println("Selected preamble length is invalid for this module!");
    while (true);
  }

  // set amplifier gain to 1 (accepted range is 1 - 6, where 1 is maximum gain)
  // NOTE: set value to 0 to enable autmatic gain control
  //       leave at 0 unless you know what you're doing
  if (loraSX1278.setGain(1) == ERR_INVALID_GAIN) {
    Serial.println("Selected gain is invalid for this module!");
    while (true);
  }

  Serial.println("All settings succesfully changed!");
}

void loop() {
  // nothing here
}
