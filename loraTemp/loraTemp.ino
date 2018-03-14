#include <Sodaq_RN2483.h>
#include <ArduinoJson.h>

#define debugSerial SerialUSB
#define loraSerial Serial2

#define NIBBLE_TO_HEX_CHAR(i) ((i <= 9) ? ('0' + i) : ('A' - 10 + i))
#define HIGH_NIBBLE(i) ((i >> 4) & 0x0F)
#define LOW_NIBBLE(i) (i & 0x0F)

//Use OTAA, set to false to use ABP
bool OTAA = false;

char TempLabel[3]="T:";
char ConductivityLabel[3]="C:";
char PayloadToSend[20];

// ABP
// USE YOUR OWN KEYS!
const uint8_t devAddr[4] =
{
    //0xF4, 0x89, 0x52, 0xAA
    0xA7, 0xde, 0xe4, 0xd2
};

// USE YOUR OWN KEYS!
const uint8_t appSKey[16] =
{
  0xe2, 0x66, 0x10, 0x41, 0x15, 0x58, 0x96, 0x25, 0x4a, 0x7a, 0xc9, 0xf5, 0x27, 0x1f, 0x7d, 0x78
  //0x30, 0x1C, 0xF0, 0xCA, 0xB5, 0xD4, 0xFE, 0xC1, 0xE6, 0x9C, 0xCD, 0x2F, 0xEE, 0x25, 0x17, 0xFB
};

// USE YOUR OWN KEYS!
const uint8_t nwkSKey[16] =
{
  0x6f, 0xe6, 0xd3, 0x18, 0xa9, 0x91, 0xf8, 0x1e, 0xde, 0x9a, 0x40, 0x3a, 0xe6, 0x88, 0x62, 0x5f
  //0xDD, 0x7C, 0xC5, 0x5c, 0x40, 0xA5, 0x38, 0xD1, 0xA0, 0xAB, 0xE3, 0xE2, 0xA9, 0x85, 0xE6, 0x82
};

// OTAA
// With using the GetHWEUI() function the HWEUI will be used
static uint8_t DevEUI[8]
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AppEUI[8] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

const uint8_t AppKey[16] =
{
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00
};

void setup()
{
  delay(1000);
  
  while ((!debugSerial) && (millis() < 10000)){
  // Wait 10 seconds for debugSerial to open
}

  debugSerial.println("Start");

  // Start streams
  debugSerial.begin(57600);
  loraSerial.begin(LoRaBee.getDefaultBaudRate());

  LoRaBee.setDiag(debugSerial); // to use debug remove //DEBUG inside library
  LoRaBee.init(loraSerial, LORA_RESET);

  //Use the Hardware EUI
  getHWEUI();

  // Print the Hardware EUI
  debugSerial.print("LoRa HWEUI: ");
  for (uint8_t i = 0; i < sizeof(DevEUI); i++) {
    debugSerial.print((char)NIBBLE_TO_HEX_CHAR(HIGH_NIBBLE(DevEUI[i])));
    debugSerial.print((char)NIBBLE_TO_HEX_CHAR(LOW_NIBBLE(DevEUI[i])));
  }
  debugSerial.println();  
  
  setupLoRa();
}

void setupLoRa(){
  if(!OTAA){
    // ABP
    setupLoRaABP();
  } else {
    //OTAA
    setupLoRaOTAA();
  }
  // Uncomment this line to for the RN2903 with the Actility Network
  // For OTAA update the DEFAULT_FSB in the library
  // LoRaBee.setFsbChannels(1);

  LoRaBee.setSpreadingFactor(9);
}

void setupLoRaABP(){  
  if (LoRaBee.initABP(loraSerial, devAddr, appSKey, nwkSKey, true))
  {
    debugSerial.println("Communication to LoRaBEE successful.");
  }
  else
  {
    debugSerial.println("Communication to LoRaBEE failed!");
  }
}

void setupLoRaOTAA(){
  
  if (LoRaBee.initOTA(loraSerial, DevEUI, AppEUI, AppKey, true))
  {
    debugSerial.println("Network connection successful.");
  }
  else
  {
    debugSerial.println("Network connection failed!");
  }
}

void loop()
{ 
   //StaticJsonBuffer<148> jsonBuffer;
   DynamicJsonBuffer  jsonBuffer(200);
   JsonObject& json = jsonBuffer.createObject();
   String StrToSend;
      
   String reading = getTemperature();
   debugSerial.println(reading);
   int analog_value = analogRead(A0);
   debugSerial.println(analog_value);

   json["temp"] = reading;
   json["conduct"] = analog_value;
/*
   size_t printTo(char* buffer, size_t size) const;
   size_t printTo(char buffer[size]) const;
   size_t printTo(Print &) const;
   size_t printTo(String &) const;
   size_t printTo(std::string &) const;
*/
   //StrToSend = String(TempLabel)+reading+","+String(ConductivityLabel)+String(analog_value,DEC);
   json.printTo(StrToSend);
   debugSerial.print("Send: ");
   debugSerial.println(StrToSend);
   // Send the payload
   switch (LoRaBee.send(1, (uint8_t*)StrToSend.c_str(), StrToSend.length()))
    {
    case NoError:
      debugSerial.println("Successful transmission.");
      break;
    case NoResponse:
      debugSerial.println("There was no response from the device.");
      break;
    case Timeout:
      debugSerial.println("Connection timed-out. Check your serial connection to the device! Sleeping for 20sec.");
      delay(20000);
      break;
    case PayloadSizeError:
      debugSerial.println("The size of the payload is greater than allowed. Transmission failed!");
      break;
    case InternalError:
      debugSerial.println("Oh No! This shouldn't happen. Something is really wrong! The program will reset the RN module.");
      setupLoRa();
      break;
    case Busy:
      debugSerial.println("The device is busy. Sleeping for 10 extra seconds.");
      delay(10000);
      break;
    case NetworkFatalError:
      debugSerial.println("There is a non-recoverable error with the network connection. The program will reset the RN module.");
      setupLoRa();
      break;
    case NotConnected:
      debugSerial.println("The device is not connected to the network. The program will reset the RN module.");
      setupLoRa();
      break;
    case NoAcknowledgment:
      debugSerial.println("There was no acknowledgment sent back!");
      break;
    default:
      break;
    }
    // Delay between readings
    // 60 000 = 1 minute
    delay(10000); 
}

String getTemperature()
{
  //10mV per C, 0C is 500mV
  float mVolts = (float)analogRead(TEMP_SENSOR) * 3300.0 / 1023.0;
  float temp = (mVolts - 500.0) / 10.0;
  
  return String(temp);
}

/**
* Gets and stores the LoRa module's HWEUI/
*/
static void getHWEUI()
{
  uint8_t len = LoRaBee.getHWEUI(DevEUI, sizeof(DevEUI));
}

