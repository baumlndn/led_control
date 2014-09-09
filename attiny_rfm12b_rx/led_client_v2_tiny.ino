#include <ws2811_8.h>
#include <EEPROM.h>

#include <JeeLib.h> // https://github.com/jcw/jeelib

#define MYNODE      8            //node ID of the receiever
#define freq        RF12_868MHZ     //frequency
#define group       212            //network group
#define LEDDATA_PIN 0
#define CLIENTID    0
#define LEDAMOUNT   12    

typedef struct {
  char client;
  boolean save;
  char r;    // red value
  char g;    // green value
  char b;    // blue value
 } leddata;
 
leddata rx;

int nodeID;    //node ID of tx, extracted from RF datapacket. Not transmitted as part of structure
 
WS2811_8 ledstrip; 
 
void(* resetFunc) (void) = 0; //declare reset function @ address 0

void softReset()
{
  // 1st 0x0F/0x00 -> delay 2s
  // 2nd 0xF0 -> 0x0F
  
  switch (EEPROM.read(99))
  {
    case 0x00:
    case 0x0F:
      EEPROM.write(99,0xF0);
      delay(2000);
      resetFunc();
      break;
    case 0xF0:
      EEPROM.write(99,0x0F);
    default:
      break;
  }
}

void setup () 
{
  pinMode(LEDDATA_PIN,OUTPUT);
  digitalWrite(LEDDATA_PIN,LOW);
//  Serial.begin(9600);
// Send data to LED strip

  for (int i = 0; i < LEDAMOUNT; i++)
  {
    ledstrip.set(i,
      EEPROM.read(100),
      EEPROM.read(101),
      EEPROM.read(102));
  }
  
  ledstrip.send();
  
//  softReset();
  
  rf12_initialize(MYNODE, freq,group); // Initialise the RFM12B

}

void loop() 
{
  if (rf12_recvDone() && rf12_crc == 0 && (rf12_hdr & RF12_HDR_CTL) == 0) 
  {
    nodeID = rf12_hdr & 0x1F;  // get node ID
    rx = *(leddata*) rf12_data;
  
    if (rx.client == CLIENTID)
    {
      if (RF12_WANTS_ACK) 
      {                  // Send ACK if requested
        rf12_sendStart(RF12_ACK_REPLY, 0, 0);
      }
  
      // Send data to LED strip
      for (int i = 0; i < LEDAMOUNT; i++)
      {
        ledstrip.set(i,rx.r,rx.g,rx.b);
      }

      if (rx.save == true)
      {
        EEPROM.write(100,rx.r);
        EEPROM.write(101,rx.g);
        EEPROM.write(102,rx.b);
      
        for (int i = 0; i < LEDAMOUNT; i++)
        {
          ledstrip.set(i,0,0,0);
        } 
       
        ledstrip.send();
      
      delay(1000);
      resetFunc(); 
      }

    ledstrip.send();                                
    }
  }
}
