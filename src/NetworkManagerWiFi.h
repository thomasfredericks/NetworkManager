#ifndef __NETWORK_MANAGER_WIFI__
#define __NETWORK_MANAGER_WIFI__

#include <NetworkManager.h>
#include <WiFi.h>
#include <ESPmDNS.h>
// https://github.com/tzapu/WiFiManager
#include <WiFiManager.h>

#ifdef ESP32
class NetworkManagerWiFi : public NetworkManagerBase {
  WiFiManager wifiManager;

public:
  NetworkManagerWiFi() {
#ifdef LOG_ACTIVATED
    wifiManager.setDebugOutput(true);
#else
    wifiManager.setDebugOutput(false);
#endif
  }


protected:
  // GET FACTORY DEFINED ESP32 MAC :
  virtual void getMac() {
    esp_efuse_mac_get_default(mac);
  }
  virtual void connect() {


    LOG("NetworkManager", "Starting WiFi Manager with name", name);
    WiFi.mode(WIFI_STA);  // explicitly set mode, esp defaults to STA+AP

    //reset settings - wipe credentials for testing
    //wm.resetSettings();
    LOG("NetworkManager", "Trying to connect WiFi");
    wifiManager.setEnableConfigPortal(true);
    wifiManager.setConfigPortalBlocking(false);
    wifiManager.setCaptivePortalEnable(true);
    bool isConnected = wifiManager.autoConnect(name);

    while ( isConnected == false ) isConnected = wifiManager.process();

    LOG("NetworkManager", "Connected to network" );
    LOG("NetworkManager", "Starting MDNS", name, "with IP", WiFi.localIP() );
    MDNS.begin((const char*)name);

  }
public:

  virtual IPAddress  resolveName(const char* hostName) {
    return MDNS.queryHost(hostName);
  }

  virtual void update() {
    wifiManager.process();
  }

  virtual void announceUDPService(const char *name, uint16_t port)  {
    MDNS.addService(name, "udp", port);
  }

  virtual void announceTCPService(const char *name, uint16_t port)  {
    MDNS.addService(name, "tcp", port);
  }

};
NetworkManagerWiFi NetworkManager;
#endif


#endif