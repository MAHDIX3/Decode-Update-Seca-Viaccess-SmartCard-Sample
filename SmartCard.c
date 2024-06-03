#include "SmartCard.h"
#include "SerialCom.h"
//----------------------------Mahdi Zakariyazadeh (2010)-----------------------------------
extern void PRINTF(const char *, ...);
extern void DUMP(unsigned char *, ...);
//-----------------------------------------------------------------------------------------
void PRINTF(const char *str,unsigned float A,unsigned float B,unsigned float C,unsigned float D,unsigned float E,unsigned float F)
{
	//printf(str,A,B,C,D,E,F);
}
//------------------------------------------------------------------------0000-----------------
void DUMP(unsigned char *adr, int l ,const char *str ,unsigned char data)
{
	/*
	int i;
	if(str){
	PRINTF(str ,data);
	}
	for(i=0; i < l; i++){
	 	PRINTF("%02X ",adr[i]);
	    if ((i&0xF)==0xF) PRINTF("\n");
	}PRINTF("\n");*/
}
//----------------------------------------------------------------------------------------------
unsigned char XorSum(const unsigned char *mem, int len)
{
  unsigned char cs=0;
  while(len>0) { cs ^= *mem++; len--; }
  return cs;
}
//----------------------------------------------------------------------------------------------
void Invert(unsigned char *data, int n)
{
  int i;
  static const unsigned char swaptab[] =  { 15,7,11,3,13,5,9,1,14,6,10,2,12,4,8,0 };
  for(i=n-1; i>=0; i--)
    data[i]=(swaptab[data[i]&0x0f]<<4) | swaptab[data[i]>>4];
}
//----------------------------------------------------------------------------------------------
/*
unsigned long Bin2LongLong(unsigned char *mem, int len)
{
  unsigned long k=0;
  int i;
  for(i=0 ; i<len ; i++) { k<<=8; k+=mem[i]; }
  return k;
}
*/
//----------------------------------------------------------------------------------------------
int Status(byte *sb ,const struct StatusMsg *m)
{
  while(m->sb[0]!=0xFF) {
    if(sb[0]==m->sb[0] && (m->sb[1]==0xFF || sb[1]==m->sb[1])) {
      if(!m->retval) PRINTF("\nMessage : %s status: %02x %02x\n",m->message,sb[0],sb[1]);
	  PRINTF("Message : %s status: %02x %02x\n",m->message,sb[0],sb[1]);
      return m->retval;
      }
    m++;
    }
  PRINTF("\nstatus: %02x %02x\n",sb[0],sb[1]);
  return false;
}
//----------------------------------------------------------------------------------------------
int ParseAtr(_atr *Atr, int id, int clock)
{
  const unsigned char *atr=Atr->atr;
  unsigned char cs;
  int Y ,i ,j ,len;

  PRINTF("=======================ATR INFO===========================\n");
  // default values
  Atr->histLen=0; Atr->T=0; Atr->F=372; Atr->D=1.0; Atr->N=0; Atr->WI=10;
  Atr->BWI=4; Atr->CWI=0; Atr->Tspec=-1;
  if(atr[0]==0x03) {
    PRINTF("%d: indirect convention detected\n",id);
    Invert(Atr->atr,Atr->atrLen);
    PRINTF("ATR After Invert:");DUMP(Atr->atr,Atr->atrLen,0);
    Atr->convention=SM_INDIRECT;
    }
  else if(atr[0]==0x3B) {
    PRINTF("%d: direct convention detected\n",id);
    Atr->convention=SM_DIRECT;
    }
  else {
    //PRINTF("%d: byte mode not supported 0x%02x\n",id,atr[0]);
     PRINTF("%d: byte mode not supported 0x%02x\n",id,atr[0]);
     PRINTF("---------try to get info!----------\n");
   // return 0;
    }

  // TS TO
  Atr->histLen=atr[1]&0x0F;
  Y=atr[1]&0xF0,i=1,len=2;
  
  PRINTF("%d: atr decoding TS=%02x hist=%d Y%d=%02x \n",id,atr[0],Atr->histLen,i,Y);
  do {
    if(Y&0x10) { // TAi
      NEED(1);
      PRINTF("TA%d=%02x \n",i,atr[len]);
      if(i==1) {
        Atr->TA1=atr[len];
        Atr->F=Ftable[(atr[len]>>4)&0x0F];
        Atr->D=Dtable[ atr[len]    &0x0F];
        PRINTF("F=%d D=%f \n",Atr->F,Atr->D);
        }
      else if(i==2) {
        Atr->Tspec=atr[len]&0x0F;
        PRINTF("Tspec=%d \n",Atr->Tspec);
        }
      else if(i==3) {
        PRINTF("IFSC=%d ",atr[len]);
        }
      len++;
      }
    if(Y&0x20) { // TBi
      NEED(1);
      PRINTF("TB%d=%02x \n",i,atr[len]);
      if(i==3) {
        Atr->BWI=(atr[len]>>4)&0x0F;
        Atr->CWI=atr[len]&0x0F;
        PRINTF("BWI=%d CWI=%d \n",Atr->BWI,Atr->CWI);
        }
      len++;
      }
    if(Y&0x40) { // TCi
      NEED(1);
      PRINTF("TC%d=%02x \n",i,atr[len]);
      if(i==1) {
        Atr->N=atr[len];
        PRINTF("N=%d \n",Atr->N);
        }
      else if(i==2) {
        Atr->WI=atr[len];
        PRINTF("WI=%d \n",Atr->WI);
        }
      else if(i==3) {
        PRINTF("CHK=%s \n",atr[len]&1 ? "CRC16":"LRC");
        }
      len++;
      }
    if(Y&0x80) { // TDi
      NEED(1);
      PRINTF("TD%d=%02x \n",i,atr[len]);
      if(i==1) {
        Atr->T=atr[len]&0x0F;
        PRINTF("T=%d \n",Atr->T);
        }
      else {
        PRINTF("Tn=%d \n",atr[len]&0x0F);
        }
      Y=atr[len]&0xF0;
      len++;
      }
    else Y=0;
    i++;
    PRINTF("Y%d=%02x \n",i,Y);
    } while(Y);
  NEED(Atr->histLen);
  

  PRINTF("%d: historical:",id);
  for(j=0 ; j<Atr->histLen ; j++) PRINTF(" %02x",atr[len+j]);
  
  PRINTF("\n%d: historical: '",id);
  for(j=0 ; j<Atr->histLen ; j++) PRINTF("%c",isprint(atr[len+j]) ? atr[len+j] : '.');

  memcpy(Atr->hist,&atr[len],Atr->histLen);
  len+=Atr->histLen;

  // TCK
  if(Atr->T!=0 && len+1<=Atr->atrLen) {
    len++;
    cs=XorSum(atr+1,len-1);
    // according to the ISO the initial TS byte isn't checksumed, but
    // some cards do so. In this case the checksum is equal TS.
    if(cs==0) PRINTF("%d: atr checksum ok",id);
    else if(cs==atr[0]) PRINTF("\niso: %d: atr checksum is TS (not ISO compliant)\n",id);
    else {
      PRINTF("%d: atr checksum FAILED (cs:%02x)\n",id,cs);
      return 0;
      }
    }
  else PRINTF("\n%d: atr checksum not given/not required\n",id);

  if(Atr->atrLen>len) PRINTF("%d: long atr read=%d len=%d",id,Atr->atrLen,len);
  Atr->wwt=960*Atr->WI*Atr->F/(clock/1000);
  Atr->bwt=(int)(960*(float)(1<<Atr->BWI)*372/(clock/1000));
  PRINTF("\n%d: indicated wwt(T0)=%d ms, bwt(T1)=%d ms (at %.4f MHz)\n",id,Atr->wwt,Atr->bwt,(float)clock/1000000);

    PRINTF("==========================================================\n\n");
  return 1;
}
//-----------------------------------------------------------------------------------------
BOOL IsoReadAtr(BYTE *buff)
{
unsigned char rByte;
int iCount ,len;

    len=0;
	for(iCount=0;iCount<rISO_TIMEOUT;iCount++)
	{
		if(SerialComReadByte(&rByte))
		{
			buff[iCount]=rByte;
			len++;
		}
		else return len;
	}
    return false;
}
//-----------------------------------------------------------------------------------------
int Read(_atr *atr,unsigned char *data, int len)
{
  int r=SerialComRead(data,len);

  dump(data ,len);
  if(atr->convention==SM_INDIRECT && r>0)Invert(data,r);

  return r;
}
//-----------------------------------------------------------------------------------------
int Write(_atr *atr,const unsigned char *data, int len)
{
  unsigned char tmp[255];
  int r;
  if(atr->convention==SM_INDIRECT) {
    memcpy(tmp,data,len);
    Invert(tmp,len);
    data=tmp;
    }

dump(data,len);

  r=SerialComWrite(data,len,wISO_TIMEOUT);
  if(r>0) {
    int rr=SerialComRead(data,len);
    if(rr<0) r=rr;
    }
  return r;
}
//-----------------------------------------------------------------------------------------
int Procedure(_atr *atr ,unsigned char ins, int restLen ,unsigned char *sb)
{
  int r;
  unsigned char buff;
  do {
    do {
      if(Read(atr,&buff,1)<=0) return -1;
       printf("buff1 %02X ",buff);
      } while(buff==0x60);

       printf("buff2 %02X ",buff);
    if((buff&0xF0)==0x60 || (buff&0xF0)==0x90) { // SW1/SW2
      sb[0]=buff;
      if(Read(atr,&buff,1)<=0) return -1;
      PRINTF("%02X",buff);
      sb[1]=buff;
      return 0;
      }
    else {
      if((buff&0xFE)==(ins&0xFE)) r=restLen;
      else if((~buff&0xFE)==(ins&0xFE)) r=1;
      else {
        PRINTF("\ncannot handle procedure %02x (ins=%02x)",buff,ins);
        return -1;
        }
      if(r>restLen) {
        PRINTF("\ndata overrun r=%d restLen=%d",r,restLen);
        return -1;
        }
      }
    } while(r==0);
  return r;
}
//-----------------------------------------------------------------------------------------
bool IsoRead(_atr *atr,const unsigned char *cmd,unsigned char *data,unsigned char *status)
{
int tr,len,r;
  if(atr->T==0) { // only for ISO complaint cards
    DUMP(cmd,CMD_LEN,"\n->INS:");
    if(Write(atr,cmd,CMD_LEN)<0) return Test(false);
    tr=cmd[LEN_IDX] ? cmd[LEN_IDX] : 256;
    len=0;
    while(1) {
      r=Procedure(atr,cmd[INS_IDX],tr-len ,status);
      if(r<=0) return Test(r==0);
      if(Read(atr,data+len,r)<0) return Test(false);
      DUMP(data+len,r,"\n<-DATA:");
      len+=r;
      }
    }
  else PRINTF("can't IsoRead() from incompatible card (T=%d)",atr->T);
  return false;
}
//-----------------------------------------------------------------------------------------
bool IsoWrite(_atr *atr,const unsigned char *cmd,unsigned char *data,unsigned char *status)
{
int len,r;
  if(atr->T==0) { // only for ISO complaint cards
    //dump(cmd,CMD_LEN);
    if(Write(atr,cmd,CMD_LEN)<0) return Test(false);
    len=0;
    while(1) {

      r=Procedure(atr,cmd[INS_IDX],cmd[LEN_IDX]-len,status);

	  printf("r:%d\n",r);

	  getch();


      if(r<=0) return Test(r==0);
      if(Write(atr,data+len,r)<0) return Test(false);
      DUMP(data+len,r,"\n->DATA:");
      len+=r;
      }
    }
  else PRINTF("can't IsoWrite() to incompatible card (T=%d)",atr->T);
  return false;
}
//-----------------------------------------------------------------------------------------
bool Test(bool res)
{
	if(!res) { printf("\nreset triggered");}
 return res;
}
//-----------------------------------------------------------------------------------------
