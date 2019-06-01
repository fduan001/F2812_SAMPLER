#ifndef ROLE_H
#define ROLE_H

typedef enum ROLE_T
{
	I_AM_MASTER = 1,
	I_AM_SLAVE,
	I_AM_UNKNOWN,
} ROLE_T;

void ResetMateCard(void);
int RoleInit(void);
UINT8 GetSlotID(void);
int IsMatePresent(void);
int GetMyRole(void);
int GetMateRole(void);
int PutMeOnline(void);
int SetMyRole(int role);
int RoleSwitch(void);

#endif
