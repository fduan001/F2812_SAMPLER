#include "F2812_datatype.h"
#include "shellconsole.h"
#include "fpga.h"
#include "role.h"

#define  ROLE_SWITCH_REG        (0x8 << 1)
#define  ROLE_WD_CTRL_REG       (0x9 << 1)
#define  ROLE_WD_SET_REG        (0xa << 1)

void MateIsrHandler(void* args);

UINT8 GetSlotID(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);

	return (val >> 4);
}

int IsMatePresent(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);
	if( (val & ( 1<< 0)) ) {
		return 1;
	}

	return 0;
}

int GetMyRole(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);
	if( (val & ( 1<< 3)) ) {
		return I_AM_MASTER;
	}

	return I_AM_SLAVE;
}

int GetMateRole(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);
	if( (val & ( 1<< 1)) ) {
		return I_AM_MASTER;
	}

	return I_AM_SLAVE;
}

int PutMeOnline(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);
	val |= ( 1 << 2);
	FPGA_REG16_W(FPGA_ROLE_SWITCH_REG, val);
	return 0;
}

int SetMyRole(int role) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_ROLE_SWITCH_REG);
	if( role == I_AM_MASTER ) {
		val |= ( 1 << 3);
	} else {
		val &= (UINT16)~( 1 << 3);
	}
	FPGA_REG16_W(FPGA_ROLE_SWITCH_REG, val);
	return 0;
}

int RoleInit(void) {

	FPGA_REG16_W(FPGA_XINT2_MASK_REG, 0x0); /* disable the interrupt */
	PutMeOnline();
	SetMyRole(I_AM_SLAVE);

	if( GetSlotID() == 1 ) {
		if( GetMateRole() != I_AM_MASTER ) {
			SetMyRole(I_AM_MASTER);
		}
	} else {
		if( !IsMatePresent() ) {
			SetMyRole(I_AM_MASTER);
		}
	}

	/* register the mate card interrupt handler */
	RegisterRoleIsr(MateIsrHandler);
	/* enable the interrupt */
	FPGA_REG16_W(FPGA_XINT2_STATUS_REG, 0x1); /* w1c to clear if exist */
    FPGA_REG16_W(FPGA_XINT2_MASK_REG, 0x1); /* enable the interrupt */

	return 0;
}

void ResetMateCard(void) {
	UINT16 val = 0;
	val = FPGA_REG16_R(FPGA_PERIPHERAL_RST_REG);
	val &= ~(1 << 9);
	FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, val);
	PlatformDelay(5);
	val |= (1 << 9);
	FPGA_REG16_W(FPGA_PERIPHERAL_RST_REG, val);
	return 0;
}

void MateIsrHandler(void* args) {
	SetMyRole(I_AM_MASTER);
}

int RoleSwitch(void) {
	if( GetMyRole() != I_AM_MASTER ) {
		PRINTF("I AM NOT MASTER, ABORT THIS REQUEST\n");
		return 0;
	}

	SetMyRole(I_AM_SLAVE);
	return 0;
}



