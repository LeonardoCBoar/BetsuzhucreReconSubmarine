#include <RH_ASK.h>
#include <SPI.h> 
#include <util/atomic.h>

#include "messages.hpp"
#include "motor_controller.hpp"

 
const int MESSAGE_BUFFER_SIZE = 40;

const int RX_PIN = 7;
const int LED = 13;

const int FORWARD_PIN = 3;
const int BACKWARD_PIN = 2;
const int SPEED_PIN = 4;
const int DEPTH_ENCODER_A_PIN = 18;
const int DEPTH_ENCODER_B_PIN = 19;

const int DEPTH_ENCODER_LIMIT = 15;
const int DEPTH_MOTOR_SPEED = 200;

MotorController depth_motor_controller{FORWARD_PIN, BACKWARD_PIN, DEPTH_MOTOR_SPEED};

RH_ASK driver{2000, RX_PIN, 0};
uint8_t message[MESSAGE_BUFFER_SIZE];    
uint8_t msg_len;
unsigned int msg_count = 0;

int target_depth_encoder_position = 0;
volatile int current_depth_encoder_position = 0;
 
 
void setup()   {
	Serial.begin(9600);
	pinMode(LED, OUTPUT);

	if(!driver.init())
	{
		Serial.println("RF failed");
	}

   attachInterrupt(digitalPinToInterrupt(DEPTH_ENCODER_A_PIN), on_depth_encoder_interrupt, RISING);
}

void loop()
{
  handle_rf_recv();
  handle_motors();
}


void on_depth_encoder_interrupt()
{
  const int b_input = digitalRead(DEPTH_ENCODER_B_PIN);
  if(b_input > 0)
  {
    current_depth_encoder_position++;
  }
  else
  {
    current_depth_encoder_position--;
  }
  
}

void handle_rf_recv()
{

	if (driver.recv(message, &msg_len))
	{
		digitalWrite(LED, HIGH);
		delay(100);
		digitalWrite(LED, LOW);

		msg_count++;
		Serial.print((String)msg_count + " - Recebido: " );

		ControlMessage received_command = ControlMessage(message[0]);
    	received_command.print_binary();

    analogWrite(SPEED_PIN, 200);
		if(received_command.get_command(ControlMessage::FORWARD))
    {
      Serial.println("RX: FORWARD");
      target_depth_encoder_position = DEPTH_ENCODER_LIMIT;
    }
		else if(received_command.get_command(ControlMessage::BACKWARD))
    {
      Serial.println("RX: BACKWARD");
      target_depth_encoder_position = -DEPTH_ENCODER_LIMIT;
    }
      else
    {
      Serial.println("RX: STOP");
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

}

void handle_motors()
{
  Serial.print("Encoder position:" );
  Serial.println(current_depth_encoder_position);

  int depth_position;
  ATOMIC_BLOCK(ATOMIC_RESTORESTATE) {
    depth_position = current_depth_encoder_position;
  }

  if(target_depth_encoder_position > depth_position)
  {
    Serial.println("Moving encoder forward");
    depth_motor_controller.set_reversed(false);
    depth_motor_controller.set_enabled(true);
  }
  else if(target_depth_encoder_position < depth_position)
  {
    Serial.println("Moving encoder backward");
    depth_motor_controller.set_reversed(true);
    depth_motor_controller.set_enabled(true);
  }
  else
  {
    depth_motor_controller.set_enabled(false);
  }
}
