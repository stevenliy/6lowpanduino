/**
 * \file
 *       CC2530 udp Bridge
 * \author
 *       Li Yong <li_young@126.com>
 */
#ifndef _UDP_H_
#define _UDP_H_

#include <stdint.h>
#include <Arduino.h>
#include "FP.h"
#include <cc2530duino.h>
#include "IPAddress.h"

#define DEFAULT_REST_TIMEOUT  5000

class UDP {
private:
  uint32_t timeout;
  CC *cc;
  
  uint32_t handle;

public:
  
  UDP(CC *c);

  FP<void, void*> dataCb;

  bool udp_new(IPAddress remote, uint32_t rport, uint32_t lport);
  bool udp_new(const char* host, uint32_t rport, uint32_t lport);
  
  
  void send_packet(uint8_t *data, uint16_t len);
  
};
#endif
