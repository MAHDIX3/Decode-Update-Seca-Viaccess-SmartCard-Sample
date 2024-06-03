#include <windows.h>

#ifndef BYTE
  #define BYTE unsigned char
#endif

#ifndef byte
  #define byte unsigned char
#endif

#ifndef bool
  #define bool int
#endif

#ifndef true
	#define false 0
	#define true (!false)
#endif	/* if TRUE not already defined */
//---------------------------------------------------------------------------
#ifndef SerialComH
	#define SerialComH
//---------------------------------------------------------------------------
/* Baud Rate 
CBR_110
CBR_300
CBR_600
CBR_1200
CBR_2400
CBR_4800
CBR_9600
CBR_14400
CBR_19200
CBR_38400
CBR_56000
CBR_57600
CBR_115200
CBR_128000
CBR_256000
*/

/* Parity
EVENPARITY
MARKPARITY
NOPARITY
ODDPARITY
SPACEPARITY
*/

/* StopBit
ONESTOPBIT
ONE5STOPBITS
TWOSTOPBITS
*/

// Flow control flags
#define FC_DTRDSR       0x01
#define FC_RTSCTS       0x02
#define FC_XONXOFF      0x04

// ascii definitions
#define ASCII_BEL       0x07
#define ASCII_BS        0x08
#define ASCII_LF        0x0A
#define ASCII_CR        0x0D
#define ASCII_XON       0x11
#define ASCII_XOFF      0x13

#define BIT_DCD   0x01        // Pin1 inp. Data Carrier detect 
#define BIT_DTR   0x08        // Pin4 out. Data Terminal Ready   \ Handshake 4 with 6 
#define BIT_DSR   0x20        // Pin6 inp. Data Set Ready        /
#define BIT_RTS   0x04        // Pin7 out  Ready To Sent    \  Handshake 7 with 8  
#define BIT_CTS	  0x40 	      // Pin8 inp. Clear To Send    /
#define BIT_RI    0x80        // Pin9 inp.

#define sTIMEOUT 100
//---------------------------------------------------------------------------
HANDLE hComm;
BOOL SerialComOpenPort(const char *portname);
BOOL SerialComSetDSR();
BOOL SerialComClearDSR();
BOOL SerialComSetDTR();
BOOL SerialComClearDTR();
BOOL SerialComSetCTS();
BOOL SerialComClearCTS();
BOOL SerialComSetRTS();
BOOL SerialComClearRTS();
BOOL SerialComConfigurePort(DWORD BaudRate, BYTE ByteSize, BYTE Parity, BYTE StopBits);
unsigned char SerialComProperties(int code);
BOOL SerialComSetCommunicationTimeouts(DWORD ReadIntervalTimeout, DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant);
BOOL SerialComWrite(BYTE *Data ,int Len ,DWORD delay);
BOOL SerialComWriteByte(BYTE bybyte);
BOOL SerialComRead(BYTE  *Data ,DWORD Len);
BOOL SerialComReadByte(BYTE *Data);
BOOL _SerialComReadByte(BYTE  *resp);
BOOL Flush(DWORD dwFlag);
void SerialComClosePort();
//---------------------------------------------------------------------------
#endif

