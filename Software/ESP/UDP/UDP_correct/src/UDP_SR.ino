#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#define PRINT 20

const char* ssid = "Papa Smerf";
const char* password = "szybkiinternet";
unsigned int localUdpPort = 4210;  // local port to listen on
char incomingPacket[255];  // buffer for incoming packets
volatile char axis, sign, axis1, sign1;
int16_t value, value1;
uint8_t convertedValue, convertedValue1;
int actualTime, previousTime,previousTime1;
bool var = true;
WiFiUDP Udp;

void setup()
{
  Serial.begin(115200);
  //Serial.printf("Connecting to %s ", ssid);
  WiFi.begin(ssid, password);
  while (WiFi.status() != WL_CONNECTED)
  {
    delay(500);
    Serial.print(".");
  }
  Serial.println(" connected");

  Udp.begin(localUdpPort);
  Serial.printf("Now listening at IP %s, UDP port %d\n", WiFi.localIP().toString().c_str(), localUdpPort);
}


void loop()
{
  int packetSize = Udp.parsePacket();
  if (packetSize)
  {
    int len = Udp.read(incomingPacket, 255);
    if (len > 0)
    {
      incomingPacket[len] = 0;
    }
    if (var == true)
    {
      axis = incomingPacket[0];
      value = atoi(&incomingPacket[1]);
      
      if(value < 0)
      {
        sign = '-';
        convertedValue = abs(value);
      }
      else
      {
        sign = '+';
        convertedValue = value;
      }
      var = false;
      Serial.printf("%c%c%c", axis,sign,convertedValue);
    }
    else
    {
      axis1 = incomingPacket[0];
      value1 = atoi(&incomingPacket[1]);
      
      if(value1 < 0)
      {
        sign1 = '-';
        convertedValue1 = abs(value1);
      }
      else
      {
        sign1 = '+';
        convertedValue1 = value1;
      }
      var = true;
      delay(10);
      Serial.printf("%c%c%c", axis1,sign1,convertedValue1);
      Serial.flush();
    }
  }
}
