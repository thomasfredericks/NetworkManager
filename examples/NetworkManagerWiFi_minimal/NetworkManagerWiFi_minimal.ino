
#define REMOTE_NAME "MSI"

#include <NetworkManagerWiFi.h>

void setup() {

  // Start with "esp-" as a prefix, followed by the three last values of the MAC address
  NetworkManager.begin("esp-", 3); 

  IPAddress remoteIp = NetworkManager.resolveName(REMOTE_NAME);

}

void loop() {

  NetworkManager.update();

}
