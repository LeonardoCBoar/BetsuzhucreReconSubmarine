#include <RH_ASK.h>
#include <SPI.h>
#include <Wire.h>


#include "messages.hpp"


#define THRUST_BUTTON_PIN 0
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

void setup() {
  Serial.begin(9600);
  pinMode(THRUST_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_5V_PIN, OUTPUT);
  digitalWrite(JOYSTICK_5V_PIN, HIGH);
  if(!driver.init())
  {
    Serial.println("RF failed");
  }

}

void loop() {
  ControlMessage control_message;

 if (digitalRead(THRUST_BUTTON_PIN) == LOW) 
 {
   control_message.set_command(ControlMessage::FORWARD);
 }

  const int analog_read_x = analogRead(ANALOG_X_PIN);
  const int analog_read_y = analogRead(ANALOG_Y_PIN);

  //Serial.print("Y: ");
  //Serial.println(analog_read_y);
  //Serial.print("X: ");
  //Serial.println(analog_read_x);
  if(analog_read_x < ANALOG_LOW_THRESOLD)
  {
    Serial.println("UP");
    control_message.set_command(ControlMessage::UP);
  }
  else if(analog_read_x > ANALOG_HIGH_THRESOLD)
  {
    Serial.println("DOWN");
    control_message.set_command(ControlMessage::DOWN);   
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
  send(control_message.data, 1);
  delay(100);

}

void send (uint8_t* message, const size_t size)
{
  const uint8_t aproved = driver.send(message, 1);
  tx_count += 1;
}
