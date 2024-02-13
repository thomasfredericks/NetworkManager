
#ifndef __NETWORK_MANAGER__
#define __NETWORK_MANAGER__



#define NETMANAGER_MDNS_NAME_MAX_LENGTH 64

#ifndef __MICRO_LOG__
  #define LOG(...)
#endif


class NetworkManagerBase {
private:
  
protected:
  char name[NETMANAGER_MDNS_NAME_MAX_LENGTH]; 
  uint8_t mac[6];
  virtual void connect()=0;
  virtual void getMac()=0;
  void shortDelay() {
    randomSeed(micros());
    delay(1000 + random(1000));
  }
public:

  void begin(char * newname, uint8_t appendMacValues=0) {
    LOG("NetworkManager", "Getting MAC");
    getMac();
    
    appendMacValues = constrain(appendMacValues,0,6);
    size_t nameLength = min(strlen(newname), (size_t)(NETMANAGER_MDNS_NAME_MAX_LENGTH - 1 - (appendMacValues*2)) );
    // copy the prefix
    strncpy(this->name, newname, nameLength);
    char * appendPointer = this->name + nameLength;
    for ( int i =0 ; i < appendMacValues; i++ ) {
		
      sprintf(appendPointer , "%2x", mac[i+(6-appendMacValues)] );
      appendPointer += 2;
    }
  LOG("NetworkManager", "Name:", this->name);
    connect();
  }

  virtual IPAddress resolveName(const char* hostName) = 0;
  bool ipIsValid(IPAddress ip) {
    return ip != INADDR_NONE;
  }
  virtual void update() = 0;
  const char * getName() {
    return name;
  }

  virtual void announceUDPService(const char *name, uint16_t port) = 0;
  virtual void announceTCPService(const char *name, uint16_t port) = 0;
};

#endif


