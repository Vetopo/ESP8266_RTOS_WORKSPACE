#include "eloop.h"
#include "ZDO.h"
#include "zigbee_conf.h"
#include "MT_Output.h"

uint8 MT_SendZDOCommand(uint8 stry, uint8 cmd0, uint8 cmd1,uint16 addr, uint8 *pdata, uint8 len)
{
	uint8 ret = MT_ReqSend(stry,addr,cmd0,cmd1,pdata,len);
	return ret;
}
