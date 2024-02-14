#define REMOTE_NAME "B1387-00"

#include <NetworkManagerEthernet.h>

void setup() {

  // CONFIGURE ETHERNET HARDWARE
  SPI.begin(22, 23, 33, 19);
  Ethernet.init(19);

  // Start NetworkManager (with Ethernet or WiFi as set by the include)
  NetworkManager.begin("esp-", 3); // Start with "esp-" as a prefix, followed by the three last values of the MAC address

  // Resolve the IP of the REMOTE_NAME
  IPAddress remoteIp = NetworkManager.resolveName(REMOTE_NAME);
}

void loop() {

  NetworkManager.update();

}
