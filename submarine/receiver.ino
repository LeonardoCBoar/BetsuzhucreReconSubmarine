#include <VirtualWire.h>

 
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
	}
}
