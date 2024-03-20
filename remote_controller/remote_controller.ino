#include <RH_ASK.h>
#include <SPI.h>

#include "messages.hpp"


const int TEST_BUTTON_PIN = 13;
const int JOYSTICK_5V_PIN = 2;
const int TX_PIN = 8;

const int ANALOG_X_PIN = A0;
const int ANALOG_Y_PIN = A1;
const int ANALOG_LOW_THRESOLD = 128;
const int ANALOG_HIGH_THRESOLD = 895;

RH_ASK driver{2000, 0, TX_PIN};

int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(TEST_BUTTON_PIN, INPUT_PULLUP);
  pinMode(JOYSTICK_5V_PIN, OUTPUT);
  digitalWrite(JOYSTICK_5V_PIN, HIGH);
  if(!driver.init())
  {
    Serial.println("RF failed");
  }

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

  Serial.print("Y: ");
  Serial.println(analog_read_y);
  Serial.print("X: ");
  Serial.println(analog_read_x);
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


  control_message.print_binary();
  send(control_message.data, 1);
  delay(1500);

}

void send (uint8_t* message, const size_t size)
{
  char abc = 123;
  const uint8_t aproved = driver.send((uint8_t)&abc, 1);
  i+=1;
  Serial.print(i);
  Serial.print(" - Approved: ");
  Serial.println(aproved);
}
