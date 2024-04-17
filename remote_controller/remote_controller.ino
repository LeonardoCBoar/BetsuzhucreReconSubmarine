#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 32 // OLED display height, in pixels

#include "messages.hpp"


#define TEST_BUTTON_PIN 13
#define JOYSTICK_5V_PIN  2
#define RX_PIN  7
#define TX_PIN  8

#define ANALOG_X_PIN A0
#define ANALOG_Y_PIN  A1
#define ANALOG_LOW_THRESOLD  128
#define ANALOG_HIGH_THRESOLD  895

#define RF_BUFFER_SIZE 60

RH_ASK driver{2000, RX_PIN, TX_PIN};
//RH_ASK rx_driver{2000, RX_PIN, 0};

int tx_count = 0;

#define OLED_RESET -1
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

void setup() {
  Serial.begin(9600);
  pinMode(TEST_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_5V_PIN, OUTPUT);
  digitalWrite(JOYSTICK_5V_PIN, HIGH);
  if(!driver.init())
  {
    Serial.println("RF failed");
  }

  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { 
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  display.display();
  delay(2000);

  display.clearDisplay();

}

void loop() {
  ControlMessage control_message;
  char data[1];
  data[0] = 'b';
//  if (digitalRead(13) == LOW) {
//    control_message.set_command(ControlMessage::DOWN);
//  }
//  else if (digitalRead(14) == LOW) {
//    control_message.set_command(ControlMessage::UP);
//  }

  const int analog_read_x = analogRead(ANALOG_X_PIN);
  const int analog_read_y = analogRead(ANALOG_Y_PIN);

  //Serial.print("Y: ");
  //Serial.println(analog_read_y);
  //Serial.print("X: ");
  //Serial.println(analog_read_x);
  if(analog_read_x < ANALOG_LOW_THRESOLD)
  {
    Serial.println("FORWARD");
    control_message.set_command(ControlMessage::FORWARD);
  }
  else if(analog_read_x > ANALOG_HIGH_THRESOLD)
  {
    Serial.println("BACKWARD");
    control_message.set_command(ControlMessage::BACKWARD);   
  }

  if(analog_read_y < ANALOG_LOW_THRESOLD)
  {
    Serial.println("RIGHT");
    control_message.set_command(ControlMessage::RIGHT);   
  }
  else if(analog_read_y > ANALOG_HIGH_THRESOLD)
  {
    Serial.println("LEFT");
    control_message.set_command(ControlMessage::LEFT);       
  }


  //control_message.print_binary();
  handle_rf_rx();
  send(control_message.data, 1);
  delay(100);

}

void handle_rf_rx()
{
  uint8_t message[RF_BUFFER_SIZE];
  uint8_t message_size;
	if (driver.recv(message, &message_size))
	{
    display.clearDisplay();
    for(int byte = 0; byte < message_size; byte++)
    {
      for(int8_t bit = 7; bit >= 0; bit--)
      {
        uint8_t pixel_value = (message[byte] & ( 1 << bit)) != 0;
        uint8_t pixel_x = (byte * 8 + bit) % SCREEN_WIDTH;
        uint8_t pixel_y = (byte * 8 + bit) / SCREEN_WIDTH;
        display.drawPixel(pixel_x, pixel_y, pixel_value);
        Serial.print(pixel_value);
      }
      Serial.print(",");
    }
    display.display();
    Serial.println("");
  }

}

void send (uint8_t* message, const size_t size)
{
  const uint8_t aproved = driver.send(message, 1);
  tx_count += 1;
  // Serial.print(tx_count);
  // Serial.print(" - Approved: ");
  // Serial.println(aproved);
}
