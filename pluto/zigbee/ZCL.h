
#ifndef __ZCL_H__
#define __ZCL_H__

typedef struct
{
	uint16 	addr;
	uint8  	ep;
	uint8  	seq;
	uint8  	disable_respone;
	uint16 	cID;
	uint8  	len;
	struct
	{
		uint8  cmd;
		uint8  specific;
		uint8  direction;
		uint16 manucode;
		uint8  dlen;
		uint8  data[0];
	}payload;
}ZCL_CMD_t;

#endif


