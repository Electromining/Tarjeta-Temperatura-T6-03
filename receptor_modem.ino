
/*
   23/11/2021
   Este es el software
   del receptor inalambrico (modem)
   Recibe de la tarjeta cvi34-01
   recibe de la tarjeta TI-6-03
   Envia  datos al software de adquisicion de datos
*/
// se agregan las librerias que permiten el enlace inalambrico
#include <esp_now.h>
#include <WiFi.h>
#include <Wire.h>

typedef struct struct_message {
  int   id;
  float t1;
  float t2;
  float t3;
  float t4;
  float t5;
  float t6;
  int   i1;
  int   i2;
  int   i3;
  int   v1;
  int   v2;
  int   v3;
  int   v4;
  int   muestras;
  int   micro;
} struct_message;


// Se crea una estructura llamada my_data
struct_message myData;

// Se declara un array de muestras
int muestras[15];
int contador = 201;
int contador_temperaturas = 201;

// Es una funcion que se ejecuta cuando se recibe data.
void OnDataRecv(const uint8_t * mac_addr, const uint8_t *incomingData, int len) {
  /*Por motivos de debug este codigo puede ser util
    char macStr[18];
    Serial.print("Se recibio el paquete de: ");
    snprintf(macStr, sizeof(macStr), "%02x:%02x:%02x:%02x:%02x:%02x",
           mac_addr[0], mac_addr[1], mac_addr[2], mac_addr[3], mac_addr[4], mac_addr[5]);
    Serial.println(macStr);
    memcpy(&myData, incomingData, sizeof(myData));

    Serial.printf("Board ID %u: %u bytes\n", myData.id, len);
  */
  // Update the structures with the new incoming data
  memcpy(&myData, incomingData, sizeof(myData));

  if ( myData.id == 0 ) { // si recibe del cvi
    contador_temperaturas = 0;
    muestras[6] = myData.i1;
    muestras[7] = myData.i2;
    muestras[8] = myData.i3;
    muestras[9] = myData.v1;
    muestras[10] = myData.v2;
    muestras[11] = myData.v3;
    muestras[12] = myData.v4;
    muestras[13] = myData.muestras;
    muestras[14] = myData.micro;
    if (contador == 0 && muestras[13] == 0)
      contador = 1;
    if ( contador != 0 && contador <= 200) {
      for (int i = 0; i < 14; i++) {
        Serial.print(muestras[i]);
        Serial.print(",");
      }
      Serial.println(muestras[14]);
      contador++;
    }
  }
  else { // si recive del t8
    muestras[0] = myData.t1 * 100;
    muestras[1] = myData.t2 * 100;
    muestras[2] = myData.t3 * 100;
    muestras[3] = myData.t4 * 100;
    muestras[4] = myData.t5 * 100;
    muestras[5] = myData.t6 * 100;
    if(contador_temperaturas > 5){
        for (int i = 0; i < 14; i++) {
            Serial.print(muestras[i]);
            Serial.print(",");
        }
        Serial.println(muestras[14]);
        contador_temperaturas = 0;
    } else {
        contador_temperaturas++;
    }
  }
}

void setup() {
  //Initialize Serial Monitor
  Serial.begin(115200);
  delay(1000);
  muestras[0] = -24202;
  muestras[1] = -24202;
  muestras[2] = -24202;
  muestras[3] = -24202;
  muestras[4] = -24202;
  muestras[5] = -24202;

  //Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  //Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }

  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(OnDataRecv);
}


void loop() {
  // Acess the variables for each board
  if (Serial.available() != 0) {
    char lectura = Serial.read();
    if (lectura == 'a') {
      contador = 0;
    }
  }
}