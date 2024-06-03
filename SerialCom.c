/* Includes --------------------------------------------------------------- */
#include <string.h>
#include <stdio.h>
#include "SerialCom.h"
//---------------------------------------------------------------------------/

/**************************************************************************
*
*                          SerialComOpenPort(......)
*
***************************************************************************/
BOOL SerialComOpenPort(const char *portname)
{
hComm = CreateFile(portname,
                      GENERIC_READ | GENERIC_WRITE,
                      0,
                      0,
                      OPEN_EXISTING,
                      0,
                      0);
		if(hComm==INVALID_HANDLE_VALUE){
		return false;
		}
		else return true;
}

/**************************************************************************
*
*                         SerialComSetDSR(......)
*
***************************************************************************/
BOOL SerialComSetDSR()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fOutxDsrFlow = 1;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                          SerialComClearDSR(......)
*
***************************************************************************/
BOOL SerialComClearDSR()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fOutxDsrFlow = 0;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                           SerialComSetDTR(......)
*
***************************************************************************/
BOOL SerialComSetDTR()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
      m_dcb.fDtrControl = 1;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                           SerialComClearDTR(......)
*
***************************************************************************/
BOOL SerialComClearDTR()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
      m_dcb.fDtrControl = 0;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}

/**************************************************************************
*
*                          SerialComSetCTS(......)
*
***************************************************************************/
BOOL SerialComSetCTS()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fOutxCtsFlow = 1;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                            SerialComClearDTR(......)
*
***************************************************************************/
BOOL SerialComClearCTS()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fOutxCtsFlow = 0;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}

/**************************************************************************
*
*                           SerialComSetCTS(......)
*
***************************************************************************/
BOOL SerialComSetRTS()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fRtsControl  = 1;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                           SerialComClearDTR(......)
*
***************************************************************************/
BOOL SerialComClearRTS()
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

   // setup hardware flow control
     m_dcb.fRtsControl  = 0;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/**************************************************************************
*
*                         SerialComConfigurePort(......)
*
***************************************************************************/
BOOL SerialComConfigurePort(DWORD BaudRate ,BYTE ByteSize, BYTE Parity, BYTE StopBits)
{
    DCB m_dcb;
    
   // purge any information in the buffer
    PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT |PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
	m_dcb.DCBlength = sizeof( DCB );
	if(!GetCommState(hComm, &m_dcb))
	{
		printf("GetCommState Error");
		CloseHandle(hComm);
	    return false;
	}

m_dcb.BaudRate = BaudRate;
m_dcb.ByteSize = ByteSize;
m_dcb.Parity   = Parity ;
m_dcb.StopBits = StopBits;
m_dcb.fBinary  = TRUE;
m_dcb.fParity  = 0;

   // setup hardware flow control
m_dcb.fOutxDsrFlow = 0;
m_dcb.fDtrControl  = 0;
m_dcb.fOutxCtsFlow = 0;
m_dcb.fRtsControl  = 0;

   // setup software flow control
m_dcb.fInX	   = m_dcb.fOutX = 0 ;
m_dcb.XonChar  = ASCII_XON  ;
m_dcb.XoffChar = ASCII_XOFF ;
m_dcb.XonLim   =  m_dcb.XoffLim = 100 ;

     if(!SetCommState(hComm, &m_dcb))
	 {
		printf("SetCommState Error");
		CloseHandle(hComm);
	    return false;
	 }

     return true;
}
/*****************************************************************************
*
*                       SerialComProperties(int code)
*
******************************************************************************/
unsigned char SerialComProperties(int code)
{
 unsigned char rs232status;
 DWORD     ModemStat=0 ;
 LPDWORD   lpModemStat;
 lpModemStat = &ModemStat;

	switch (code)
	{
    case 0:  EscapeCommFunction(hComm , CLRDTR ); rs232status &= (BIT_DTR ^ 0xFF);  break;
    case 1:  EscapeCommFunction(hComm , SETDTR ); rs232status |= BIT_DTR;           break; 
    case 10: EscapeCommFunction(hComm , CLRRTS ); rs232status &= (BIT_RTS ^ 0xFF);  break;
    case 11: EscapeCommFunction(hComm , SETRTS ); rs232status |= BIT_RTS;           break; 
	}

    if(GetCommModemStatus(hComm, lpModemStat))
	{
	        rs232status |= ((unsigned char) ModemStat & MS_CTS_ON);
	}

    return(rs232status);
}
/*****************************************************************************
*
*                       SerialComWriteByte(BYTE bybyte)
*
******************************************************************************/
BOOL SerialComSetCommunicationTimeouts(DWORD ReadIntervalTimeout, DWORD ReadTotalTimeoutMultiplier, DWORD ReadTotalTimeoutConstant, DWORD WriteTotalTimeoutMultiplier, DWORD WriteTotalTimeoutConstant)
{
HANDLE m_bPortReady;
COMMTIMEOUTS m_CommTimeouts;
if((m_bPortReady = GetCommTimeouts (hComm, &m_CommTimeouts))==0)
   return false;
m_CommTimeouts.ReadIntervalTimeout =ReadIntervalTimeout;
m_CommTimeouts.ReadTotalTimeoutConstant =ReadTotalTimeoutConstant;
m_CommTimeouts.ReadTotalTimeoutMultiplier =ReadTotalTimeoutMultiplier;
m_CommTimeouts.WriteTotalTimeoutConstant = WriteTotalTimeoutConstant;
m_CommTimeouts.WriteTotalTimeoutMultiplier =WriteTotalTimeoutMultiplier;

		m_bPortReady = SetCommTimeouts (hComm, &m_CommTimeouts);
		if(m_bPortReady ==0){
        printf("StCommTimeouts function failed");
		CloseHandle(hComm);
		return false;}
		return true;

}
/****************************************************************************
*
*                       SerialComWrite(BYTE bybyte)
*
*****************************************************************************/
BOOL SerialComWrite(BYTE *Data ,DWORD Len ,DWORD delay)
{
int iCount;
	for(iCount=0;iCount<Len;iCount++)
	{
		if(!SerialComWriteByte(Data[iCount]))return false;
		Sleep(delay);
	}
    return Len;
}
/****************************************************************************
*
*                       SerialComWriteByte(BYTE bybyte)
*
*****************************************************************************/
BOOL SerialComWriteByte(BYTE bybyte)
{
    byte iBytesWritten=0;
    if(!WriteFile(hComm,&bybyte,1,&iBytesWritten,NULL))  return false;
    else return true;
}
/****************************************************************************
*
*                      _SerialComReadByte(BYTE	*resp , DWORD Len)
*
*****************************************************************************/
BOOL SerialComRead(BYTE  *Data ,DWORD Len)
{
unsigned char rByte;
int iCount;

	for(iCount=0;iCount<Len;iCount++)
	{
		if(SerialComReadByte(&rByte))
		{
			Data[iCount]=rByte;
		}
		else return false;
	}
	 return Len;
}
/****************************************************************************
*
*                      _SerialComReadByte(BYTE	*resp , DWORD Len)
*
*****************************************************************************/
BOOL SerialComReadByte(BYTE *Data)
{
int iCount;

	for(iCount=0;iCount<sTIMEOUT;iCount++)
	{
		if(_SerialComReadByte(Data)) return true;
	}
	 return false;
}
/****************************************************************************
*
*                           SerialComReadByte(BYTE *resp)
*
*****************************************************************************/
BOOL _SerialComReadByte(BYTE  *resp)
{
    BYTE rx=0;
    DWORD dwBytesTransferred=0;
    DWORD dwErrorFlags;
    COMSTAT    ComStat ;
    resp[0]=0;

     	// only try to read number of bytes in queue
        if(ReadFile(hComm, &rx, 1, &dwBytesTransferred ,0))
		{
			 if (dwBytesTransferred==1)
			 {
				 resp[0]=rx;
				 return true;
			 }
		}	  

     	return false;
}
/****************************************************************************
*
*                                Flush(DWORD dwFlag)
*
*****************************************************************************/
BOOL Flush(DWORD dwFlag)
{
	if(PurgeComm(hComm, dwFlag)) return true;
	else return false;
}
/****************************************************************************
*
*                                SerialComClosePort()
*
*****************************************************************************/
void SerialComClosePort()
{
   // purge any outstanding reads/writes and close device handle
   PurgeComm(hComm, PURGE_TXABORT | PURGE_RXABORT | PURGE_TXCLEAR | PURGE_RXCLEAR ) ;
   CloseHandle(hComm);
   return;
}

