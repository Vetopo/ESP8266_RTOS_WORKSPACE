#include "eloop.h"
#include "zigbee_conf.h"
#include "ZCL.h"
#include "MT_Uart.h"
#include "MT_Output.h"

uint8 MT_SendZCLCommand(uint16 dst,uint8 ep,uint16 clusterID, uint8 cmd,
						uint8 specific, uint8 direction,uint8 disableDefaultRsp,
						uint16 manuCode, uint8 seqNum,
                        uint16 cmdFormatLen, uint8 *cmdFormat )
{
	ZCL_CMD_t	*pZclCmd;
	uint8 		len;
	uint8		ret = ES_FAILED;
	eloop_log(0,"send to addr:%04x ep:%02x seq:%02x \r\n",dst,ep,seqNum);
	len = sizeof(ZCL_CMD_t) + cmdFormatLen;
	pZclCmd = (ZCL_CMD_t*)eloop_malloc(len);
	if(pZclCmd!=NULL)
	{
		pZclCmd->addr = dst;
		pZclCmd->ep = ep;
		pZclCmd->cID = clusterID;
		pZclCmd->seq = seqNum;
		pZclCmd->disable_respone = disableDefaultRsp;
		pZclCmd->len = 6 + cmdFormatLen;
		pZclCmd->payload.cmd = cmd;
		pZclCmd->payload.specific = specific;
		pZclCmd->payload.direction = direction;
		pZclCmd->payload.manucode = manuCode;
		pZclCmd->payload.dlen = cmdFormatLen;
		if(cmdFormatLen>0)
		{
			eloop_memcpy(pZclCmd->payload.data,cmdFormat,cmdFormatLen);
		}
		ret = MT_ReqSend(ZB_ZCL_SEND_MAX_TRY,dst,
        (MT_RPC_CMD_SREQ|MT_RPC_SYS_APP),MT_CmdAppZCLCommand,
        (uint8*)pZclCmd,len);
		eloop_free(pZclCmd);
	}
	return ret;
}
