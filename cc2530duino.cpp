/**
 * \file
 *       ESP8266 bridge arduino library 
 * \author
 *       Tuan PM <tuanpm@live.com>
 */
#include "cc2530duino.h"

#include <stdlib.h>
#include <stdio.h>
char debug_buf[30];

RESPONSE::RESPONSE(void * response)
{
  cmd = (PACKET_CMD*)response;
  arg_ptr = (uint8_t*)&cmd->args;
  arg_num = 0;
}

uint16_t RESPONSE::getArgc()
{
  return cmd->argc;
}

uint16_t RESPONSE::argLen()
{
  return *(uint16_t*)arg_ptr;
}

int32_t RESPONSE::popArgs(uint8_t *data, uint16_t maxLen)
{
  uint16_t length, len, incLen = 0;

   if(arg_num >= cmd->argc)
     return -1;
 
  length = *(uint16_t*)arg_ptr;
  len = length;
  arg_ptr += 2;

  while(length --){
    *data ++ = *arg_ptr ++;
    incLen ++;
    if(incLen > maxLen){
      arg_num ++;
      return maxLen;
    }
      
  }
  arg_num ++;
  return len;
}

String RESPONSE::popString()
{
  String ret;
  uint16_t len = *(uint16_t*)arg_ptr;
  arg_ptr += 2;
  while(len --)
    ret += (char)*arg_ptr ++;
  arg_num ++;
  return ret;
}

void RESPONSE::popString(String* data)
{
  uint16_t len = *(uint16_t*)arg_ptr;
  arg_ptr += 2;
  while(len --)
    data->concat( (char)*arg_ptr ++);
  arg_num ++;
}

void CC::protoCompletedCb(void)
{
  PACKET_CMD *cmd = (PACKET_CMD*)_proto.buf;
  uint16_t crc = 0, argc, len, resp_crc;
  uint8_t *data_ptr;
  argc = cmd->argc;
  data_ptr = (uint8_t*)&cmd->args ;
  crc = crc16_data((uint8_t*)&cmd->cmd, 12, crc);
  while(argc--){
    len = *((uint16_t*)data_ptr);
    crc = crc16_data(data_ptr, 2, crc);
    data_ptr += 2;
    while(len --){
      crc = crc16_data(data_ptr, 1, crc);
      data_ptr ++;
    }
  }
  resp_crc =  *(uint16_t*)data_ptr;
  if(crc != resp_crc) {
	  sprintf(debug_buf, "ARDUINO: Invalid CRC : 0x%4x, 0x%04x", resp_crc, crc);
      INFO(debug_buf);
    return;
  }

  FP<void, void*> *fp;
  if(cmd->callback != 0){
    fp = (FP<void, void*>*)cmd->callback;

    return_cmd = cmd->cmd;
    return_value = cmd->_return;

    if(fp->attached())
      (*fp)((void*)cmd);
  } else {
    if(cmd->argc == 0) {
      is_return = true;
      return_cmd = cmd->cmd;
      return_value = cmd->_return;
    }
    
  }
}

void CC::write(uint8_t data)
{
  while(digitalRead(_rts_pd)) {
	INFO("Controller Flow OFF ! wait 1ms.");
	delay(1);
  }
  
  switch(data){
  case SLIP_START:
  case SLIP_END:
  case SLIP_REPL:
    _serial->write(SLIP_REPL);
    _serial->write(SLIP_ESC(data));
    break;
  default:
    _serial->write(data);
  }
}

void CC::write(uint8_t* data, uint16_t len)
{
  while(len --)
    write(*data ++);
}

uint16_t CC::request(uint16_t cmd, uint16_t argc, uint32_t _return, uint32_t callback)
{
  uint16_t crc = 0;
  _serial->write(0x7E);
  write((uint8_t*)&cmd, 2);
  //sprintf(debug_buf, "0x%02x 0x%02x ", ((uint8_t*)&cmd)[0], ((uint8_t*)&cmd)[1]);
  //INFO(debug_buf);
  crc = crc16_data((uint8_t*)&cmd, 2, crc);

  write((uint8_t*)&argc, 2);
  //sprintf(debug_buf, "0x%02x 0x%02x ", ((uint8_t*)&argc)[0], ((uint8_t*)&argc)[1]);
  //INFO(debug_buf);
  crc = crc16_data((uint8_t*)&argc, 2, crc);
  
  write((uint8_t*)&_return, 4);
  //sprintf(debug_buf, "0x%02x 0x%02x 0x%02x 0x%02x ", ((uint8_t*)&_return)[0], ((uint8_t*)&_return)[1], ((uint8_t*)&_return)[2], ((uint8_t*)&_return)[3]);
  //INFO(debug_buf);
  crc = crc16_data((uint8_t*)&_return, 4, crc);
  
  write((uint8_t*)&callback, 4);
  //sprintf(debug_buf, "0x%02x 0x%02x 0x%02x 0x%02x ", ((uint8_t*)&callback)[0], ((uint8_t*)&callback)[1], ((uint8_t*)&callback)[2], ((uint8_t*)&callback)[3]);
  //INFO(debug_buf);
  crc = crc16_data((uint8_t*)&callback, 4, crc);


  return crc;
}

uint16_t CC::request(uint16_t crc_in, uint8_t* data, uint16_t len)
{
  uint8_t temp = 0;
  uint16_t pad_len = len;
  while(pad_len % 2 != 0)
    pad_len++;
  write((uint8_t*)&pad_len, 2);
  crc_in = crc16_data((uint8_t*)&pad_len, 2, crc_in);
  while(len --){
    write(*data);
    crc_in = crc16_data((uint8_t*)data, 1, crc_in);
    data ++;
    if(pad_len > 0) pad_len --;
  }
  
  while(pad_len --){
    write(temp);
    crc_in = crc16_data((uint8_t*)&temp, 1, crc_in);
  }
  return crc_in;
}

uint16_t CC::request(uint16_t crc)
{
  write((uint8_t*)&crc, 2);
  _serial->write(0x7F);
  return crc;
}

void CC::init()
{
  _proto.buf = _protoBuf;
  _proto.bufSize = sizeof(_protoBuf);
  _proto.dataLen = 0;
  _proto.isEsc = 0;
  pinMode(_chip_pd, OUTPUT);
  pinMode(_rts_pd, INPUT);

}

CC::CC(Stream *serial, int rts_pd, int chip_pd):
_serial(serial), _rts_pd(rts_pd), _chip_pd(chip_pd)
{
  _debugEn = false;
  init();
}

CC::CC(Stream *serial, Stream* debug, int rts_pd, int chip_pd):
_serial(serial), _debug(debug), _rts_pd(rts_pd),_chip_pd(chip_pd)
{
    _debugEn = true;
    //_serial = _debug;
    init();
}
void CC::enable()
{
	digitalWrite(_chip_pd, HIGH);
}
void CC::disable()
{
	digitalWrite(_chip_pd, LOW);
}

void CC::INFO(String info)
{
  if(_debugEn)
    _debug->println(info);
}

void CC::reset()
{
  uint16_t crc = request(CMD_RESET, 0, 0, 0);
  request(crc);
}

// char buf[100];
boolean CC::ready()
{
  uint32_t wait;
  int i =0;

  for(uint8_t wait_time=5; wait_time>0; wait_time--){
	i =0;
    is_return = false;
    return_value = 0;
	INFO("Send CMD_IS_READY");
    uint16_t crc = request(CMD_IS_READY, 0, 1, 0);
    request(crc);
    wait = millis();
	//INFO("Send CMD_IS_READY 1");
    while(is_return == false && (millis() - wait < 1000)) {
	  //INFO("Send CMD_IS_READY 2");
	  #if 0
	  while(_serial->available()) {
		value = _serial->read();
		buf[i] = value;
		i++;
		if(i >= 18) {
			sprintf(debug_buf, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x", buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7],buf[8]);
			INFO(debug_buf);
			sprintf(debug_buf, "0x%02x 0x%02x 0x%02x 0x%02x 0x%02x 0x%02x %d %d 0x%02x", buf[9],buf[10],buf[11],buf[12],buf[13],buf[14],buf[15],buf[16],buf[17]);
			INFO(debug_buf);
		}
	  }

	  
	  #endif
      process();
    }
    if(is_return && return_value)
      return true;
  }
  return false;
  
}
boolean CC::waitReturn(uint32_t timeout)
{
  is_return = false;
  return_value = 0;
  return_cmd = 0;
  uint32_t wait = millis();
  while(is_return == false && (millis() - wait < timeout)) {
    process();
  }
  return is_return;
}

boolean CC::waitReturn()
{
  return waitReturn(ESP_TIMEOUT);
}

uint8_t CC::process()
{
  char value;
  uint8_t packets = 0;
  while(_serial->available()) {
    value = _serial->read();
	//sprintf(debug_buf, "0x%02x ", value);
    //INFO(debug_buf);
    switch(value){
    case 0x7D:
      _proto.isEsc = 1;
      break;
    
    case 0x7E:
      _proto.dataLen = 0;
      _proto.isEsc = 0;
      _proto.isBegin = 1;
      break;
    
    case 0x7F:
#if 0	
	  INFO("CmpleteCB");
	  for(i = 0; i < _proto.dataLen; i++) {
		sprintf(debug_buf, "0x%02x ", _proto.buf[i]);
		INFO(debug_buf);
	  }
#endif		
      protoCompletedCb();
      _proto.isBegin = 0;
	  packets++;
      break;
    
    default:
      if(_proto.isBegin == 0) {
        if(_debugEn) {
            _debug->write(value);
        }
        break;
      }
      if(_proto.isEsc){
        value ^= 0x20;
        _proto.isEsc = 0;
      }
        
      if(_proto.dataLen < _proto.bufSize)
        _proto.buf[_proto.dataLen++] = value;
        
      break;
    }
  }

  return packets;
}
