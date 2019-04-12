#ifndef  FPGA_H
#define  FPGA_H

#define FPGA_VER_YEAR_REG                               0x0
#define FPGA_VER_DATE_REG                               0x1
#define FPGA_SELF_TEST_REG                              0x2

/* master/slave role switch */
#define FPGA_ROLE_SWITCH_REG                            0x4

/* peripherals reset */
#define FPGA_PERIPHERAL_RST_REG                         0x5

/* interrupt registers */
#define FPGA_XINT1_STATUS_REG                           0x6
#define FPGA_XINT1_MASK_REG                             0x7

#define FPGA_XINT1_STATUS_REG                           0x8
#define FPGA_XINT1_MASK_REG                             0x9

/* IO control */
#define FPGA_IO_STATUS_REG                              0xA
#define FPGA_IO_SELF_TEST_REG                           0xB

/* gas mearsument */
#define FPGA_GAS_SWITCH_REG                        0x10
#define FPGA_GAS_MEAS_REG                          0x11
#define FPGA_GAS_STATUS_REG                        0x12

#define FPGA_GAS_TUNNEL1_RESULT_H_REG              0x13
#define FPGA_GAS_TUNNEL1_RESULT_L_REG              0x14
#define FPGA_GAS_TUNNEL2_RESULT_H_REG              0x15
#define FPGA_GAS_TUNNEL2_RESULT_L_REG              0x16
#define FPGA_GAS_TUNNEL3_RESULT_H_REG              0x17
#define FPGA_GAS_TUNNEL3_RESULT_L_REG              0x18
#define FPGA_GAS_TUNNEL4_RESULT_H_REG              0x19
#define FPGA_GAS_TUNNEL4_RESULT_L_REG              0x1A

/* flow measurment */
#define FPGA_SPI_AS5684_TX_DATA_REG                    0x20
#define FPGA_SPI_AS5684_RX_DATA_REG                    0x20
#define FPGA_SPI_AS5684_CTRL_REG                       0x21
#define FPGA_SPI_AS5684_CLK_REG                        0x22
#define FPGA_SPI_AS5684_SS_REG                         0x23

#define FPGA_FLOW_MEAS_SWITCH_REG                      0x30
#define FPGA_FLOW_MEAS_CTRL_REG                        0x31
#define FPGA_FLOW_MEAS_STATUS_REG                      0x32
#define FPGA_FLOW_TUNNEL1_RESULT_H_REG                 0x33
#define FPGA_FLOW_TUNNEL1_RESULT_L_REG                 0x34
#define FPGA_FLOW_TUNNEL2_RESULT_H_REG                 0x35
#define FPGA_FLOW_TUNNEL2_RESULT_L_REG                 0x36
#define FPGA_FLOW_TUNNEL3_RESULT_H_REG                 0x37
#define FPGA_FLOW_TUNNEL3_RESULT_L_REG                 0x38
#define FPGA_FLOW_TUNNEL4_RESULT_H_REG                 0x39
#define FPGA_FLOW_TUNNEL4_RESULT_L_REG                 0x3A

/* temperature measument */
#define FPGA_SPI_ADS1248_TX_DATA_REG                    0x40
#define FPGA_SPI_ADS1248_RX_DATA_REG                    0x40
#define FPGA_SPI_ADS1248_CTRL_REG                       0x41
#define FPGA_SPI_ADS1248_CLK_REG                        0x42
#define FPGA_SPI_ADS1248_SS_REG                         0x43

#define FPGA_TEMP_MEAS_CTRL_REG                         0x50
#define FPGA_TEMP_MEAS_STATUS_REG                       0x51

/* RS422 */
#define FPGA_RS422_TRX1_RX_REG                               0x60
#define FPGA_RS422_TRX1_TX_REG                               0x60

#define FPGA_RS422_TRX1_LSB_REG                              0x60
#define FPGA_RS422_TRX1_MSB_REG                              0x61
#define FPGA_RS422_TRX1_INTR_CTRL_REG                        0x61
#define FPGA_RS422_TRX1_INTR_STATUS_REG                      0x62
#define FPGA_RS422_TRX1_FIFO_CTRL_REG                        0x62
#define FPGA_RS422_TRX1_LCR_REG                              0x63
#define FPGA_RS422_TRX1_LSR_REG                              0x65

#define FPGA_RS422_TRX2_RX_REG                               0x70
#define FPGA_RS422_TRX2_TX_REG                               0x70

#define FPGA_RS422_TRX2_LSB_REG                              0x70
#define FPGA_RS422_TRX2_MSB_REG                              0x71
#define FPGA_RS422_TRX2_INTR_CTRL_REG                        0x71
#define FPGA_RS422_TRX2_INTR_STATUS_REG                      0x72
#define FPGA_RS422_TRX2_FIFO_CTRL_REG                        0x72
#define FPGA_RS422_TRX2_LCR_REG                              0x73
#define FPGA_RS422_TRX2_LSR_REG                              0x75

#define FPGA_RS422_TRX3_RX_REG                               0x80
#define FPGA_RS422_TRX3_TX_REG                               0x80

#define FPGA_RS422_TRX3_LSB_REG                              0x80
#define FPGA_RS422_TRX3_MSB_REG                              0x81
#define FPGA_RS422_TRX3_INTR_CTRL_REG                        0x81
#define FPGA_RS422_TRX3_INTR_STATUS_REG                      0x82
#define FPGA_RS422_TRX3_FIFO_CTRL_REG                        0x82
#define FPGA_RS422_TRX3_LCR_REG                              0x83
#define FPGA_RS422_TRX3_LSR_REG                              0x85

#define FPGA_RS422_TRX4_RX_REG                               0x90
#define FPGA_RS422_TRX4_TX_REG                               0x90

#define FPGA_RS422_TRX4_LSB_REG                              0x90
#define FPGA_RS422_TRX4_MSB_REG                              0x91
#define FPGA_RS422_TRX4_INTR_CTRL_REG                        0x91
#define FPGA_RS422_TRX4_INTR_STATUS_REG                      0x92
#define FPGA_RS422_TRX4_FIFO_CTRL_REG                        0x92
#define FPGA_RS422_TRX4_LCR_REG                              0x93
#define FPGA_RS422_TRX4_LSR_REG                              0x95

#define FPGA_RS422_TRX5_RX_REG                               0xA0
#define FPGA_RS422_TRX5_TX_REG                               0xA0

#define FPGA_RS422_TRX5_LSB_REG                              0xA0
#define FPGA_RS422_TRX5_MSB_REG                              0xA1
#define FPGA_RS422_TRX5_INTR_CTRL_REG                        0xA1
#define FPGA_RS422_TRX5_INTR_STATUS_REG                      0xA2
#define FPGA_RS422_TRX5_FIFO_CTRL_REG                        0xA2
#define FPGA_RS422_TRX5_LCR_REG                              0xA3
#define FPGA_RS422_TRX5_LSR_REG                              0xA5

#ifdef __cplusplus
extern  "C" {
#endif

void WriteFpgaRegister( UINT32 regaddr, UINT16 regvalue);
UINT16 ReadFpgaRegister( UINT32 regaddr );
void WriteFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue);
UINT8 ReadFpgaRegisterBit(UINT32 regaddr, UINT8 bitpos, UINT8 bitvalue);

#ifdef __cplusplus
}
#endif

#endif