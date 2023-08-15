#include <algorithm>
#include <cstdio>
#include <utility>
#include <vector>

#include <QNEthernet.h>

using namespace qindesign::network;

constexpr uint32_t kDHCPTimeout = 15'000;  // 15 seconds
constexpr uint32_t kLinkTimeout = 5'000;  // 5 seconds
constexpr uint16_t kServerPort = 80;
constexpr uint32_t kClientTimeout = 5'000;  // 5 seconds
constexpr uint32_t kShutdownTimeout = 30'000;  // 30 seconds
// Set the static IP to something other than INADDR_NONE (all zeros)
// to not use DHCP. The values here are just examples.
IPAddress staticIP{0, 0, 0, 0};//{192, 168, 1, 101};
IPAddress subnetMask{255, 255, 255, 0};
IPAddress gateway{192, 168, 1, 1};

#include "pinConfig.h"
#include "MCP48FEB28.h"
#include <Arduino_CRC32.h>
Arduino_CRC32 crc32;
#include <USBHost_t36.h>

USBHost myusb;
USBSerial userial(myusb);

#define SERIAL_FRAME_BUFFER_SIZE 176

union SERIAL_FRAME{
  struct{
    uint16_t time;
    uint16_t control;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  volatile uint8_t data[SERIAL_FRAME_BUFFER_SIZE];
};

SERIAL_FRAME rx, tx;

MCP48FEB28 *dac_0;
MCP48FEB28 *dac_1;

#define BUFFER_SIZE 176
union FRAME{
  struct{
    uint16_t time;
    uint16_t control;
    float temperature[17];
    uint16_t led_fan;
    uint16_t chamber_fan;
    uint16_t target_current[16];
    uint16_t current[16];
    uint16_t gate[16];
    uint32_t crc;
  }values;
  volatile uint8_t data[BUFFER_SIZE];
};

// Keeps track of state for a single client.
struct ClientState {
  ClientState(EthernetClient client)
      : client(std::move(client)) {}

  EthernetClient client;
  bool closed = false;

  // For timeouts.
  uint32_t lastRead = millis();  // Mark creation time

  // For half closed connections, after "Connection: close" was sent
  // and closeOutput() was called
  uint32_t closedTime = 0;    // When the output was shut down
  bool outputClosed = false;  // Whether the output was shut down

  // Parsing state
  FRAME rx, tx;
};

// --------------------------------------------------------------------------
//  Program State
// --------------------------------------------------------------------------

// Keeps track of what and where belong to whom.
std::vector<ClientState> clients;

// The server.
EthernetServer server{kServerPort};

bool interlock = false;
bool over_temp[7] = {false,false,false,false,false,false,false};
int32_t current_raw[16];
int32_t target_current[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
int32_t gate_sp[16] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint8_t temp_pins[17] = { TEMP_LED0,TEMP_LED1,TEMP_LED2,TEMP_LED3,
                          TEMP_LED4,TEMP_LED5,TEMP_LED6,TEMP_LED7,
                          TEMP_LED8,TEMP_LED9,TEMP_LED10,TEMP_LED11,
                          TEMP_LED12,TEMP_LED13,TEMP_LED14,TEMP_LED15,
                          TEMP_DRIVER };
const float temp_poly[4] = {-1.52720929e-06,  1.49737064e-03, -6.06179414e-01,  1.37842069e+02};
int32_t temp_raw[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
float temp[17] = {0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0};
uint32_t iteration = 0;

float calcTemp(int val, const float *p){
  return p[0]*val*val*val+p[1]*val*val+p[2]*val+p[3];
}


void setup() {
  dac_0 = new MCP48FEB28(CS0,LATCH0);
  dac_1 = new MCP48FEB28(CS1,LATCH1);
  dac_0->init();
  dac_1->init();

  for(int i=0;i<8;i++){
    dac_0->write(i,0);
    dac_1->write(i,0);
  }
  myusb.begin();
  userial.begin(4000000);
  pinMode(LED_FAN,OUTPUT);
  pinMode(CHAMBER_FAN,OUTPUT);
  // analogWrite(LED_FAN,100);
  // analogWrite(CHAMBER_FAN,200);

  pinMode(LED_SEL0,OUTPUT);
  pinMode(LED_SEL1,OUTPUT);

  pinMode(LED_ENABLE, INPUT_PULLUP);

  pinMode(LED_LATCH, OUTPUT);  
  digitalWrite(LED_LATCH, true);
  
  pinMode(LED_DIAG_ENABLE_A, OUTPUT);
  pinMode(LED_DIAG_ENABLE_B, OUTPUT);
  pinMode(LED_DIAG_ENABLE_C, OUTPUT);
  pinMode(LED_DIAG_ENABLE_D, OUTPUT);
  pinMode(LED_DIAG_ENABLE_E, OUTPUT);
  pinMode(LED_DIAG_ENABLE_F, OUTPUT);

  digitalWrite(LED_DIAG_ENABLE_A, false);
  digitalWrite(LED_DIAG_ENABLE_B, false);
  digitalWrite(LED_DIAG_ENABLE_C, false);
  digitalWrite(LED_DIAG_ENABLE_D, false);
  digitalWrite(LED_DIAG_ENABLE_E, false);
  digitalWrite(LED_DIAG_ENABLE_F, false);

  pinMode(LED_SEL0, OUTPUT);
  pinMode(LED_SEL1, OUTPUT);
  digitalWrite(LED_SEL0, false);
  digitalWrite(LED_SEL1, false);

  uint8_t mac[6];
  Ethernet.macAddress(mac);  // This is informative; it retrieves, not sets
  
  // Listen for link changes
  Ethernet.onLinkState([](bool state) {
    
  });

  // Listen for address changes
  Ethernet.onAddressChanged([]() {
    IPAddress ip = Ethernet.localIP();
    bool hasIP = (ip != INADDR_NONE);
    if (hasIP) {
      ip = Ethernet.subnetMask();
      ip = Ethernet.gatewayIP();
      ip = Ethernet.dnsServerIP();
    }
  });

  if (initEthernet()) {
    // Start the server
    server.begin();
  }
}

bool initEthernet() {
  // DHCP
  if (staticIP == INADDR_NONE) {
    printf("Starting Ethernet with DHCP...\r\n");
    if (!Ethernet.begin()) {
      printf("Failed to start Ethernet\r\n");
      return false;
    }

    // We can choose not to wait and rely on the listener to tell us
    // when an address has been assigned
    if (kDHCPTimeout > 0) {
      printf("Waiting for IP address...\r\n");
      if (!Ethernet.waitForLocalIP(kDHCPTimeout)) {
        printf("No IP address yet\r\n");
        // We may still get an address later, after the timeout,
        // so continue instead of returning
      }
    }
  } else {
    // Static IP
    printf("Starting Ethernet with static IP...\r\n");
    if (!Ethernet.begin(staticIP, subnetMask, gateway)) {
      printf("Failed to start Ethernet\r\n");
      return false;
    }

    // When setting a static IP, the address is changed immediately,
    // but the link may not be up; optionally wait for the link here
    if (kLinkTimeout > 0) {
      printf("Waiting for link...\r\n");
      if (!Ethernet.waitForLink(kLinkTimeout)) {
        printf("No link yet\r\n");
        // We may still see a link later, after the timeout, so
        // continue instead of returning
      }
    }
  }

  return true;
}

void processClientData(ClientState &state) {
  int bytes_received = 0;
  bool first = true;
  uint32_t t0;
  while (true) {
    int avail = state.client.available();
    if (avail <= 0) {
      return;
    }
    if(first){
      t0 = millis(); 
      first = false;
    }
    state.rx.data[bytes_received] = state.client.read();
    bytes_received++;
    if(bytes_received>=BUFFER_SIZE){
      break;
    }
  }
  // state.tx.values.time = (millis()-t0);
  IPAddress ip = state.client.remoteIP();
  memcpy(state.tx.data,tx.data,BUFFER_SIZE);
  memcpy(rx.data,state.rx.data,BUFFER_SIZE);
  for(int i=0;i<16;i++){
    target_current[i] = (int)(rx.values.target_current[i])/4.8125; // 800 ticks == 3850mA
  }
  state.client.write((char *)state.tx.data,BUFFER_SIZE);
  state.client.flush();

  state.client.closeOutput();
  state.closedTime = millis();
  state.outputClosed = true;
}

// Main program loop.
void loop() {
  unsigned long t0 = millis();
  
  // read LED currents
  digitalWrite(LED_DIAG_ENABLE_A,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[0] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[1] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_A,  false);

  digitalWrite(LED_DIAG_ENABLE_B,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[2] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[3] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_B,  false);

  digitalWrite(LED_DIAG_ENABLE_C,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[4] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[5] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_C,  false);

  digitalWrite(LED_DIAG_ENABLE_D,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[6] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[7] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_D,  false);

  digitalWrite(LED_DIAG_ENABLE_E,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[8] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[9] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_E,  false);

  digitalWrite(LED_DIAG_ENABLE_F,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[10] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[11] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_F,  false);

  digitalWrite(LED_DIAG_ENABLE_G,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[12] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[13] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_G,  false);

  digitalWrite(LED_DIAG_ENABLE_H,  true);
  digitalWrite(LED_SEL1, false);
  delay(1);
  current_raw[14] = analogRead(LED_SENS);
  digitalWrite(LED_SEL1, true);
  delay(1);
  current_raw[15] = analogRead(LED_SENS);
  digitalWrite(LED_DIAG_ENABLE_H,  false);

  interlock = !digitalRead(LED_ENABLE);
  if(interlock){
    for(int i=0;i<8;i++){
      dac_0->write(i,0);
      dac_1->write(i,0);
      target_current[i] = 0;
      target_current[i+8] = 0;
      gate_sp[i] = 0;
      gate_sp[i+8] = 0;
    }
    digitalWrite(LED_LATCH, false);
  }else{
    digitalWrite(LED_LATCH, true);
    bool all_off = true;     
    for(int i=0;i<16;i++){
      if(target_current[i]==0){
        gate_sp[i] = 0;
      }else{
        all_off = false;
      }

      if(target_current[i]>0 && gate_sp[i]==0){
        gate_sp[i] = 2500;
      }

      if(current_raw[i]<target_current[i]){
        if(gate_sp[i]<3600){
          gate_sp[i]+=1;
        }
      }else{
        if(gate_sp[i]>0){
          gate_sp[i]-=1;  
        }
      }
    }

    for(int i=0;i<8;i++){
      dac_0->write(i,gate_sp[i]);
      dac_1->write(i,gate_sp[i+8]);
    }

    
  }
  tx.values.control = interlock;

  unsigned long t1 = millis();

  for(int i=0;i<17;i++){
    temp_raw[i] = analogRead(temp_pins[i]);
    temp[i] = temp[i]*0.9+0.1*calcTemp(temp_raw[i],temp_poly);
  }

  for(int i=0;i<16;i++){
    tx.values.target_current[i] = target_current[i]*4.8125; // 800 ticks == 3850mA
    tx.values.current[i] = current_raw[i]*4.8125; // 800 ticks == 3850mA
    tx.values.gate[i] = gate_sp[i];
    tx.values.temperature[i] = temp[i];
  }
  tx.values.temperature[16] = temp[16];
  tx.values.time = millis()-t0;

  tx.values.led_fan = rx.values.led_fan;
  tx.values.chamber_fan = rx.values.chamber_fan;

  analogWrite(LED_FAN,255-rx.values.led_fan);
  analogWrite(CHAMBER_FAN,255-rx.values.chamber_fan);

  tx.values.crc = crc32.calc((uint8_t const *)&tx.data[0], BUFFER_SIZE-4);

  if(iteration++%10==0){
    // userial.write((char*)tx.data,BUFFER_SIZE);
    // userial.readBytes((char*)&rx.data[0], BUFFER_SIZE);
    // uint32_t crc = crc32.calc((uint8_t const *)&rx.data[0], BUFFER_SIZE-4);
    // if(crc==rx.values.crc){
    //   for(int i=0;i<16;i++){
    //     target_current[i] = (rx.values.target_current[i]>=0 && rx.values.target_current[i]<100)?rx.values.target_current[i]:0;
    //   }
    //   if(rx.values.chamber_fan!=tx.values.chamber_fan){
    //     analogWrite(CHAMBER_FAN,rx.values.chamber_fan);
    //     tx.values.chamber_fan = rx.values.chamber_fan;
    //   }
    //   if(rx.values.led_fan!=tx.values.led_fan){
    //     analogWrite(LED_FAN,rx.values.led_fan);
    //     tx.values.led_fan = rx.values.led_fan;
    //   }
    // }
  }

  {
    EthernetClient client = server.accept();
    if (client) {
      // We got a connection!
      IPAddress ip = client.remoteIP();
      clients.emplace_back(std::move(client));
    }

    // Process data from each client
    for (ClientState &state : clients) {  // Use a reference so we don't copy
      if (!state.client.connected()) {
        state.closed = true;
        continue;
      }

      // Check if we need to force close the client
      if (state.outputClosed) {
        if (millis() - state.closedTime >= kShutdownTimeout) {
          IPAddress ip = state.client.remoteIP();
          state.client.close();
          state.closed = true;
          continue;
        }
      } else {
        if (millis() - state.lastRead >= kClientTimeout) {
          IPAddress ip = state.client.remoteIP();
          state.client.close();
          state.closed = true;
          continue;
        }
      }

      processClientData(state);
    }
    

    // Clean up all the closed clients
    size_t size = clients.size();
    clients.erase(std::remove_if(clients.begin(), clients.end(),
                                [](const auto &state) { return state.closed; }),
                  clients.end());
  }
  tx.values.time = millis()-t0;
}
