#include "SerialCom.h"
#include "SmartCard.h"
#include <string.h>
#include <stdio.h>
#include "scSeca.h"
#include "scviaccess.h"
//------Sample Code for decode and update SECA , Viaccess SmartCard by Mahdi Zakariyazadeh (2010)-------/
void dump(unsigned char *adr, int l)
{
	int i;
	for(i=0; i < l; i++){
	 	printf("%02X ",adr[i]);
	    if ((i&0xF)==0xF) printf("\n");
	}printf("\n");
}
//---------------------------------------------------------------------------------
int SetSerialPhonix(int SM_MODE)
{
	if(SerialComOpenPort("COM1"))
	{
         switch(SM_MODE) 
		 {
           case SM_8E2: 
	              if(SerialComConfigurePort(CBR_9600 ,8 ,EVENPARITY ,TWOSTOPBITS)){
		            	if(SerialComSetCommunicationTimeouts(1,1,1,0,0)) return true;
				  }
                break;
           case SM_8N2:
	              if(SerialComConfigurePort(CBR_9600 ,8 ,NOPARITY ,TWOSTOPBITS)){
		            	if(SerialComSetCommunicationTimeouts(1,1,1,0,0)) return true;
				  }
                break;
           case SM_8O2:
	              if(SerialComConfigurePort(CBR_9600 ,8 ,ODDPARITY ,TWOSTOPBITS)){
		            	if(SerialComSetCommunicationTimeouts(1,1,1,0,0)) return true;
				  }
                break;
          default:
			     printf("====Bad Mode====");
                 return false;
		 }
	}
	 return false;
}
//---main--------------------------------------------------------------------------
SecaCardInfo iSecaCard[0x1000];
ViaCardInfo  iViaCard[0x1000];

int main()
{

    unsigned char ecm[]={
		/*
 0x81,0x70,0x31,0x00,0x90,0x03,0x00,0x7C,0x08,0xE2,0x03,0x37
,0x91,0x07,0xE2,0x03,0x37,0x91,0x13,0xE2,0x03,0x37,0x91,0x38
,0xEA,0x10,0x2C,0x4F,0xD3,0x06,0x17,0x13,0xD5,0x5A,0x8C,0x02
,0x22,0x00,0x73,0x33,0xA2,0xDD,0xF0,0x08,0xC8,0x5B,0x28,0xD5
,0x51,0x02,0xD3,0x1B
/*/
 0x80,0x70,0x3B,0x00,0x90,0x03,0x02,0x44,0x08
,0xE2,0x03,0x37,0x8F,0xFE
,0xE2,0x03,0x37,0x8F,0xFF
,0xE2,0x03,0x37,0x8F,0xF4
,0xE2,0x03,0x37,0x8F,0xF3
,0xE2,0x03,0x37,0x8F,0xF5
,0xEA,0x10,0x79,0xCD,0x41,0xB3,0x6E,0xF3,0xA7,0x33,0x49,0x2F,0x4D,0x8E,0xEB,0x02,0x4C,0x09
,0xF0,0x08,0x09,0x2A,0x36,0x5F,0x8B,0xB8,0xF9,0xE8
/*
 0x81,0x00,0x2E,0x00,0x30,0x00,0x00,0x0C
,0x71,0x00,0x00,0x30,0x07,0xD7,0x00,0x0C,0x12,0x00,0x27
,0x23,0x8D,0x13,0x01,0xD1,0x72,0xC0,0x42,0x6F,0x60,0xA2,0x20,0xE7,0x03,0x6D,0x6A
,0x40,0xB6,0xE9,0x1F,0xD0,0x82,0x77,0x47,0xCB,0x1C,0xE1,0x7B,0x27,0x85*/
	};

    unsigned char emm[]={
 0x8D,0x00,0x40,
 //0x00,0x02,0x8C,0xB3,0x00,
 0x90,0x03,0x00,0x94,0x04,
 0x9E,0x20,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,
 0xA1,0x03,0x00,0x94,0x0E,
 0xEF,0x08,0xBF,0xC6,0xC8,0x21,0xCA,0x1B,0xC4,0x1B, 
 0xF0,0x08,0x4C,0xCA,0x12,0xA9,0xE1,0x9A,0x34,0x65

/*
 0x8D,0x70,0x14
,0x90,0x03,0x00,0x94,0x04
,0xA1,0x03,0x00,0x94,0x0E
,0xEF,0x08,0xC6,0x04,0x4F,0x8B,0xB6,0xDB,0x7E,0xFE
/*
 0x8E,0x70,0x2C,0xC2,0x8D,0xED,0x00,0x60,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x01
,0x04,0x01,0x00,0x00,0x00,0x01,0x00,0x00,0x00,0x00,0x00,0x00,0x02,0x00,0x00,0x00
,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x83,0xDB,0x1A,0xCC,0xF3,0x79,0x9B,0x40

	/*/ /*
 0x84,0x00,0x50,0x00,0x30,0x00,0x00,0x29,0x10,0x81,0x71,0x62,0x2B,0xE8,0x90,0xA7
,0x2B,0x54,0x88,0x84,0x08,0xDE,0xD4,0x43,0x5F,0x5C,0x88,0x84,0x08,0xDE,0xD4,0x43
,0x5F,0x5C,0x88,0x84,0x08,0xDE,0xD4,0x43,0x5F,0x5C,0x23,0xB5,0xF1,0x50,0xCE,0x98
,0x84,0xEB,0xCE,0x76,0xA5,0xC3,0x02,0x9F,0x4B,0x64,0x90,0x25,0x10,0x77,0xBA,0x4F
,0x9C,0x97,0xC9,0x19,0x71,0x30,0x23,0x75,0x63,0x26,0x01,0x08,0xC0,0x3F,0x94,0x12
,0x15,0xC0,0x85
/*

 0x82,0x00,0x5C,0x00,0x00,0x00,0xF2,0x8B,0xB5,0x00,0x30,0x10,0x80,0x43,0x8C,0xB8
,0x0D,0x43,0x73,0x81,0x6C,0x86,0x41,0xB1,0x54,0x1B,0x10,0x91,0xED,0xD0,0x91,0x89
,0xCA,0x8C,0x94,0xA3,0x1C,0x51,0x23,0x72,0xD8,0x41,0xCA,0x96,0xC7,0x3A,0x86,0x16
,0x18,0x42,0x70,0x60,0x0B,0xAB,0x7C,0x9B,0x61,0xCD,0x84,0x31,0x0C,0xD2,0x04,0xBF
,0x64,0x72,0x5B,0xFC,0xCF,0x23,0x6F,0x4D,0xEB,0x3B,0xB9,0xDF,0x7D,0x0F,0xAD,0x48
,0x4E,0xAE,0x76,0x7A,0x7B,0x56,0xA9,0x8B,0x4C,0x11,0x1B,0x4D,0x7A,0xDE,0xC2*/
	};

    unsigned char emm8E[]={
 0x8E,0x70,0x2C,0x02,0x8C,0xB3,0x00,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,
 0x4C,0xCA,0x12,0xA9,0xE1,0x9A,0x34,0x65
	};
    unsigned char emm8D[]={
 0x8D,0x70,0x14,0x90,0x03,0x00,0x94,0x04,0xA1,0x03,0x00,0x94,0x0E,0xEF,0x08,0xBF
,0xC6,0xC8,0x21,0xCA,0x1B,0xC4,0x1B
	};

    unsigned char ATR[32] ,Id[2] ,cw[0x29];
	int AtrLen ,i ,l;
	_atr cATR[100];

	FILE *in;
	FILE *out;
	unsigned char temp[255];
	unsigned char id[3]={0x00,0x94,0x00};

	//SetSerial
	if(!SetSerialPhonix(SM_8O2)){
		printf("Serial Port Error");
	}

	//Get Atr "Set & Clear RTS"
	SerialComSetRTS();
	Sleep(20);
	SerialComClearRTS();

	//Read ATR
	AtrLen = IsoReadAtr(ATR);
	printf("ATR:");dump(ATR , AtrLen);

	//Get ATR Info
    memcpy(cATR->atr ,ATR ,AtrLen); 
	cATR->atrLen=AtrLen;
	ParseAtr(cATR, 0, ISO_FREQ);

	cViaccessInit(cATR,iViaCard);
  //cViaccessUpdate(cATR,iViaCard,emm8E,id);
  //cViaccessUpdate(cATR,iViaCard,emm8D,id);
	if(cViaccessDecode(cATR ,iViaCard , ecm ,cw))
	{
       printf("\nDCW :" );dump(cw , 16);
	}

	/*
    if(cSecaInit(cATR,iSecaCard))
	{

		cSecaUpdate(cATR,iSecaCard,emm);
       // if(cSecaDecode(cATR,iSecaCard,ecm,cw))
		//{
        //   printf("\nDCW :" );dump(cw , 16);
		//}
	}
	/*/


	getch();
}
