#define OLED_I2C_ADDRESS 0x3C /// default for HELTEC OLED
#define BME_I2C_ADDRESS 0x76 // default for the BME/BMP280
#define i2c_sda_pin 4 // sharing i2c with the HALTEC_ESP32_OLED
#define i2c_scl_pin 15 // same
#define serial1_rx_pin 27 //12 will interfere with flash
#define serial1_tx_pin 14
#define wire_Frequency 300000   /// could be in the range of 100000 to 400000
#define rst_pin 16  /// <== delete this line if your OLED does not have/support OLED reset pin
#define HARDWARE_LED 25 /// HELTEC LED/// <== delete this line if your OLED does not have/support OLED reset pin
#define TIMEZONE_OFFSET -8
#define uS_TO_S_FACTOR 1000000  /* Conversion factor for micro seconds to seconds */
#define TIME_TO_SLEEP 30 /* Time ESP32 will go to sleep (in seconds) */
// LoRa configs
#define BAND 915E6 //you can set band here directly,e.g. 868E6,915E6
#define LORA_SS_PIN     18
#define LORA_RESET_PIN  14
#define LORA_DI0_PIN 26
#define PA_OUTPUT_PA_BOOST_PIN  1
#define PA_OUTPUT_RFO_PIN       0

#include <Arduino.h>
#include <SPI.h>
#include <LoRa.h>
#include "SSD1306Ascii.h"
#include "SSD1306AsciiWire.h"

// ESP32 + OLED PRINTING LIKE Serial.print but in OLED ///
SSD1306AsciiWire oled;

void onReceive(int packetSize) {
  // received a packet
  oled.print("Received packet '");

  // read packet
  for (int i = 0; i < packetSize; i++) {
    oled.print((char)LoRa.read());
  }

  // print RSSI of packet
  oled.print("' with RSSI ");
  oled.println(LoRa.packetRssi());
}

void setup() {
  Serial.begin(9600);
  while (!Serial);
  /// IT IS VERY IMPORTANT TO GIVE YOUR VALUES IN THE Wire.begin(...) bellow Wire.begin(sda_pin, scl_pin, wire_Frequency); form 100000 to 400000
  Wire.begin(i2c_sda_pin, i2c_scl_pin, wire_Frequency); /// wire_Frequency: form 100000 to 400000

  #ifdef rst_pin
    pinMode(rst_pin, OUTPUT); /// THIS IS THE RESET PIN OF HELTEC OLED ... ///
    digitalWrite(rst_pin, LOW);    // set GPIO16 low to reset OLED
    delay(50);
    digitalWrite(rst_pin, HIGH); // while OLED is running, must set GPIO16 in high
  #endif

  oled.begin(&Adafruit128x64, OLED_I2C_ADDRESS); /// in this demo sketch the selected resolution is 128x64
  ///oled.setFont(System5x7); /// you can change this font but always choose monospaced font for "defined" characters length per line...
  oled.setFont(Adafruit5x7);
  oled.setScrollMode(SCROLL_MODE_OFF);


  LoRa.setPins(LORA_SS_PIN, LORA_RESET_PIN, LORA_DI0_PIN);
  if (!LoRa.begin(BAND)) {
    oled.println("Starting LoRa failed!");
    while (1);
  }

  // register the receive callback
  LoRa.onReceive(onReceive);

  oled.println("Callback Registered");
  // put the radio into receive mode
  LoRa.receive();
}

void loop() {
  // do nothing
}
