/***********************************-----( Bus Stop )-----*********************************/
/********************-----( Receiving RFID & Sending Data via GSM )-----*******************/

/*-----( Import needed libraries )-----*/
#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>
#include <SoftwareSerial.h>
/*-----( Declare Constants and Pin Numbers )-----*/
#define CE_PIN   9
#define CSN_PIN 10
#define RX_PIN 2
#define TX_PIN 3

#define NUM_OF_BUSES 25
//#define DEBUG_DAM

volatile bool done = false;

const uint64_t pipe = 0xE8E8F0F0E2LL; // Define the transmiting pipe

/*-----( Declare objects )-----*/
RF24 radio(CE_PIN, CSN_PIN); // Create a Radio
SoftwareSerial mySerial(RX_PIN, TX_PIN);  // RX, TX

/*-----( Declare Variables )-----*/
int bus_id;   //Variable holding the ID of the particular bus
int bus_id_array[NUM_OF_BUSES];   // To store several Bus IDs
int index = 1;    // To keep track of the next array index
String httpURLbegin = "at+httppara=\"URL\",\"http://daminda.16mb.com/AddingData.php?BusStopID=1&BusID=";
String httpURLend = "\"";
String httpURL;   // To store the URL to send data to the online database



void setup()    /****************** SETUP ******************/
{
  Serial.begin(9600);
  delay(2000);
  Serial.println("Nrf24L01 Receiver Starting...");
  radio.begin();
  radio.openReadingPipe(1,pipe);
  radio.startListening();
  Serial.println("GSM Communication Starting...");
  mySerial.begin(9600);  // Speed port for connection to the Arduino GSM module
  mySerial.println("AT");
  bus_id_array[0] = 0;  // To avoid the garbage value error while checking the bus_id_array[index-1]
}//--(end setup )---


void loop()     /****************** LOOP *******************/
{
  delay(5000);
  RFIDReceive();
  delay(2000);
  GSMCommunication();
}//--(end main loop )---


void RFIDReceive(){
  if ( radio.available() )
  {
    // Read the data payload until we've received everything
    done = false;
    delay(2000);
    while (!done)
    {
      // Fetch the data payload
      done = radio.read( &bus_id, sizeof(bus_id) );
      Serial.println(done);

      if (bus_id_array[index-1] != bus_id){   // To avoid storing the same bus_id in the array
        bus_id_array[index] = bus_id;
        index++;
      }
      if(bus_id_array[index-1] == 255){
        Serial.println("Kottawa - Mount Lavinia bus is going");
      }
      if( index >= NUM_OF_BUSES){
        index = NUM_OF_BUSES - 1;
      }
//      delay(1000);
    }
  }
  else
  {    
      Serial.println("No buses are going");
      delay(3000);
  }
} // End RFIDReceive Method


void GSMCommunication(){
  if(mySerial.available()){
    Serial.write(mySerial.read());
  }
  if(Serial.available()){
    mySerial.write(Serial.read());
  }
  Serial.println("Connecting...");
  mySerial.println("AT+CGATT=1");   // Attach GPRS
  delay(2000);
  readResponse();
  mySerial.println("AT+CGDCONT=1,\"IP\",\"EBB\"");    // Define a PDP(Packet Data Protocol) as contextID=1, IPConnection, APN=EBB
  delay(2000);
  readResponse();
  mySerial.println("AT+CGACT=1,1");   // Set up session parameters(activate, contextID);
  delay(2000);
  readResponse();
  mySerial.println("AT+SAPBR=3,1,\"APN\",\"EBB\"");    // Define the APN=EBB
  delay(2000);
  readResponse();
  mySerial.println("AT+SAPBR=1,1");   // Set up session parameters(activate, contextID);
  delay(2000);
  readResponse();
  
  mySerial.println("AT+HTTPINIT");     // Initiate HTTP service
  delay(2000);
  readResponse();
  mySerial.println("AT+HTTPPARA=\"CID\",1");    // Set the HTTP session
  delay(2000);
  readResponse();

    #ifdef DEBUG_DAM
      bus_id_array[index] = 320; 
      index++;
    #endif

  while (index > 1){
    httpURL = httpURLbegin + bus_id_array[--index] + httpURLend;
  
    #ifdef DEBUG_DAM
      Serial.println("Checking the httpURL");
      Serial.println(httpURL);
    #endif
  
    mySerial.println(httpURL);  // Set the HTTP URL
    delay(2000);
    readResponse();
    mySerial.println("AT+HTTPACTION=0");    // Start the session
    delay(5000);
    readResponse();        // If successful return code = *,200,*
  }
  mySerial.println("AT+HTTPREAD");      // Read the data of HTTP server
  delay(2000);
  readResponse();
  mySerial.println("AT+HTTPTERM");        // Terminate the HTTP service'
  delay(2000);
  readResponse();

  mySerial.println("AT+CGACT=0,1");   // Deactivate the PDP context - No additional data send or receive
  delay(2000);
  readResponse();
  mySerial.println("AT+CGATT=0");     // Detach GPRS
  delay(2000);
  readResponse();
  Serial.println("Disconnected");
  delay(3000);
} // End GSMCommunication Method

void readResponse(){  // A method to make easy the reading the response for AT commands from GSM module
  while (mySerial.available()){
    Serial.print((char)mySerial.read());
  }
  Serial.println("");  
} // End readResponse Method

//*********( THE END )***********
