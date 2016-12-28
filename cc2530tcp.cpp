/**
 * \file
 *       CC2530 tcp Bridge
 * \author
 *       Li Yong <li_young@126.com>
 */

#include "cc2530tcp.h"

TCP::TCP(CC *c)
{
  cc = c;
  timeout = DEFAULT_REST_TIMEOUT;
}

bool TCP::tcp_init(void)
{
	uint32_t cb_data;

	uint16_t crc = cc->request(CMD_TCP_SOCKET_REGISTER, 2, 1, 0);
	cb_data = (uint32_t)&eventCb;
	crc = cc->request(crc,(uint8_t*)&cb_data, 4);
	cb_data = (uint32_t)&dataCb;
	crc = cc->request(crc,(uint8_t*)&cb_data, 4);
	cc->request(crc);

	if(cc->waitReturn(timeout) && cc->return_value != 0){
		return true;
	}
	return false;
}

bool TCP::tcp_connect(IPAddress remote, uint16_t port)
{
	uint32_t addr = (uint32_t)remote;
	
	uint16_t crc = cc->request(CMD_TCP_SOCKET_CONNECT, 2, 1, 0);
	crc = cc->request(crc,(uint8_t*)&addr, sizeof(uint32_t));
	crc = cc->request(crc,(uint8_t*)&port, 2);
	cc->request(crc);

	if(cc->waitReturn(timeout) && cc->return_value != 0){
		return true;
	}
	return false;
}

uint16_t TCP::tcp_send(uint8_t *data, uint16_t len)
{
  uint16_t crc;

  crc = cc->request(CMD_TCP_SOCKET_SEND, 2, 1, 0);
  crc = cc->request(crc,(uint8_t*)&len, 2);
  crc = cc->request(crc,(uint8_t*)data, len);
  cc->request(crc);
  
  if(cc->waitReturn(timeout) && cc->return_value != 0){
	return cc->return_value;
  }
  return 0;
}

bool TCP::tcp_close(void)
{
	uint16_t crc = cc->request(CMD_TCP_SOCKET_CLOSE, 0, 1, 0);
	cc->request(crc);

	if(cc->waitReturn(timeout) && cc->return_value != 0){
		return true;
	}
	return false;
}
  
  