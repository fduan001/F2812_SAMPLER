
#ifndef _EEPROM_H_
#define _EEPROM_H_


int eeprom_init(int i2c_bus_num);
int eeprom_read(unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);
int eeprom_write (unsigned dev_addr, unsigned offset, uchar *buffer, unsigned cnt);


#endif

