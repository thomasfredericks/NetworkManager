#ifndef __NETWORK_MANAGER_ETHERNET__
#define __NETWORK_MANAGER_ETHERNET__


#ifdef ESP32
#include <NetworkManager.h>
#include <SPI.h>
#include <Ethernet.h>
#include <EthernetBonjour.h>


#define NETMANAGER_MDNS_NAME_MAX_LENGTH 64

#ifndef LOG
#define LOG(...)
#endif


// ---------------------------------------------------
bool _NetworkManagerEthernetReturned = false;
IPAddress _NetworkManagerEthernetIp = INADDR_NONE;
void _NetworkManagerEthernetCallback(const char* name, const byte ipAddr[4]) {
  if (NULL != ipAddr) {
    _NetworkManagerEthernetIp = IPAddress(ipAddr[0], ipAddr[1], ipAddr[2], ipAddr[3]);
  } else {
    _NetworkManagerEthernetIp = INADDR_NONE;
  }
  _NetworkManagerEthernetReturned = true;
}


// ---------------------------------------------------
class NetworkManagerEthernet : public NetworkManagerBase {

protected:

  // GET FACTORY DEFINED ESP32 MAC :
  virtual void getMac() {
    esp_efuse_mac_get_default(mac);
  }

  virtual void connect() {


    // START ETHERNET LOOKING FOR DHCP
    Ethernet.begin(mac);

    while (Ethernet.localIP() == INADDR_NONE) {
      LOG("NetworkManager", "Did not find DHCP server!");
      shortDelay();
      Ethernet.begin(mac);
    }

    EthernetBonjour.begin(name);

    update();
  }

public:

  virtual IPAddress resolveName(const char* hostName) {
    EthernetBonjour.setNameResolvedCallback(_NetworkManagerEthernetCallback);
    
    
    while ( true ) {
      _NetworkManagerEthernetIp = INADDR_NONE;
      LOG("NetworkManagerEthernet", "resolving", name);
      EthernetBonjour.resolveName(hostName, 5000);

      while ( _NetworkManagerEthernetReturned == false ) {
        EthernetBonjour.run();
        yield();
      }

      _NetworkManagerEthernetReturned = false;
      if ( _NetworkManagerEthernetIp == INADDR_NONE ) {
        LOG("NetworkManagerEthernet","FAILED");
        shortDelay();
      } else {
        LOG("NetworkManagerEthernet","FOUND");
        break;
      }
       
    }

    return _NetworkManagerEthernetIp;
  }

  virtual void update() {
    EthernetBonjour.run();
  }


  virtual void announceUDPService(const char *serviceName, uint16_t servicePort ) {
    EthernetBonjour.addServiceRecord(serviceName, servicePort, MDNSServiceUDP);
  }
  virtual void announceTCPService(const char *serviceName,  uint16_t servicePort ) {
    EthernetBonjour.addServiceRecord(serviceName, servicePort, MDNSServiceTCP);
  }


};


NetworkManagerEthernet NetworkManager;
#endif
#endif