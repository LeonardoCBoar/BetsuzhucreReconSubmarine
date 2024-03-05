#include <VirtualWire.h>

String mensagem;

int i = 0;

void setup() {
  Serial.begin(9600);
  pinMode(13, INPUT_PULLUP);
  vw_set_tx_pin(8);
  vw_setup(2000);

}

void loop() {
  char data[1];
  data[0] = 'b';
  if (digitalRead(13) == LOW) {
    send(data, 1);
    //delay(1000);
    
  }

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
