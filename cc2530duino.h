/**
 * \file
 *       cc2530 bridge arduino library 
 * \author
 *       Yong Li <li_young@126.com>
 */
#ifndef _ARDUINO_CC2530_H_
#define _ARDUINO_CC2530_H_
#include <stdint.h>
#include <HardwareSerial.h>
#include <Arduino.h>
#include "FP.h"
#include "crc16.h"
#include "ringbuf.h"

#define ESP_TIMEOUT 2000

#define SLIP_START 0x7E
#define SLIP_END  0x7F
#define SLIP_REPL 0x7D
#define SLIP_ESC(x) (x ^ 0x20)

typedef enum
{
  CMD_NULL = 0,
  CMD_RESET,
  CMD_IS_READY,
  CMD_UDP_NEW,
  CMD_UDP_SEND_PACKET,
  CMD_UDP_EVENTS,
  CMD_TCP_SOCKET_REGISTER,
  CMD_TCP_SOCKET_CONNECT,
  CMD_TCP_SOCKET_SEND,
  CMD_TCP_SOCKET_CLOSE,
  CMD_TCP_EVENTS	
}CMD_NAME;

typedef struct{
  uint8_t *buf;
  uint16_t bufSize;
  uint16_t dataLen;
  uint8_t isEsc;
  uint8_t isBegin;
}PROTO;

typedef struct __attribute((__packed__)) {
  uint16_t len;
  uint8_t data;
} ARGS;

typedef struct __attribute((__packed__)) {
  uint16_t cmd;
  uint16_t argc;
  uint32_t _return;
  uint32_t callback;
  ARGS args;
}PACKET_CMD;

class CC;
class RESPONSE;

class RESPONSE {
private:
  uint16_t arg_num;
  uint8_t *arg_ptr;
  PACKET_CMD* cmd;
public:
  RESPONSE(void *response);
  uint16_t getArgc();
  int32_t popArgs(uint8_t *data, uint16_t maxLen);
  uint16_t argLen();
  String popString();
  void popString(String* data);
};

class CC
{
public:
  CC(Stream *serial, Stream* debug, int rts_pd, int chip_pd);
  CC(Stream *serial, int rts_pd, int chip_pd);
  Stream *_debug;

  uint32_t return_value;
  uint16_t return_cmd;
  boolean is_return;

  uint8_t process();
  uint16_t request(uint16_t cmd, uint16_t argc, uint32_t _return, uint32_t callback);
  uint16_t request(uint16_t crc_in, uint8_t* data, uint16_t len);
  uint16_t request(uint16_t crc);
  void reset();
  boolean ready();
  void enable();
  void disable();
  boolean waitReturn(uint32_t timeout);
  boolean waitReturn();

  void configure(Stream *serial, Stream* debug, int chip_pd) {_serial = serial; _debug = debug; _chip_pd = chip_pd;};


private:
  Stream *_serial;
  
  boolean _debugEn;
  PROTO _proto;
  uint8_t _protoBuf[128];
  int _chip_pd;
  int _rts_pd;

  void init();
  void INFO(String info);
  void protoCompletedCb(void);
  void write(uint8_t data);
  void write(uint8_t* data, uint16_t len);
};
#endif
