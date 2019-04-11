
#include <stdio.h>
#include <stdint.h>
#include <string.h>

#include "config.h"
#include "fpga.h"
#include "altera_rs422.h"
#include "semLib.h"
#include "taskLib.h"
#include "libgpio.h"
#include "gpio_pin_def.h"
#include "cmd_console.h"

typedef int BOOL;

#ifndef ERROR
#define ERROR   (-1)
#endif

#ifndef OK
#define OK    (0)
#endif

#define RX_LEN   2048

#define   RS422_IRQNO    GPIO_IRQ1

#define UART_REG(reg, pchan) \
 (*(volatile unsigned short *)(((unsigned int)(pchan->baseAddr)) + (reg * 1)))


typedef struct{

    BOOL isOpen;
    
	unsigned char RXBuffer[RX_LEN]; // [# of buffers][RX_LEN bytes]
	unsigned int  msgrd,msgwr;	
	
#ifdef C6748
	sys_sem_t   *rx_semSync;
#else
	SEM_ID   rx_semSync;
#endif
	
	unsigned int baseAddr;
	unsigned char reset_bit;
	unsigned char int_bit;
		
}  UART_BUFF;

int uartintUserCnt = 0;
UART_BUFF  altera_uart_buff[RS422_NUM];

unsigned char UART_RESET_BIT[RS422_NUM]={RS422_CHIP1_RESET_BIT,RS422_CHIP2_RESET_BIT};
unsigned char UART_INT_BIT[RS422_NUM]={RS422_CHIP1_INT_BIT,RS422_CHIP2_INT_BIT};

#ifndef C6748
IMPORT void sysUsDelay(UINT32 delay);
IMPORT void sysMsDelay(UINT32 delay);
#endif

void uartReset(unsigned char chipNo);
int uartTransBytes(UART_BUFF *  pdevFd, char *pBuf, int nBytes);
void uartRecvHandle(UART_BUFF * pDev);
int uartRecvBytes(UART_BUFF *  pdevFd, char *pBuf, int nBytes);


void DebugSysReg(void)
{
    PRINTF("0x%08x = 0x%04x\r\n",SYS_FPGA_RESET_REG,FPGA_REG16_R(SYS_FPGA_RESET_REG));
    PRINTF("0x%08x = 0x%04x\r\n",SYS_FPGA_INTF_REG,FPGA_REG16_R(SYS_FPGA_INTF_REG));
    PRINTF("0x%08x = 0x%04x\r\n",SYS_FPGA_INTMASK_REG,FPGA_REG16_R(SYS_FPGA_INTMASK_REG));
}
void DebugUartReg(UART_BUFF *pdevFd)
{
	if(pdevFd == NULL)
		return;
    PRINTF("0x%08x:IER = 0x%04x\r\n",pdevFd->baseAddr,UART_REG(IER,pdevFd));
    PRINTF("0x%08x:IIR = 0x%04x\r\n",pdevFd->baseAddr,UART_REG(IIR,pdevFd));
    PRINTF("0x%08x:LSR = 0x%04x\r\n",pdevFd->baseAddr,UART_REG(LSR,pdevFd));
    PRINTF("0x%08x:LCR = 0x%04x\r\n",pdevFd->baseAddr,UART_REG(LCR,pdevFd));    
}

void DebugUartRegInfo(unsigned char chipNo)
{
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
	 
    if(chipNo >= RS422_NUM)
	   return;
    
    // PRINTF("SYS INT REG = 0x%x\r\n",FPGA_REG16_R(SYS_FPGA_INTF_REG));
    DebugSysReg();
    DebugUartReg(pdevFd);
}


 void altera_uart_isr(void)
 {
	 UART_BUFF *pdevFd = NULL;
	 int channel = 0;
	 unsigned int isr = 0;	 
	 
	 FPGA_REG16_W(SYS_FPGA_INTMASK_REG, (FPGA_REG16_R(SYS_FPGA_INTMASK_REG) | (0x01 << RS422_IRQMASK_BIT)));   // IRQ1
	 
	 for(channel = 0;channel < RS422_NUM ;channel++)
	 {
		 pdevFd = &altera_uart_buff[channel];
		 
	      if(pdevFd == NULL || pdevFd->isOpen != TRUE)
	        continue;
	      
	      if((FPGA_REG16_R(SYS_FPGA_INTF_REG) & (1 << pdevFd->int_bit)) == 0)
	    	  continue;
	      
	     // logMsg("%s \r\n","altera_uart_isr",0,0,0,0,0); 
	      
	      if((UART_REG(IIR, pdevFd) & IIR_INT) != IIR_INT)
	      //if((UART_REG(LSR,pdevFd) & LSR_RECV_VALID) == LSR_RECV_VALID)
	      {
	    	 // logMsg("chipNo=%d recv...\r\n",channel,0,0,0,0,0); 
		      uartRecvHandle(pdevFd);	    	  
	      }

	 }
	 
     FPGA_REG16_W(SYS_FPGA_INTMASK_REG, (FPGA_REG16_R(SYS_FPGA_INTMASK_REG) & (~( 1 << RS422_IRQMASK_BIT))));   // IRQ1 
	 
	 return;
 }


int altera_uart_open(unsigned char chipNo,char party,unsigned char stop,unsigned char data_bit,unsigned int baud)
{
	int ret = OK;
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
	 
    if(chipNo >= RS422_NUM)
	   return(ERROR);
	    
    if(pdevFd->isOpen == TRUE)
    	return(ERROR);
    
    altera_uart_init(chipNo);
    
    if(altera_uart_setopt(chipNo,party,stop,data_bit) != OK)
    	return(ERROR);
    

    if(altera_uart_setbaud(chipNo,baud) != OK)
    	return(ERROR);
    
#ifdef C6748
    pdevFd->rx_semSync = sys_sem_create();
 #else
    pdevFd->rx_semSync = semBCreate(SEM_Q_FIFO, SEM_EMPTY);
#endif
    
     if(pdevFd->rx_semSync == NULL)
     {
         return ERROR;
     }
  
    pdevFd->isOpen = TRUE;
    
    UART_REG(IER, pdevFd) = IER_RECV_VALID; //EN RECV DATA INT

    if(uartintUserCnt == 0)
     {
#ifdef C6748
     	if( registerGpioIsrHandler(RS422_IRQNO, altera_uart_isr, NULL) != 0 ) 
#else
        if(intConnect(INUM_TO_IVEC(RS422_IRQNO),(VOIDFUNCPTR)altera_uart_isr, NULL) != OK)
#endif
         {
       	   PRINTF("altera uart install error......[irq = %d]\r\n",RS422_IRQNO);
         }   
         else
         {
        	 uartintUserCnt++;
             if(uartintUserCnt == 1)
             {
#ifndef C6748
               intEnable(RS422_IRQNO);
#endif
               FPGA_REG16_W(SYS_FPGA_INTMASK_REG, (FPGA_REG16_R(SYS_FPGA_INTMASK_REG) & (~( 1 << RS422_IRQMASK_BIT))));   // IRQ1
             }
         }
     }else uartintUserCnt++;   
    
    if(ret) {
    	PRINTF("altera_uart_open success....\r\n");
    }
    
    DebugSysReg();
    
    DebugUartReg(pdevFd);
    
	return ret;
}

int altera_uart_close(unsigned char chipNo)
{
    int ret = OK;
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
   
      if(chipNo >= RS422_NUM)
  	   return(ERROR);
      
      
      if(pdevFd->isOpen != TRUE)
      	return(ERROR);
      
      UART_REG(IER, pdevFd) = 0;
  
      if(pdevFd->rx_semSync != NULL)
      {
      	#ifdef C6748
      	  sys_sem_destroy(pdevFd->rx_semSync);
      	#else
          semDelete(pdevFd->rx_semSync );
        #endif
  	      pdevFd->rx_semSync  = NULL;
      }     
    	
  	 if(uartintUserCnt == 1) 	
  	 {
  		FPGA_REG16_W(SYS_FPGA_INTMASK_REG, (FPGA_REG16_R(SYS_FPGA_INTMASK_REG) | (0x01 << RS422_IRQMASK_BIT)));   // IRQ1
	#ifndef C6748
		intDisable(RS422_IRQNO);
  
  		 if(intDisconnect(INUM_TO_IVEC(RS422_IRQNO),(VOIDFUNCPTR)altera_uart_isr, NULL) != OK)
  	     {
		    	 PRINTF("altera uart uninstall error......[irq = %d]\r\n",RS422_IRQNO);
  	     }
  	#endif        
  	 }
   	 if(uartintUserCnt > 0)
  		uartintUserCnt--;
      
      memset(pdevFd,0,sizeof(UART_BUFF));
      
      pdevFd->isOpen = FALSE;
   
    return ret;
}

int altera_uart_read(unsigned char chipNo,char * buf,unsigned int nBytes)
{
    unsigned int readNum = 0;
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
 
    if(chipNo >= RS422_NUM)
	   return(ERROR);
    
    if(pdevFd->isOpen != TRUE)
    	return(ERROR);

#ifdef C6748
    if( sys_sem_wait(pdevFd->rx_semSync) != 0 )
#else
    if(ERROR==semTake(pdevFd->rx_semSync,WAIT_FOREVER))
#endif
    {	
 		return(ERROR);
     }

    while(((pdevFd->msgrd)!=(pdevFd->msgwr)) && (readNum < nBytes))
 	{	
	    buf[readNum] = pdevFd->RXBuffer[pdevFd->msgrd];
 		pdevFd->msgrd++;
 		if(pdevFd->msgrd == RX_LEN)
 		{
 			pdevFd->msgrd = 0;
 		}
 		readNum++;
 	}
     if((pdevFd->msgrd)!=(pdevFd->msgwr)) 
     	{
#ifdef C6748
     		sys_sem_post(pdevFd->rx_semSync);
#else
 	      	semGive(pdevFd->rx_semSync);
#endif
 	  	}
     
    // PRINTF("chipNo = %d,read data,len = %d\r\n",chipNo,readNum); 
     
    return readNum;
}

int altera_uart_write(unsigned char chipNo,char * buf,unsigned int nBytes)
{
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
     
    if (pdevFd == NULL)
        return (ERROR);
   
	if(pdevFd->isOpen != TRUE)
	{
	    return ERROR;
	}    

    uartTransBytes(pdevFd,buf,nBytes);	
   
    return nBytes;   
}

int altera_uart_init(unsigned char chipNo)
{
    int ret = OK;
    
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
 
    if(chipNo >= RS422_NUM)
	   return(ERROR);
    
    memset(pdevFd,0,sizeof(UART_BUFF));      

    pdevFd->baseAddr = RS422_BASE_ADDR+chipNo * 16;
    pdevFd->reset_bit = UART_RESET_BIT[chipNo];
    pdevFd->int_bit = UART_INT_BIT[chipNo];

    uartReset(chipNo);    
    
    UART_REG(FCR,pdevFd) = FCR_RECV_CLR | FCR_TRANS_CLR | FCR_RECV_TIG1 | 0x01;
    sysUsDelay(1);
    UART_REG(FCR,pdevFd) = FCR_RECV_TIG1 | 0x01;
    
    return ret;
}

int altera_uart_setopt(unsigned char chipNo,char party,unsigned char stop_bit,unsigned char data_bit)
{
    int ret = OK;
    unsigned char lcr;
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
    
    lcr = 0;
    
    if(party == 'O' || party == 'o')
    {
    	lcr |= LCR_PARITY_EN | LCR_PARITY_ODD;
    }
    else if(party == 'E' || party == 'e')
    {
    	lcr |= LCR_PARITY_EN | LCR_PARITY_EVEN;
    }
    
    if(data_bit == 5)
    	lcr |= LCR_DATA_BIT5;
    else if(data_bit == 6)
    	lcr |= LCR_DATA_BIT6;
    else if(data_bit == 7)
    	lcr |= LCR_DATA_BIT7;
    else
    	lcr |= LCR_DATA_BIT8;
    
    if(stop_bit == 2)
    	lcr |= LCR_STOP_BIT2;
    else
    	lcr |= LCR_STOP_BIT1;
    
    UART_REG(LCR,pdevFd) = lcr;

    return ret;
}

int altera_uart_setbaud(unsigned char chipNo,unsigned int baud)
{
    int ret = OK;
    unsigned short dll,dlh;
    unsigned int clkdiv;
    
    UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
    
    UART_REG(LCR,pdevFd) = (UART_REG(LCR,pdevFd) | LCR_CLKDIV_ACCESS);
    
    clkdiv = SYS_CLK / (16 * (unsigned int)baud);
    
   
    dlh = (clkdiv >> 8) & 0xff;
    dll = clkdiv & 0xff;
    
    
    UART_REG(DLH,pdevFd) = dlh;
    UART_REG(DLL,pdevFd) = dll;
    
    UART_REG(LCR,pdevFd) = (UART_REG(LCR,pdevFd) & (~LCR_CLKDIV_ACCESS));
    
    return ret;
}




int uartRecvBytes(UART_BUFF *  pdevFd, char *pBuf, int nBytes)
{
    int index = 0;
    char * ptr = pBuf;
    volatile unsigned int  tptr;	
   
    if (pdevFd == NULL)
        return (0);

    for(index = 0;index <nBytes;index++)
    {

        tptr =  pdevFd->msgwr+1;
				
	    if(tptr == RX_LEN)
	    {
	        tptr = 0;
	    }
	
	    if(tptr !=pdevFd->msgrd)//确保先写后读，如果写指针即将覆盖未读信息，则不再向缓冲区中写入
	    {  

		  pdevFd->RXBuffer[pdevFd->msgwr] = *ptr ;
		  
		 // logMsg("msgwr =%d,recv = 0x%x\r\n",pdevFd->msgwr,pdevFd->RXBuffer[pdevFd->msgwr],0,0,0,0); 
		  
		  ptr++;
    	  pdevFd->msgwr++;

    	 
			  
    		if(pdevFd->msgwr == RX_LEN)//如果写指针已经已经越界，则回头
    		{
    		     pdevFd->msgwr = 0;
    		}

	   }
	   else 
	      break;

    }
    return index;
}

void uartRecvHandle(UART_BUFF * pDev)
{
	UART_BUFF *pdevFd = (UART_BUFF *)pDev;
   unsigned char buf[4];
   unsigned short int data;
 
    while((UART_REG(LSR,pdevFd) & LSR_RECV_VALID) == LSR_RECV_VALID)
    	{
    	
    	 data = (unsigned short)UART_REG(RBR,pdevFd) ;
	     buf[0] = (data >> 0) & 0xff;
	     
	     if(uartRecvBytes(pdevFd,&buf[0],sizeof(buf[0])) != 0)
	     	{
#ifdef C6748
	     		sys_sem_post(pdevFd->rx_semSync);
#else
	     	    semGive(pdevFd->rx_semSync);
#endif
	     	}
     
    	}

      return;
}



int uartTransBytes(UART_BUFF *  pdevFd, char *pBuf, int nBytes)
{
    int index = 0;
    char * ptr = pBuf;
    unsigned char data;
   
    if (pdevFd == NULL)
        return (ERROR);

    for(index = 0;index < nBytes;index++)
    {
         while((UART_REG(LSR,pdevFd) & LSR_TRANS_FIFO_EMTPY) != LSR_TRANS_FIFO_EMTPY)
         {
 	 	    sysUsDelay(1);
         }

    	 data = (unsigned char)(*ptr);
    	 UART_REG(THR,pdevFd) = data;
    	 ptr++;
     }
    return nBytes;
}

void uartReset(unsigned char chipNo)
{
	
	UART_BUFF *pdevFd = (UART_BUFF *)&(altera_uart_buff[chipNo]);
	 
    FPGA_REG16_W(SYS_FPGA_RESET_REG, FPGA_REG16_R(SYS_FPGA_RESET_REG) | (1 << pdevFd->reset_bit)); // MR = HI;
  	sysUsDelay(10); 
    
  	FPGA_REG16_W(SYS_FPGA_RESET_REG, FPGA_REG16_R(SYS_FPGA_RESET_REG) & (~(1 << pdevFd->reset_bit))); // MR = LO;
  	sysMsDelay(10);
   
    FPGA_REG16_W(SYS_FPGA_RESET_REG, FPGA_REG16_R(SYS_FPGA_RESET_REG) | (1 << pdevFd->reset_bit)); // MR = HI;
   	sysUsDelay(10);   
   	return;
}

