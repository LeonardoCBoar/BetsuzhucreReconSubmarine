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
const int DEPTH_ENCODER_TOLERANCE = 25;

int count = 0;
RH_ASK driver{2000, RX_PIN, 0};

int target_depth_encoder_position = 0;
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

uint64_t previous_time = 0;
float previous_error = 0;
float integral_error = 0;
 
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

    analogWrite(SPEED_PIN, 200);
    if(received_command.get_command(ControlMessage::FORWARD))
    {
      //Serial.println("RX: FORWARD");
      target_depth_encoder_position = DEPTH_ENCODER_LIMIT;
    }
    else if(received_command.get_command(ControlMessage::BACKWARD))
    {
      //Serial.println("RX: BACKWARD");
      target_depth_encoder_position = -DEPTH_ENCODER_LIMIT;
    }
      else
    {
      //Serial.println("RX: STOP");
      target_depth_encoder_position = 0;
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

  const uint64_t current_time;
  uint64_t delta = previous_time - current_time;
  if(delta == 0)
    delta = 1;
  previous_time = current_time;

  int depth_position;
  int current_depth;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    current_depth = current_depth_encoder_position;
  }

  static float kp = 1;
  static float kd = 0.5;

  const int depth_error = target_depth_encoder_position - current_depth;
  previous_error = depth_error;
  const float derivative_error = (depth_error - previous_error) / ((float)delta);

   float control_signal = kp * depth_error + kd * derivative_error;
  if(abs(depth_error) < DEPTH_ENCODER_TOLERANCE)
    control_signal = 0;

    int power = abs(control_signal);
    if( power > 250 )
        power = 250;
    if(power < 195)
        power = 195;


  Serial.print("Encoder_Target:");
  Serial.print(target_depth_encoder_position);
  Serial.print(",");
  Serial.print("Encoder_Real:");
  Serial.print(current_depth_encoder_position);
  Serial.print(",");
  Serial.print("Error:");
  Serial.print(depth_error);
  Serial.print(",");
  Serial.print("Control_Signal:");
  Serial.println(power);

  if(control_signal == 0)
  {
      //Serial.println("Stopping encoder");
      digitalWrite(FORWARD_PIN, LOW);
      digitalWrite(BACKWARD_PIN, LOW);
      return;
  }
  else if(control_signal>0)
  {
    //Serial.println("Moving encoder forward");
    digitalWrite(FORWARD_PIN, HIGH);
    digitalWrite(BACKWARD_PIN, LOW);
    analogWrite(SPEED_PIN, power);

  }
  else
  {
    //Serial.println("Moving encoder backward");
    digitalWrite(FORWARD_PIN, LOW);
    digitalWrite(BACKWARD_PIN, HIGH);
    analogWrite(SPEED_PIN, power);
  }
}

