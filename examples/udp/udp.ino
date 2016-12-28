/**
 * \file
 *       CC2530 UDP Bridge example
 * \author
 *       Yong Li <steven.liyong@gmail.com>
 */
#include <SoftwareSerial.h>
#include <cc2530duino.h>
#include <cc2530udp.h>
#include "DHT.h"

// https://www.arduino.cc/en/Reference/SoftwareSerial
/* Not all pins on the Mega and Mega 2560 support change interrupts, 
 *  so only the following can be used for RX: 10, 11, 12, 13, 14, 15, 
 *  50, 51, 52, 53, A8 (62), A9 (63), A10 (64), A11 (65), A12 (66), 
 *  A13 (67), A14 (68), A15 (69).
 */
SoftwareSerial DBGSerial(13, 12); // RX, TX
#ifndef __AVR__
#define CCSerial Serial1
#else
/* Notes :
   1) Disconnect the Serial connection between ccduino and UNO before doing the programming.
   2) Don't use PC USB Port (PC USB Port has the interal UART Tx/Rx) after programming, use the externl USB port charger.  
*/
#define CCSerial Serial
#endif
CC cc(&CCSerial, &DBGSerial, 7, 4);
UDP udp(&cc);
#ifndef __AVR__
IPAddress server(123, 56, 135, 41);
#else
IPAddress server(123, 56, 135, 41);
#endif

#define CC_STATUS 5

DHT dht;
#define DH11_PIN 6

void udpData(void* response)
{
  /* Should not block this callback, this callback is from the uart isr !!!! 
  otherwise will cause the abnormal behavior of the stream class */
  RESPONSE res(response);
  DBGSerial.print("Received: data=");
  String recv = res.popString();
  DBGSerial.println(recv);
}

const char UDP_FAIL_STR[] = "udp new fail!";
const char UDP_READY_STR[] = "udp ready!";
const char UDP_RESTART[] = "ccdunio reset!, starting...";

void setup() {
  CCSerial.begin(115200);
  // https://www.arduino.cc/en/Reference/SoftwareSerial
  // If using multiple software serial ports, only one can receive data at a time.
  DBGSerial.begin(115200);

  dht.setup(DH11_PIN); // data pin 6
  
  DBGSerial.println(UDP_RESTART);
  pinMode(CC_STATUS, OUTPUT);
  digitalWrite(CC_STATUS, 0);
 
  cc.disable();
  delay(10);
  cc.enable();
  delay(1000);
  cc.reset();
  delay(100);
  while(!cc.ready());
  digitalWrite(CC_STATUS, 1);

  udp.dataCb.attach(&udpData);
  bool result = udp.udp_new(server, 5990, 3000);
  if(result == false) {
    DBGSerial.println(UDP_FAIL_STR);
  }
  DBGSerial.println(UDP_READY_STR);

  
}

void loop() {
  static uint32_t loop_count = 0;
  uint32_t humVal = 0;
  uint32_t tempVal = 0;
  uint32_t timeout = 5000;
  DBGSerial.println("ccduino loop start..");

  float humidity = dht.getHumidity();
  float temperature = dht.getTemperature();
  humVal = (uint32_t)humidity;
  tempVal = (uint32_t)temperature;
  
  if(!cc.ready()) {
    digitalWrite(CC_STATUS, 0);

    cc.disable();
    delay(10);
    cc.enable();
    delay(1000);
    cc.reset();
    delay(100);
    while(!cc.ready());
    digitalWrite(CC_STATUS, 1);

    udp.dataCb.attach(&udpData);  
    bool result = udp.udp_new(server, 5990, 3000);
    if(result == false) {
      DBGSerial.println(UDP_FAIL_STR);
    }
    DBGSerial.println(UDP_READY_STR);
  }
  
  cc.process();
 
  udp.send_packet((unsigned char*)(&humVal), sizeof(humVal)); 
  udp.send_packet((unsigned char*)(&tempVal), sizeof(tempVal)); 
  while(timeout -- ) {
     delay(1);
     cc.process();
  }
  loop_count++;
}
