#ifndef __CRC_H__
#define __CRC_H__


static void crc_byte(unsigned int onebyte,unsigned int *crc_reg);

static void crcxdata(unsigned int length, unsigned char * buffer, unsigned char *crc);

void  get_ewm_crc_str(unsigned int length, unsigned char * buffer, unsigned char *pcrcstr);

#endif