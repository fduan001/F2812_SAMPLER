Driver Verification Command:
1. F-RAM
   It needs to do initialization before any FRAM operation
   a). initialization
      > fram init
      Note: it will initialization the DSP SPI controller
   b). read from FRAM
   		> fram read #offset [#length]
   		ie: > fram read  0x10
   			Note: read 1 byte from offset 0x10
   		ie: > fram read  0x10   0x1 
   			Note: read 1 byte from offset 0x10
   		ie: fram read 0x20  0x5
   			Mote: read 5 bytes from offset 0x20
   c). write to FRAM
   		> fram write #offset #value0 #value1 #value2
   		ie: fram write 0x10 0x5a
   			Note: write 0x5a to offset 0x10
   		ie: fram write 0x20  0x5a 0x67 0x78 0x89
   			Note: write [ 0x5a 0x67 0x78 0x89 ] to offset [ 0x20 0x21 0x22 0x23 ]

2. AD5684
   It need to do initialization before any ad568x operations.
   a). initialization
      ad568x init
   b). set channel output volatge
      > ad568x  chan   vout  vref
      	Note: chan: 1 - channel A
                  	2 - channel B
                  	4 - channel C
                  	8 - channel D
            	vout: expected voltage output for specified channel, unit is mV
            	vref: volatge reference, designed by hardware, fix-ed with 5000 mV in current
      	ie: ad568x setvol  1   2456  5000
      		Note: set channel A's volatge output to 2456 mV. (vref is 5000)
      	ie: ad568x setvol  8   1700 5000
      		Note: set channel D's volatge to 1.7 V. (vref is 5000)
   c). dump dac register value
      > ad568x rdbk  channle
      	ie: ad568x rdbk   1
      			Note: read the DAC register from channel 1

3. ADS124S08
   It need to do initialization before any ads124s08 operations.
   a). initialization
       > ads124 init
   b). read register
       > ads124 rr #reg
       ie: ads124 rr 0x3
       		Note: read register 0x3
   c). write register
       > ads124 wr  #reg  #value
       ie: ads124 wr 0x3 0x22
       		Note: write 0x22 into register 0x3
