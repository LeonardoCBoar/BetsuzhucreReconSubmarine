#include <RH_ASK.h>
#include <SPI.h> 

#include "messages.hpp"
#include "motor_controller.hpp"

 
const int MESSAGE_BUFFER_SIZE = 40;

const int RX_PIN = 7;
const int LED = 13;

int count = 0;
RH_ASK driver{2000, RX_PIN, 0};
 
 
void setup()   {
	Serial.begin(9600);
	pinMode(LED, OUTPUT);

	if(!driver.init())
	{
		Serial.println("RF failed");
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
		Serial.print((String)count + " - Recebido: " );

		ControlMessage received_command = ControlMessage(message[0]);
    	received_command.print_binary();

		if(received_command.get_command(ControlMessage::FORWARD))
			Serial.println("RX: FORWARD");
		if(received_command.get_command(ControlMessage::BACKWARD))
			Serial.println("RX: BACKWARD");
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
