
// This example requieres MicroOsc version 0.1.7 or higher
#include <MicroOscUdp.h>

// MicroLog (https://github.com/thomasfredericks/MicroLog) is used for logging
#include <MicroLog.h>

#define MY_PORT 7000
#define REMOTE_PORT 8000
#define REMOTE_NAME "B1387-00"

#include <M5Atom.h>

CRGB embeddedPixel;

#include <NetworkManagerEthernet.h>

EthernetUDP myUdp;


MicroOscUdp<1024> myMicroOsc(&myUdp);

unsigned long myChronoStart = 0;  // VARIABLE USED TO LIMIT THE SPEED OF THE SENDING OF OSC MESSAGES


/********
  SETUP
*********/
void setup() {

  M5.begin(false, false, false);

  Serial.begin(115200);

  FastLED.addLeds<WS2812, DATA_PIN, GRB>(&embeddedPixel, 1);
  // Animation de démarrage
  while (millis() < 5000) {
    embeddedPixel = CHSV((millis() / 5) % 255, 255, 255 - (millis() * 255 / 5000));
    FastLED.show();
    delay(50);
  }

  // Show red pixel as the device is getting ready to connect
  embeddedPixel = CRGB(255, 255, 0);
  FastLED.show();

  // CONFIGURE ATOM POE ETHERNET
  SPI.begin(22, 23, 33, 19);
  Ethernet.init(19);

  // Start NetworkManager (with Ethernet or WiFi as set by the include)
  LOG("Starting NetworkManager");
  // This will loop until the DHCP server is found
  NetworkManager.begin("esp-", 3); // Start with "esp-" as a prefix, followed by the three last values of the MAC address

  // Resolve the IP of the REMOTE_NAME
  LOG("Attempting to resolve remote name");
  IPAddress remoteIp = NetworkManager.resolveName(REMOTE_NAME);

  LOG("Found", REMOTE_NAME, "at", remoteIp);

  LOG("Setting OSC destination to", remoteIp, REMOTE_PORT);
  myMicroOsc.setDestination(remoteIp, REMOTE_PORT);

  embeddedPixel = CRGB(0, 255, 0);
  FastLED.show();

  LOG("Starting UDP");
  myUdp.begin(MY_PORT);

  LOG("Annoucing OSC UDP on port", MY_PORT);
  NetworkManager.announceUDPService("OSC", MY_PORT);

  LOG("Starting loop()");
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

  NetworkManager.update();

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
