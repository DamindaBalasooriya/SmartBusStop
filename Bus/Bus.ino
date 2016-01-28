/***********************************-----( Bus )-----*********************************/
/****************************-----( Transmitting RFID )-----**************************/

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10

const uint64_t pipe = 0xE8E8F0F0E2LL; // Define the transmiting pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
/*-----( Declare Variables )-----*/
int bus_id = 255;   //Variable holding the ID of a particular bus

void setup()   /****** SETUP ******/
{
  Serial.begin(9600);
  radio.begin();
  radio.openWritingPipe(pipe);
}//--(end setup )---


void loop()   /****** LOOP ******/
{
  radio.write( &bus_id, sizeof(bus_id) );
  delay(5000);
  Serial.println("BusID sent");
  
}//--(end main loop )---

//*********( THE END )***********}
