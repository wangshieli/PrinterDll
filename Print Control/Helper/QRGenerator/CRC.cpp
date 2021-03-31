#include <stdio.h>
#include "CRC.h"


static void crc_byte(unsigned int onebyte,unsigned int *crc_reg)
{
 	int i;
    onebyte <<= 8;
    for(i=8;i>0;i--)
    {
       	if((onebyte^*crc_reg) & 0x8000 )
        	*crc_reg = (*crc_reg<<1)^0x8005;
		else
          	*crc_reg <<= 1;
      	onebyte <<= 1;
  	}
}

static void crcxdata(unsigned int length, unsigned char * buffer, unsigned char *crc)
{
  	unsigned int i;
    unsigned int CRC;

	CRC = 0;

    for(i=0;i<length;i++)
    {	
     	crc_byte(*buffer,&CRC);
     	buffer++;
    }

	CRC = CRC&0xFFFF;

	crc[0]= CRC >> 8;
	crc[1]= CRC & 0xff;
}

void  get_ewm_crc_str(unsigned int length, unsigned char * buffer, unsigned char *pcrcstr)
{
	unsigned char crc[4];
	crcxdata(length,buffer,crc);
	if(((crc[0]&0xf0) >> 4) > 9 )
	{
		pcrcstr[0]=('A'+(((crc[0]&0xf0) >> 4)-10));
	}
	else
	{
		pcrcstr[0]=('0'+((crc[0]&0xf0) >> 4));
	}
	
	if((crc[0]&0x0f) >9)
	{
		pcrcstr[1]=('A'+((crc[0]&0x0f)-10));
	}
	else
	{
		pcrcstr[1]=('0'+(crc[0]&0x0f));
	}

	if(((crc[1]&0xf0) >> 4) > 9)
	{
		pcrcstr[2]=('A'+(((crc[1]&0xf0) >> 4)-10));
	}
	else
	{
		pcrcstr[2]=('0'+((crc[1]&0xf0) >> 4));

	}
	if((crc[1]&0x0f)  >9)
	{
		pcrcstr[3]=('A'+((crc[1]&0x0f)-10));
	}
	else
	{
		pcrcstr[3]=('0'+(crc[1]&0x0f));
	}
	pcrcstr[4]='\0';
}