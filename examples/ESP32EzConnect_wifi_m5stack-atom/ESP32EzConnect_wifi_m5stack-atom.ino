
#define MY_PORT 7000
#define REMOTE_PORT 8000
#define REMOTE_NAME "MSI"

#include <M5Atom.h>

CRGB embeddedPixel;

#include <ESP32EzConnect.h>

#include <WiFiUdp.h>
WiFiUDP myUdp;




#include <MicroOscUdp.h>

// THE NUMBER 1024 BETWEEN THE < > SYMBOLS  BELOW IS THE MAXIMUM NUMBER OF BYTES RESERVED FOR INCOMMING MESSAGES.
// OUTGOING MESSAGES ARE WRITTEN DIRECTLY TO THE OUTPUT AND DO NOT NEED ANY RESERVED BYTES.
// PROVIDE A POINTER TO UDP, AND THE IP AND PORT FOR OUTGOING MESSAGES.
// DO NOT FORGET THAT THE UDP CONNEXION MUST BE INITIALIZED IN SETUP() WITH THE RECEIVE PORT.
// WE ARE INITIALIZING WITH nullIp AS IT WILL BE CHANGED LATER.
MicroOscUdp<1024> myMicroOsc(&myUdp);

unsigned long myChronoStart = 0;  // VARIABLE USED TO LIMIT THE SPEED OF THE SENDING OF OSC MESSAGES


/********
  SETUP
*********/
void setup() {



  M5.begin(false, false, false);

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(&embeddedPixel, 1);
  // Animation de démarrage
  while (millis() < 5000) {
    embeddedPixel = CHSV((millis() / 5) % 255, 255, 255 - (millis() * 255 / 5000));
    FastLED.show();
    delay(50);
  }

  embeddedPixel = CRGB(127, 127, 127); FastLED.show();


  Serial.begin(115200);

  delay(100);

  embeddedPixel = CRGB(255, 255, 0); FastLED.show();
  NetworkManagerWifi.begin();

  Serial.println("Launching blocking name resolution");
  IPAddress remoteIp = NetworkManagerWifi.resolveName(REMOTE_NAME);

  if ( remoteIp == INADDR_NONE ) {
   embeddedPixel = CRGB(255, 0, 0);
   FastLED.show();
    while( true ) {};
  }
  
  Serial.print("Found ");
  Serial.print(REMOTE_NAME);
  Serial.print(" at ");
  Serial.print(remoteIp);
  Serial.println();
  myMicroOsc.setDestination(remoteIp, REMOTE_PORT);

  embeddedPixel = CRGB(0, 255, 0); FastLED.show();

  Serial.println("Starting loop()");

  /*
  if (!connect()) {
    while (1) {
      // STAY IN THIS LOOP FOREVER
      mesPixels[0] = CRGB(255, 0, 0);
      FastLED.show();
      delay(500);
      mesPixels[0] = CRGB(0, 0, 0);
      FastLED.show();
      delay(500);
    }
  }
*/
  myUdp.begin(MY_PORT);
}

/****************
  myOnOscMessageReceived is triggered when a message is received
*****************/
void myOnOscMessageReceived(MicroOscMessage& oscMessage) {

  // CHECK THE ADDRESS OF THE OSC MESSAGE
  if (oscMessage.checkOscAddress("/pixel")) {

    int red = oscMessage.nextAsInt();
    int green = oscMessage.nextAsInt();
    int blue = oscMessage.nextAsInt();
    embeddedPixel = CRGB(red, green, blue);
    FastLED.show();
  }
}

/*******
  LOOP
********/
void loop() {

  M5.update();

  NetworkManagerWifi.update();

  // TRIGGER myOnOscMessageReceived() IF AN OSC MESSAGE IS RECEIVED :
  myMicroOsc.onOscMessageReceived(myOnOscMessageReceived);

  // SEND OSC MESSAGES (EVERY 50 MILLISECONDS) :
  if (millis() - myChronoStart >= 50) {  // IF 50 MS HAVE ELLAPSED
    myChronoStart = millis();            // RESTART CHRONO

    myMicroOsc.sendInt("/millis", millis());

    // USE THE FOLLOWING METHODS TO SEND OSC MESSAGES :
    /*
      // SEND AN INT(32)
      myMicroOsc.sendInt(const char *address, int32_t i);
      // SEND A FLOAT
      myMicroOsc.sendFloat(const char *address, float f);
      // SEND A STRING
      myMicroOsc.endString(const char *address, const char *str);
      // SEND A BLOB
      myMicroOsc.sendBlob(const char *address, unsigned char *b, int32_t length);
      // SEND DOUBLE
      myMicroOsc.sendDouble(const char *address,double d);
      // SEND MIDI
      myMicroOsc.sendMidi(const char *address,unsigned char *midi);
      // SEND INT64
      myMicroOsc.sendInt64(const char *address, uint64_t h);
      // SEND A MIXED TYPE VARIABLE LENGTH MESSAGE
      myMicroOsc.sendMessage(const char *address, const char *format, ...);
    */
  }
}
