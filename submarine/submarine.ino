#include <RH_ASK.h>
#include <SPI.h> 
#include <util/atomic.h>

#include "messages.hpp"
#include "pins.hpp"

 
const int MESSAGE_BUFFER_SIZE = 40;

const int DEPTH_ENCODER_LIMIT = 2300;

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
   pinMode(THRUST_SPEED_PIN, OUTPUT);
   pinMode(THRUST_BACKWARD_PIN, OUTPUT);
   pinMode(THRUST_FORWARD_PIN, OUTPUT);

   pinMode(TURN_SPEED_PIN, OUTPUT);
   pinMode(TURN_RIGHT_PIN, OUTPUT);
   pinMode(TURN_LEFT_PIN, OUTPUT);

   pinMode(DEPTH_FORWARD_PIN, OUTPUT);
   pinMode(DEPTH_BACKWARD_PIN, OUTPUT);
   pinMode(DEPTH_SPEED_PIN, OUTPUT);

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
    //Serial.println(current_depth_encoder_position);
    //received_command.print_binary();

    analogWrite(DEPTH_SPEED_PIN, DEPTH_MOTOR_SPEED);
    analogWrite(THRUST_SPEED_PIN, THRUST_MOTOR_SPEED);
    analogWrite(TURN_SPEED_PIN, TURN_MOTOR_SPEED);
    if(received_command.get_command(ControlMessage::UP))
    {
        Serial.println("RX: UP");
        digitalWrite(DEPTH_FORWARD_PIN, HIGH);
        digitalWrite(DEPTH_BACKWARD_PIN, LOW);
    }
    else if(received_command.get_command(ControlMessage::DOWN) && current_depth_encoder_position > -DEPTH_ENCODER_LIMIT )
    {
        Serial.println("RX: DOWN");
        digitalWrite(DEPTH_FORWARD_PIN, LOW);
        digitalWrite(DEPTH_BACKWARD_PIN, HIGH);
    }
    else
    {
        digitalWrite(DEPTH_FORWARD_PIN, HIGH);
        digitalWrite(DEPTH_BACKWARD_PIN, HIGH);
    }

    if(received_command.get_command(ControlMessage::LEFT))
    {
      Serial.println("RX: LEFT");
      digitalWrite(TURN_RIGHT_PIN, HIGH);
      digitalWrite(TURN_LEFT_PIN, LOW);
    }
    else if(received_command.get_command(ControlMessage::RIGHT))
    {
      Serial.println("RX: RIGHT");
      digitalWrite(TURN_RIGHT_PIN, LOW);
      digitalWrite(TURN_LEFT_PIN, HIGH);
    }
    else
    {
      Serial.println("RX: STOP");
      digitalWrite(TURN_RIGHT_PIN, LOW);
      digitalWrite(TURN_LEFT_PIN, LOW);
    }
    
    if(received_command.get_command(ControlMessage::FORWARD))
    {
      Serial.println("RX: FORWARD");
      digitalWrite(THRUST_FORWARD_PIN, HIGH);
      digitalWrite(THRUST_BACKWARD_PIN, LOW);
    }
    else 
    {
      digitalWrite(THRUST_FORWARD_PIN, LOW);
      digitalWrite(THRUST_BACKWARD_PIN, LOW);
    }

    if(received_command.get_command(ControlMessage::BACKWARD))
    {
      Serial.println("RX: ENCODER RESET");
      current_depth_encoder_position = 0;
    }
  }
}
