#include <VirtualWire.h>

#include "../common/messages.hpp"

 
byte message[VW_MAX_MESSAGE_LEN];
byte msgLength = VW_MAX_MESSAGE_LEN;

const int RECEIVER_PIN = 7;
const int LED = 6;

int count = 0;
 
 
void setup()   {
	Serial.begin(9600);
	pinMode(LED, OUTPUT);

	vw_set_rx_pin(RECEIVER_PIN);
	vw_setup(2000);
	vw_rx_start();
}
 
void loop()
{
	uint8_t message[VW_MAX_MESSAGE_LEN];    
	uint8_t msgLength = VW_MAX_MESSAGE_LEN; 
 
	if (vw_get_message(message, &msgLength))
	{
		digitalWrite(LED, HIGH);
		delay(100);
		digitalWrite(LED, LOW);

		count++;
		Serial.print((String)count + " - Recebido: " );
		for (int i = 0; i < msgLength; i++)
		{
			  Serial.write(message[i]);
		}
		Serial.println();

		ControlMessage received_command = ControlMessage(message[0]);

		if(received_command.get_command(ControlMessage::FORWARD))
			Serial.printLn("FORWARD");
		if(received_command.get_command(ControlMessage::BACKWARD))
			Serial.printLn("BACKWARD");
		if(received_command.get_command(ControlMessage::LEFT))
			Serial.printLn("LEFT");
		if(received_command.get_command(ControlMessage::RIGHT))
			Serial.printLn("RIGHT");
		if(received_command.get_command(ControlMessage::DOWN))
			Serial.printLn("DOWN")
		if(received_command.get_command(ControlMessage::UP))
			Serial.printLn("UP");
	}
}
