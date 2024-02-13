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
    // NAME
    makeName(dnsName, "net-", myMac);

    // START ETHERNET LOOKING FOR DHCP
    Ethernet.begin(myMac);

    while (Ethernet.localIP() == nullIp) {
      LOG("NetworkManager", "Did not find DHCP server!");
      shortDelay();
      Ethernet.begin(myMac);
    }

    EthernetBonjour.begin(dnsName);

    update();
  }

public:

  virtual IPAddress resolveName(const char* hostName) {
    EthernetBonjour.setNameResolvedCallback(_NetworkManagerEthernetCallback);
    
    
    while ( true ) {
      _NetworkManagerEthernetIp = INADDR_NONE;
      LOG("NetworkManagerEthernet:resolveName resolving", name);
      EthernetBonjour.resolveName(const char* name, 5000);

      while ( _NetworkManagerEthernetReturned == false ) {
        EthernetBonjour.run();
        yield();
      }

      _NetworkManagerEthernetReturned = false;
      if ( _NetworkManagerEthernetIp == INADDR_NONE ) {
        LOG("FAILED");
        shortDelay();
      } else {
        LOG("FOUND");
        break;
      }
       
    }

    return _NetworkManagerEthernetIp;
  }

  virtual void update() {
    EthernetBonjour.run();
  }

  virtual void addService(const char *service, const char *protocol, uint16_t port)  {
    MDNS.addService(service, protocol, port);
  }

  virtual void announceUDPService(const char *name, uint16_t port ) {
    EthernetBonjour.addServiceRecord(name, port, MDNSServiceUDP);
  }
  virtual void announceTCPService(const char *name,  uint16_t port ) {
    EthernetBonjour.addServiceRecord(name, port, MDNSServiceTCP);
  }


};


NetworkManagerEthernet NetworkManager;
#endif
#endif