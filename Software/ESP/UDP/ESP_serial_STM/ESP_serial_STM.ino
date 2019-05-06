#include <ESP8266WiFi.h>

void setup()
{
  Serial.begin(115200);
  Serial.println();
}

uint8_t counter = 0;
uint8_t axis = 'y';
uint8_t sign = '+';
void loop()
{
  Serial.write(axis);
  Serial.write(sign);
  Serial.write(counter++);
  delay(500);
}
