#include <JeeLib.h> // https://github.com/jcw/jeelib

#define myNodeID    1           // RF12 node ID in the range 1-30
#define network     212         // RF12 Network group
#define freq        RF12_868MHZ // Frequency of RFM12B module

#define LED_RED    A0
#define LED_GREEN  A1
#define LED_BLUE   A2

#define LED_SAVE   7
#define LED_CLIENT 0

typedef struct {
  char    client;
  boolean save;
  char    r;    // red value
  char    g;    // green value
  char    b;    // blue value
 } leddata_t;

leddata_t transmit_data;
int counter = 0;

//########################################################################################################################

void setup() {

  rf12_initialize(myNodeID,freq,network); // Initialize RFM12 with settings defined above 

  transmit_data.client = 2;
  transmit_data.save = false;
  
  pinMode(INPUT,LED_RED);
  pinMode(INPUT,LED_GREEN);
  pinMode(INPUT,LED_BLUE);
  pinMode(INPUT,LED_SAVE);
  pinMode(INPUT,LED_CLIENT);
}

void loop() {
  
  if (counter == 0)
  {
    // Read ADC data
    transmit_data.r = (analogRead(LED_RED) >> 2);
    transmit_data.g = (analogRead(LED_GREEN) >> 2);
    transmit_data.b = (analogRead(LED_BLUE) >> 2);

    if (digitalRead(LED_CLIENT) == 1)
    {
      transmit_data.client = 1;
    }
    else
    {
      transmit_data.client = 0;
    }
  
    rfwrite(); // Send data via RF 
    transmit_data.save = false;
  }
  
  if (counter == 9)
  {
    counter = 0;
  }
  else
  {
    counter++;
  }
  
  if (digitalRead(LED_SAVE) == 0)
  {
    transmit_data.save = true;
  }
  
  delay(100);
    
}

static void rfwrite(){
  while (!rf12_canSend())
  rf12_recvDone();
  rf12_sendStart(0, &transmit_data, sizeof transmit_data); 
  rf12_sendWait(2);
}
