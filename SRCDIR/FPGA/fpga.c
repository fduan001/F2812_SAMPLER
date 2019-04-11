/********************************************************************************

 **** Copyright (C), 2018, xx xx xx xx info&tech Co., Ltd.                ****

 ********************************************************************************
 * File Name     : fpga.c
 * Author        : yexin
 * Date          : 2018-07-27
 * Description   : fpga function
 * Version       : 1.0
 * Function List :
 *
 * Record        :
 * 1.Date        : 2018-07-27
 *   Author      : yexin
 *   Modification: Created file

*************************************************************************************************************/
#include <stdio.h>
#include "F2812_datatype.h"
#include "boardcfg.h"

void WriteFpgaRegister( UINT32 regaddr, UINT16 regvalue)
{
    volatile UINT16 *regPtr;

    regPtr = (volatile UINT16 *)regaddr;
    *(regPtr) = regvalue;
}

UINT16 ReadFpgaRegister( UINT32 regaddr )
{
    volatile UINT16 *regPtr;
    regPtr = (volatile UINT16 *)regaddr;
    return ((*regPtr) & 0xffff);
}



void WriteFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue)
{

    UINT16 regValue;
    regValue = ReadFpgaRegister(regaddr);
    if(0 == bitvalue)
        regValue = regValue & ( ~(1 << bitpos));
    else
        regValue = regValue | (1 << bitpos);
    WriteFpgaRegister(regaddr, regValue);

}


UINT8 ReadFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue)
{

    UINT16 regValue;
    regValue = ReadFpgaRegister(regaddr);
    return((regValue >> bitpos) & 0x1);
}


