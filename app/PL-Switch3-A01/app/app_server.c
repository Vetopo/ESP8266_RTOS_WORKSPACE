
#include "eloop.h"
#include "pluto.h"
#include "app_server.h"

//const uint8 test_sn[]={0xBA,0x13,0xD4,0x6C,0x3F,0x28,0x97,0x16,0xB5,0x6B,0xEA,0x28,0xA0,0x54,0x85,0xBD};
//const uint8 test_addr[]={0x01,0x56,0x05,0x00,0x01,0x00,0x00,0x30};
const uint8 test_sn[]={0xD1,0x24,0x47,0x48,0x06,0x99,0xE0,0x86,0x03,0x28,0x79,0x7E,0xC9,0x05,0x81,0x74};
const uint8 test_addr[]={0x01,0x56,0x05,0x00,0x01,0x00,0x00,0x32};
const PlutoServer_t SnInfo= {
		.product_ID=1,//uint32	product_ID;//产品种类编码，HA类产品编码为1
		.vendor_ID = 1,//uint32  vendor_ID;
		.product_date="2018-07-06",//char	*product_date;
		.password="123456",//char	*password;
		.sn = (uint8*)test_sn,//uint8 	*sn;
		.addr = (uint8*)test_addr,//uint8	*addr;
		.url = "www.glalaxy.com",//char	*url;
		.server_ipv4="119.23.8.181",//"192.168.149.164",//"119.23.8.181",//char	*server_ipv4;
		.server_udp_port=16729,//uint16	server_udp_port;
		.server_tcp_port = 16729,//uint16	server_tcp_port;
		.local_udp_port = 16729,//uint16	local_udp_port;
		.version = 1 //uint16	version;//产品初始版本
};

void app_write_server_info(void)
{
    pluto_write_server((PlutoServer_t*)&SnInfo);
}