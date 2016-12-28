/**
 * \file
 *       CC2530 udp Bridge
 * \author
 *       Li Yong <li_young@126.com>
 */

#include "cc2530udp.h"

UDP::UDP(CC *c)
{
  cc = c;
  handle = 0;
  timeout = DEFAULT_REST_TIMEOUT;
}

bool UDP::udp_new(const char* host, uint32_t rport, uint32_t lport)
{
	uint32_t cb_data;

	uint16_t crc = cc->request(CMD_UDP_NEW, 4, 1, 0);
	crc = cc->request(crc,(uint8_t*)host, strlen(host));
	crc = cc->request(crc,(uint8_t*)&rport, 2);
	crc = cc->request(crc,(uint8_t*)&lport, 2);
	cb_data = (uint32_t)&dataCb;
	crc = cc->request(crc,(uint8_t*)&cb_data, 4);
	cc->request(crc);

	if(cc->waitReturn(timeout) && cc->return_value != 0){
		handle = cc->return_value;
		return true;
	}
	return false;
}

bool UDP::udp_new(IPAddress remote, uint32_t rport, uint32_t lport)
{
	uint32_t cb_data;
	uint32_t addr = (uint32_t)remote;
	
	uint16_t crc = cc->request(CMD_UDP_NEW, 4, 1, 0);
	crc = cc->request(crc,(uint8_t*)&addr, sizeof(uint32_t));
	crc = cc->request(crc,(uint8_t*)&rport, 2);
	crc = cc->request(crc,(uint8_t*)&lport, 2);
	cb_data = (uint32_t)&dataCb;
	crc = cc->request(crc,(uint8_t*)&cb_data, 4);
	cc->request(crc);

	if(cc->waitReturn(timeout) && cc->return_value != 0){
		handle = cc->return_value;
		return true;
	}
	return false;
}

void UDP::send_packet(uint8_t *data, uint16_t len)
{
  uint16_t crc;

  if(handle == 0)
    return;

  crc = cc->request(CMD_UDP_SEND_PACKET, 3, 0, 0);
  crc = cc->request(crc,(uint8_t*)&handle, 4);
  crc = cc->request(crc,(uint8_t*)&len, 2);
  crc = cc->request(crc,(uint8_t*)data, len);
  cc->request(crc);
  return;
}

