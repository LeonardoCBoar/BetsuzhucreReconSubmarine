#include <VirtualWire.h>

#include "../commom/messages.hpp"

String mensagem;

const int TEST_BUTTON_PIN = 13;
const int ANALOG_X_PIN = A0;
const int ANALOG_Y_PIN = A1;
const int ANALOG_POSITIVE_MOVEMENT_THRESOLD = 256;
const int ANALOG_NEGATIVE_MOVEMENT_THRESOLD = 795;

int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(13, INPUT_PULLUP);
  vw_set_tx_pin(8);
  vw_setup(2000);

}

void loop() {
  ControlMessage control_message;
  char data[1];
  data[0] = 'b';
  if (digitalRead(13) == LOW) {
    control_message.set_command(ControlMessage::DOWN);
  }
  else if (digitalRead(14) == LOW) {
    control_message.set_command(ControlMessage::UP);
  }

  const int analog_read_x = analogRead(ANALOG_X_PIN);
  const int analog_read_y = analogRead(ANALOG_Y_PIN);

  if(analog_read_x < ANALOG_NEGATIVE_MOVEMENT_THRESOLD)
    control_message.set_command(ControlMessage::LEFT);
  else if(analog_read_x > ANALOG_POSITIVE_MOVEMENT_THRESOLD)
    control_messge.set_command(ControlMessage::RIGHT)

  if(analog_read_y < ANALOG_NEGATIVE_MOVEMENT_THRESOLD)
    control_message.set_command(ControlMessage::BACKWARD);
  else if(analog_read_y > ANALOG_POSITIVE_MOVEMENT_THRESOLD)
    control_messge.set_command(ControlMessage::FORWARD)

  send(control_message.data, 1);
  delay(500);

}

void send (char* message, const size_t size)
{
  const uint8_t aproved = vw_send( (uint8_t*) message, size);
  vw_wait_tx(); // Aguarda o envio de dados
  i+=1;
  Serial.print(i);
  Serial.println(aproved);
  //Serial.println(data);
}
