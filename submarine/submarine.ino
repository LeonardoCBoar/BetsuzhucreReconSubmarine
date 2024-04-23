#include <RH_ASK.h>
#include <SPI.h> 
#include <util/atomic.h>

#include "messages.hpp"
#include "motor_controller.hpp"

 
const int MESSAGE_BUFFER_SIZE = 40;

const int RX_PIN = 7;
const int LED = 13;

const int FORWARD_PIN = 2;
const int BACKWARD_PIN = 3;
const int SPEED_PIN = 4;
const int DEPTH_ENCODER_A_PIN = 18;
const int DEPTH_ENCODER_B_PIN = 19;

const int DEPTH_ENCODER_LIMIT = 1000;
const int DEPTH_MOTOR_SPEED = 230;

int count = 0;
RH_ASK driver{2000, RX_PIN, 0};

volatile int current_depth_encoder_position = 0;
 
void setup()   {
  Serial.begin(9600);
  pinMode(LED, OUTPUT);

  if(!driver.init())
  {
    Serial.println("RF failed");
  }

   pinMode(FORWARD_PIN, OUTPUT);
   pinMode(BACKWARD_PIN, OUTPUT);
   pinMode(SPEED_PIN, OUTPUT);

   pinMode(DEPTH_ENCODER_A_PIN, INPUT);
   pinMode(DEPTH_ENCODER_B_PIN, INPUT);
   attachInterrupt(digitalPinToInterrupt(DEPTH_ENCODER_A_PIN), on_depth_encoder_interruptA, CHANGE);
   attachInterrupt(digitalPinToInterrupt(DEPTH_ENCODER_B_PIN), on_depth_encoder_interruptB, CHANGE);
}

//TODO: study about bitRead;
void on_depth_encoder_interruptA()
{
  const int a_input = digitalRead(DEPTH_ENCODER_A_PIN);
  const int b_input = digitalRead(DEPTH_ENCODER_B_PIN);
  if(a_input != b_input)
  {
    current_depth_encoder_position++;
  }
  else
  {
    current_depth_encoder_position--;
  }
}

void on_depth_encoder_interruptB()
{
  const int a_input = digitalRead(DEPTH_ENCODER_A_PIN);
  const int b_input = digitalRead(DEPTH_ENCODER_B_PIN);
  if(a_input == b_input)
  {
    current_depth_encoder_position++;
  }
  else
  {
    current_depth_encoder_position--;
  }
}

void loop()
{
  uint8_t message[MESSAGE_BUFFER_SIZE];    
  uint8_t msgLength;
 
  if (driver.recv(message, &msgLength))
  {
    digitalWrite(LED, HIGH);
    delay(100);
    digitalWrite(LED, LOW);

    count++;
    //Serial.print((String)count + " - Recebido: " );

    ControlMessage received_command = ControlMessage(message[0]);
    //received_command.print_binary();

    analogWrite(SPEED_PIN, DEPTH_MOTOR_SPEED);
    if(received_command.get_command(ControlMessage::FORWARD))
    {
        //Serial.println("RX: FORWARD");
        if(current_depth_encoder_position < DEPTH_ENCODER_LIMIT)
        {
            digitalWrite(FORWARD_PIN, HIGH);
            digitalWrite(BACKWARD_PIN, LOW);
        }
    }
    else if(received_command.get_command(ControlMessage::BACKWARD))
    {
        //Serial.println("RX: BACKWARD");
        if(current_depth_encoder_position >= -DEPTH_ENCODER_LIMIT)
        {
            digitalWrite(FORWARD_PIN, LOW);
            digitalWrite(BACKWARD_PIN, HIGH);
            analogWrite(SPEED_PIN, power);

        }
    }
    else
    {
        digitalWrite(FORWARD_PIN, LOW);
        digitalWrite(BACKWARD_PIN, LOW);
    }
    if(received_command.get_command(ControlMessage::LEFT))
      Serial.println("RX: LEFT");
    if(received_command.get_command(ControlMessage::RIGHT))
      Serial.println("RX: RIGHT");
    if(received_command.get_command(ControlMessage::DOWN))
      Serial.println("RX: DOWN");
    if(received_command.get_command(ControlMessage::UP))
      Serial.println("RX: UP");
  }
}

