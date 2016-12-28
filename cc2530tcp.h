/**
 * \file
 *       CC2530 tcp Bridge
 * \author
 *       Li Yong <li_young@126.com>
 */
#ifndef _TCP_H_
#define _TCP_H_

#include <stdint.h>
#include <Arduino.h>
#include "FP.h"
#include <cc2530duino.h>
#include "IPAddress.h"

#define DEFAULT_REST_TIMEOUT  5000

class TCP {
private:
  uint32_t timeout;
  CC *cc;

public:
  
  FP<void, void*> dataCb;
  FP<void, void*> eventCb;
  
  TCP(CC *c);
  bool tcp_init();
  bool tcp_connect(IPAddress ip, uint16_t port);
  uint16_t tcp_send(uint8_t* data, uint16_t len);
  bool tcp_close();
};
#endif
