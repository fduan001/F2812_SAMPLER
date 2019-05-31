/***************************************************************************//**
 *   @file   AD568X.c
 *   @brief  Implementation of AD568X Driver.
 *   @author Dan Nechita
********************************************************************************
 * Copyright 2012(c) Analog Devices, Inc.
 *
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 *  - Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 *  - Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *  - Neither the name of Analog Devices, Inc. nor the names of its
 *    contributors may be used to endorse or promote products derived
 *    from this software without specific prior written permission.
 *  - The use of this software may or may not infringe the patent rights
 *    of one or more patent holders.  This license does not release you
 *    from the requirement that you obtain separate licenses from these
 *    patent holders to use this software.
 *  - Use of the software either in source or binary form, must be run
 *    on or directly connected to an Analog Devices Inc. component.
 *
 * THIS SOFTWARE IS PROVIDED BY ANALOG DEVICES "AS IS" AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, NON-INFRINGEMENT,
 * MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL ANALOG DEVICES BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 * LIMITED TO, INTELLECTUAL PROPERTY RIGHTS, PROCUREMENT OF SUBSTITUTE GOODS OR
 * SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER
 * CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY,
 * OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 * OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
********************************************************************************
*   SVN Revision: 775
*******************************************************************************/

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "F2812_datatype.h"
#include "shellconsole.h"
#include "AD568X.h"		// AD568X definitions.
#include "fpga.h"

/******************************************************************************/
/***************************** Include Files **********************************/
/******************************************************************************/
#include "Communication.h"	// Communication definitions.

/******************************************************************************/
/************************ Variables Definitions *******************************/
/******************************************************************************/
UINT8 currentPowerRegValue = 0; 
UINT8 deviceBitsNumber     = 0;

/******************************************************************************/
/************************ Functions Definitions *******************************/
/******************************************************************************/

void AD568X_HWReset(void) {
    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) | (1 << 1)); // MR = HI;
    PlatformDelay(5); 

    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) & (~(1 << 1))); // MR = LO;
    PlatformDelay(5);

    FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG) | (1 << 1)); // MR = HI;
    PlatformDelay(5); 
}

/***************************************************************************//**
 * @brief Initializes the device.
 *
 * @param ad568x - Device version.
 *                 Example: AD5684  - 12-bit DAC(no internal vRef).
 *                          AD5686  - 16-bit DAC(no internal vRef).
 *                          AD5684R - 12-bit DAC(with internal vRef).
 *                          AD5685R - 14-bit DAC(with internal vRef).
 *                          AD5685R - 16-bit DAC(with internal vRef).
 *
 * @return status - Result of the initialization procedure.
 *					Example: 0x0 - SPI peripheral was not initialized.
 *				  			 0x1 - SPI peripheral is initialized.
*******************************************************************************/
UINT8 AD568X_Init(UINT8 ad568x)
{
    UINT8 status = 0;
    AD568X_HWReset();
#if 0
    /* GPIO configuration. */
    AD568X_LDAC_LOW;
    AD568X_LDAC_OUT;
    AD568X_RESET_HIGH;
    AD568X_RESET_OUT;
    AD568X_RSTSEL_LOW;
    AD568X_RSTSEL_OUT;
    AD568X_GAIN_LOW;
    AD568X_GAIN_OUT;
#endif
    status = SPI_Init(0,        // Transfer format.
                      8000,  // SPI clock frequency.
                      1,        // SPI clock polarity.
                      1);       // SPI clock edge.
    if((ad568x == AD5686) || (ad568x == AD5686R))
    {
        deviceBitsNumber = 16;
    }
    else if(ad568x == AD5685R)
    {
        deviceBitsNumber = 14;
    }
    else
    {
        deviceBitsNumber = 12;
    }
    
    return status;
}

/***************************************************************************//**
 * @brief Puts the device in a specific power mode.
 *
 * @param channel - Channel option.
 *                  Example: AD568X_ADDR_DAC_A
 *                           AD568X_ADDR_DAC_B
 *                           AD568X_ADDR_DAC_C
 *                           AD568X_ADDR_DAC_D
 * @param pwrMode - Power mode of the device.
 *                  Example: AD568X_PD_NORMAL   - Normal operation
 *                           AD568X_PD_1K       - 1 kOhm to GND
 *                           AD568X_PD_100K     - 100 kOhm to GND
 *                           AD568X_PD_3STATE   - Three-state
 *                               
 *
 * @return none.
*******************************************************************************/
void AD568X_PowerMode(UINT8 channel, UINT8 pwrMode)
{    
    switch(channel)
    {
        case AD568X_ADDR_DAC_A:
            currentPowerRegValue &= ~AD568X_PWR_PDA(0x3);   // Clear power bits
            currentPowerRegValue |= AD568X_PWR_PDA(pwrMode);
            break;
        case AD568X_ADDR_DAC_B:
            currentPowerRegValue &= ~AD568X_PWR_PDB(0x3);
            currentPowerRegValue |= AD568X_PWR_PDB(pwrMode);
            break;
        case AD568X_ADDR_DAC_C:
            currentPowerRegValue &= ~AD568X_PWR_PDC(0x3);
            currentPowerRegValue |= AD568X_PWR_PDC(pwrMode);
            break;
        case AD568X_ADDR_DAC_D:
            currentPowerRegValue &= ~AD568X_PWR_PDD(0x3);
            currentPowerRegValue |= AD568X_PWR_PDD(pwrMode);
            break;
    }
    AD568X_SetInputRegister(AD568X_CMD_POWERMODE, channel, AD568X_PWR_PDD(pwrMode));
}

/***************************************************************************//**
 * @brief Resets the device(clears the outputs to either zero scale or 
          midscale).
 *
 * @param resetOutput - The output values of the device at power-up or reset.
 *                      Example: 
 *                    AD568X_RST_ZERO_SCALE - outputs are cleared to zero scale.
 *                    AD568X_RST_MIDSCALE   - outputs are cleared to midscale.
 *
 * @return none.
*******************************************************************************/
void AD568X_Reset(UINT8 resetOutput)
{
	AD568X_HWReset();
    AD568X_SetInputRegister(AD568X_CMD_SOFT_RESET, 0, 0);
}

/***************************************************************************//**
 * @brief Writes a 24-bit data-word to the Input Register of the device.
 *
 * @param registerValue - Value of the register.
 *
 * @return none.
*******************************************************************************/
void AD568X_SetInputRegister(UINT8 cmd, UINT8 chan, UINT16 data)
{
    UINT8 registerWord[3] = {0, 0, 0};

    registerWord[0] = (cmd << 4) | chan;
    registerWord[1] = (data >> 8) & 0xFF;
    registerWord[2] = data & 0xFF;
    
    SPI_Write(AD568X_SLAVE_ID, registerWord, 3);
}

/***************************************************************************//**
 * @brief Select internal or external voltage reference.
 *
 * @param vRefMode - Voltage reference option.
 *                   Example: AD568X_INT_REF_ON  - Internal reference is used.
 *                            AD568X_INT_REF_OFF - External reference is used.
 *
 * @return none.
*******************************************************************************/
void AD568X_InternalVoltageReference(UINT8 vRefMode)
{
    AD568X_SetInputRegister(AD568X_CMD_INT_REF_SETUP, 0, vRefMode);
}

/***************************************************************************//**
 * @brief Write data to the Input Register or to DAC Register of a channel.
 *
 * @param writeCommand - The write command option.
 *                       Example: 
*                        AD568X_CMD_WR_INPUT_N    - Write to Input Register n.
 *                       AD568X_CMD_WR_UPDT_DAC_N - Write to and update DAC n.
 * @param channel - Channel option.
 *                  Example: AD568X_ADDR_DAC_A
 *                           AD568X_ADDR_DAC_B
 *                           AD568X_ADDR_DAC_C
 *                           AD568X_ADDR_DAC_D
 * @param data -  Data value to write.
 *
 * @return none.
*******************************************************************************/
void AD568X_WriteFunction(UINT8 writeCommand, 
                          UINT8 channel, 
                          UINT16 data)
{
    UINT8 shiftValue = 0;
    
    /* Different types of devices have different data bits positions. */
    shiftValue = 16 - deviceBitsNumber;
    AD568X_SetInputRegister(writeCommand, channel, ((UINT16)AD568X_DATA_BITS(data) << shiftValue));
}

/***************************************************************************//**
 * @brief Reads back the binary value written to one of the channels.
 *
 * @param dacChannelAddr - Channel address.
 *                         Example: AD568X_ADDR_DAC_A
 *                                  AD568X_ADDR_DAC_B
 *                                  AD568X_ADDR_DAC_C
 *                                  AD568X_ADDR_DAC_D 
 *
 * @return 12-bit value of the selected channel.
*******************************************************************************/
UINT16 AD568X_ReadBack(UINT8 dacChannelAddr)
{
    unsigned long channelValue = 0;
    UINT8 shiftValue   = 0;
    UINT8 rxBuffer[3]  = {0, 0, 0};
    UINT32 val = 0;
    val = (UINT32)AD568X_CMD((UINT32)AD568X_CMD_SET_READBACK);
    val |= (UINT32)AD568X_ADDR((UINT32)dacChannelAddr);
    
    /* Different types of devices have different data bits positions. */
    shiftValue = 16 - deviceBitsNumber;
    AD568X_SetInputRegister(AD568X_CMD_SET_READBACK, dacChannelAddr, 0);
    
    SPI_Read(AD568X_SLAVE_ID, rxBuffer, 3);
    PRINTF("rdbk: 0x%02x 0x%02x 0x%02x\n", rxBuffer[0], rxBuffer[1], rxBuffer[2]);
    
    channelValue = ((UINT16)rxBuffer[1] << 8) | rxBuffer[2];
    channelValue >>= shiftValue;
    
    return channelValue;
}

/***************************************************************************//**
 * @brief Selects the output voltage of the selected channel.
 *
 * @param channel - Channel option.
 *                  Example: AD568X_ADDR_DAC_A
 *                           AD568X_ADDR_DAC_B
 *                           AD568X_ADDR_DAC_C
 *                           AD568X_ADDR_DAC_D
 * @param outputVoltage - Output voltage value.
 * @param vRef - Value of the voltage reference used. If GAIN pin is tied to Vdd
 *               vRef value is multiplied by 2 inside this function.
 *
 * @return The actual value of the output voltage.
*******************************************************************************/
float AD568X_SetVoltage(UINT8 channel, 
                        float outputVoltage, 
                        float vRef)
{
    UINT16 binaryValue   = 0;
    float          actualVoltage = 0;
    
    if(vRef == 0)
    {
        vRef = 5.0;
    }
#if 0
    vRef *= (AD568X_GAIN_STATE != 0) ? 2 : 1;
#endif
    // vRef *= 2;

    binaryValue = (UINT16)(outputVoltage * (1 << deviceBitsNumber) / 
                                  vRef);
    AD568X_WriteFunction(AD568X_CMD_WR_UPDT_DAC_N, channel, binaryValue);
    actualVoltage = (float)(vRef * binaryValue) / (1 << deviceBitsNumber);
    
    return actualVoltage;
}
