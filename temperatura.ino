/*  Este código pertenece al Transmisor que lee las temperaturas. 
 *  SU DIRECCION MAC ES:80 7d 3a f3 75 a0   
 */
// se agregan las librerias que permiten el enlace inalambrico
#include <esp_now.h>
#include <WiFi.h>

#include "esp32-hal-adc.h" // needed for adc pin reset
#include "soc/sens_reg.h" // needed for adc pin reset
uint64_t reg_b; // Used to store Pin registers

#define PIN 13 // Set ADC2 Pin here
int value;

//  THE MAC Address of your receiver 
//  Mi receptor será  8C:AA:B5:85:59:70
uint8_t broadcastAddress[] = {0x30, 0xAE, 0xA4, 0xEF, 0xB9, 0xE4};
// Estructura del dato enviado en las ESP_NOW
typedef struct struct_message {
  int id;
  float t1;
  float t2;
  float t3;
  float t4;
  float t5;
  float t6;
  int i1;
  int i2;
  int i3;
  int v1;
  int v2;
  int v3;
  int v4;
  int muestras;
  int micro;
} struct_message;

// Create a struct_message called tempReadings 
struct_message tempReadings;
// Create a struct_message to hold incoming from esp_now.
struct_message wirelessReadings;
// Variable to store if sending data was successful
String success;
// Callback when data from TEMPs is sent to esp_now()
void OnDataSent(const uint8_t *mac_addr, esp_now_send_status_t status) {
}
// Callback when data is received from esp_now() and sent to LED
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
}
 
void setup() {
  reg_b = READ_PERI_REG(SENS_SAR_READ_CTRL2_REG);
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);
  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    // encender led_built_in
    return;
  }
  Serial.begin(115200);
  Serial.println(WiFi.macAddress());
  // Once ESPNow is successfully Init, we will register for Send CB to
  // get the status of Trasnmitted packet
  esp_now_register_send_cb(OnDataSent);
  // Register peer
  esp_now_peer_info_t peerInfo;
  memcpy(peerInfo.peer_addr, broadcastAddress, 6);
  peerInfo.channel = 0;  
  peerInfo.encrypt = false;
  // Add peer        
  if (esp_now_add_peer(&peerInfo) != ESP_OK){
      Serial.println("Failed to add peer");
    return;
  }
  // Register for a callback function that will be called when data is received
  esp_now_register_recv_cb(OnDataRecv);
}
 
void loop() {
  get_temp();
  // Send message via ESP-NOW  
  esp_err_t result = esp_now_send(broadcastAddress, (uint8_t *) &tempReadings, sizeof(tempReadings));
    //digitalWrite(transmitir, LOW);
  if (result == ESP_OK) {
    Serial.println("envio correcto");
    //led indication
  }
  else {
    Serial.println("Envio incorrecto");
    // led indication
  }
  delay(500);
}

void get_temp(){
  tempReadings.id = 1;
  
  WRITE_PERI_REG(SENS_SAR_READ_CTRL2_REG, reg_b);
  tempReadings.t1 = analogRead(PIN);

  //tempReadings.t1 = analogRead(13);
  //tempReadings.t1 = max1.temperature(RNOMINAL, RREF);
  tempReadings.t2 = analogRead(12);
  tempReadings.t3 = analogRead(14);
  tempReadings.t4 = analogRead(27);
  tempReadings.t5 = analogRead(26);
  tempReadings.t6 = analogRead(25);
  Serial.print("lectura de temperatura");
  Serial.println();
  Serial.println(tempReadings.t1);
  Serial.println(tempReadings.t2);
  Serial.println(tempReadings.t3);
  Serial.println(tempReadings.t4);
  Serial.println(tempReadings.t5);
  Serial.println(tempReadings.t6); 
  //Serial.println();
}

