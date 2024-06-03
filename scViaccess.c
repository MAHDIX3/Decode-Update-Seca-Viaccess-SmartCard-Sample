#include "SmartCard.h"
#include "SerialCom.h"
#include "scViaccess.h"
// -- cSmartCardViccess -------------------------------------------------------------------------
#define SCT_LEN(sct) (3+(((sct)[1]&0x0f)<<8)+(sct)[2])
//--- GetSW1 ------------------------------------------------------------------------------------
unsigned char GetSW1(unsigned char *sb) { return sb[1]; }
// -- cNanoStart --------------------------------------------------------------------------------
const unsigned char *vNanoStart(const unsigned char *data)
{
  switch(data[0]) {
    case 0x88: return &data[8];
    case 0x8e: return &data[7];
    case 0x8c:
    case 0x8d: return &data[3];
    case 0x80:
    case 0x81: return &data[4];
    }
  return 0;
}
// -- cCheckNano90FromNano ----------------------------------------------------------------------
const unsigned char *vCheckNano90FromNano(const unsigned char *data)
{
  if(data && data[0]==0x90 && data[1]==0x03) return data;
  return 0;
}
// -- cCheckNano90 ------------------------------------------------------------------------------
const unsigned char *vCheckNano90(const unsigned char *data)
{
  return vCheckNano90FromNano(vNanoStart(data));
}
// -- vKeyNr ----------------------------------------------------------------------
int vKeyNr(const unsigned char *data)
{
  return vKeyNrFromNano(vCheckNano90(data));
}
// -- vKeyNrFromNano --------------------------------------------------------------
int vKeyNrFromNano(const unsigned char *data)
{
  return data ? data[4]&0x0f : -1;
}
// -- vProvIdPtr ------------------------------------------------------------------
const unsigned char *vProvIdPtr(const unsigned char *data)
{
  data=vCheckNano90(data);
  return data ? &data[2] : 0;
}
// -- vProvIdPtr ------------------------------------------------------------------
const unsigned char *ProvIdPtr(const unsigned char *data)
{
  data=vCheckNano90(data);
  return data ? &data[2] : 0;
}
// -- vProvId ---------------------------------------------------------------------
int vProvId(const unsigned char *data)
{
   const unsigned char *id=ProvIdPtr(data);
   return id ? (id[0]<<16)+(id[1]<<8)+(id[2]&0xf0) : -1;
}
// -- vSetProvider ---------------------------------------------------------------------
bool vSetProvider(_atr *atr,const unsigned char *id)
{
  static const unsigned char insa4[] = { 0xca,0xa4,0x04,0x00,0x03 };
  unsigned char msq[2],ident[2];

  ident[0]=id[0];
  ident[1]=id[1];
  ident[2]=(id[2]&0xF0);
  if(!IsoWrite(atr,insa4,ident,msq) || !Status(msq,ViaccessMsg))  return false;
  if(msq[0]==0x90)
  {
	  if(msq[1]==0x00)return true;
	  if(msq[1]==0x08)return false;
  }
  return false;
}
// -- vSetProvider ---------------------------------------------------------------------
bool vCheckKey(ViaCardInfo *iViaCard ,int index ,const unsigned char keynr)
{
unsigned int j;

  for(j=0; j<sizeof(iViaCard->VPrv[index].availdKeys);j++)
  {
    if(iViaCard->VPrv[index].availdKeys[j]==keynr) return true;
  }

  return false;
}
// -- vMatchEmm ---------------------------------------------------------------------
int vMatchEmm(const unsigned char *data ,const unsigned char *sa ,ViaCardInfo *iViaCard)
{
  int len=SCT_LEN(data);
  switch(data[0]) 
  {
    case 0x8E:
		if(!memcmp(data+3,sa,3)){
		    memcpy(iViaCard->EmmBuf,data,len);
			iViaCard->Emm8DLen=len;
			iViaCard->EmmCheck=1;
		    return 0;
		}
		else{
			iViaCard->EmmCheck=0;
		    return 0;
		}
    case 0x8C:
    case 0x8D: 
		if(iViaCard->EmmCheck==1){
		   memcpy(iViaCard->EmmBuf+iViaCard->Emm8DLen,data,len);
		   iViaCard->EmmCheck=0;
		   return 2; //OK
		}
		else return 0;

    default:return 0;
  }
}
// -- vMatchEmm ---------------------------------------------------------------------
int vAssembledEmm(ViaCardInfo *iViaCard,unsigned char *data)
{
	int i,len=0,nlen=0;
	//9Exx
    data[8]=0x9E;data[9]=iViaCard->EmmBuf[2]-4-8;
	memcpy(&data[10] ,&iViaCard->EmmBuf[7] ,data[9]);
	len+=data[9]+2;
	for(i=0;i<iViaCard->EmmBuf[iViaCard->Emm8DLen+2];)
	{
		switch(iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i])
		{
		//9003
		case 0x90:
			 memcpy(&data[3],&iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i],5);
			 i+=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1];
			 len+=5;
			 break;
	    //AXXX
		case 0xA1:
			 nlen=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1]+2;
			 memcpy(&data[10+data[9]],&iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i],nlen);
			 i+=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1];
			 len+=nlen;
			 break;
		case 0xA3:
			 i+=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1];
			 break;
		case 0xA9:
			 i+=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1];
			 break;

	    //EF08
		case 0xEF:
			 memcpy(&data[10+data[9]+nlen],&iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i],10);
			 nlen+=10;
			 i+=iViaCard->EmmBuf[iViaCard->Emm8DLen+3+i+1];
			 len+=10;
			 break;
		}
    	i++;
	}
	//F008
    data[10+data[9]+nlen]=0xF0;data[10+data[9]+nlen+1]=0x08;
	memcpy(&data[10+data[9]+nlen+2] ,&iViaCard->EmmBuf[7+data[9]] ,8);
	len+=10;
	data[2]=len;
	data[0]=0x8D;
	data[1]=0;
}
//--- cViaccessInit -----------------------------------------------------------------------------
int cViaccessInit(_atr *atr,ViaCardInfo *iViaCard)
{
  static const unsigned char verifyBytes[] = { 0x90,0x00 };
  static unsigned char insac[] = { 0xCA, 0xac, 0x00, 0x00, 0x00 }; // select data
  static unsigned char insb8[] = { 0xCA, 0xb8, 0x00, 0x00, 0x00 }; // read selected data
  static unsigned char insa4[] = { 0xCA, 0xa4, 0x00, 0x00, 0x00 }; // select issuer
  static unsigned char insc0[] = { 0xCA, 0xc0, 0x00, 0x00, 0x00 }; // read data item
  unsigned char buff[MAX_LEN],buff2[MAX_LEN],msq[2];
  unsigned int nameLen;
  char *ver=0;

  if(atr->T!=0 || atr->histLen<7 || memcmp(&atr->hist[5],verifyBytes,sizeof(verifyBytes))) {
    printf("doesn't look like a Viaccess card");
    return false;
    }
    printf("CAS : ViccessCard\n");

  switch((atr->hist[3]<<8)|atr->hist[4]) 
  {
    case 0x6268: ver="2.3"; break;
    case 0x6468:
    case 0x6668: ver="2.4"; break;
    case 0xa268: ver="2.5"; break;
    case 0xc168: ver="2.6"; break;
    default: ver="unknown"; break;
  }

  printf("Card V%s\n",ver);
  insac[2]=0xa4; // request unique id

  if(!IsoWrite(atr,insac,buff,msq) || !Status(msq,ViaccessMsg)) {
    printf("failed to request ua\n");
    return false;
    }
  insb8[4]=0x07; // read unique id
  if(!IsoRead(atr,insb8,buff,msq) || !Status(msq,ViaccessMsg) || buff[1]!=0x05) {
    printf("failed to read ua\n");
    return false;
    }
	printf("Card Serial Number Hex: ");dump(&buff[2],5);

  insa4[2]=0x00; // select issuer 0
  if(!IsoWrite(atr,insa4,buff,msq) || !Status(msq,ViaccessMsg)) {
    printf("failed to select issuer 0\n");
    return false;
    }

  iViaCard->NumPrv=0;
  do {
    insc0[4]=0x1A; // show provider properties
    if(!IsoRead(atr,insc0,buff,msq) || !Status(msq,ViaccessMsg)) {
      printf("failed to read prov properties");
      return false;
      }
      memcpy(iViaCard->VPrv[iViaCard->NumPrv].id,buff,3);

    insac[2]=0xa5; // request sa
    if(!IsoWrite(atr,insac,buff2,msq) || !Status(msq,ViaccessMsg)) {
      printf("failed to request sa");
      return false;
      }
    insb8[4]=0x06; // read sa
    if(!IsoRead(atr,insb8,buff2,msq) || !Status(msq,ViaccessMsg)) {
      printf("failed to read sa");
      return false;
      }
      memcpy(iViaCard->VPrv[iViaCard->NumPrv].sa,buff2+2,3);
      memcpy(iViaCard->VPrv[iViaCard->NumPrv].availdKeys,buff+10,sizeof(iViaCard->VPrv[iViaCard->NumPrv].availdKeys));

      insac[2]=0xa7; // request name
      if(!IsoWrite(atr,insac,buff,msq) || !Status(msq,ViaccessMsg)) {
        printf("failed to request prov name");
        return false;
        }
	  
      insb8[4]=0x02; // read name nano + len
      if(!IsoRead(atr,insb8,buff,msq) || !Status(msq,ViaccessMsg)) {
        printf("failed to read prov name length");
        return false;
        }

      nameLen=buff[1];
      if(nameLen>=sizeof(iViaCard->VPrv[iViaCard->NumPrv].name)) {
        printf("provider name buffer overflow");
        nameLen=sizeof(iViaCard->VPrv[iViaCard->NumPrv].name)-1;
        }
	  
      insb8[4]=nameLen;
      if(!IsoRead(atr,insb8,buff,msq) || !Status(msq,ViaccessMsg)) {
        printf("failed to read prov name");
        return false;
        }
        memcpy(iViaCard->VPrv[iViaCard->NumPrv].name,buff,nameLen); 
	    iViaCard->VPrv[iViaCard->NumPrv].name[nameLen]=0;

      insa4[2]=0x02; // next issuer
      if(!IsoWrite(atr,insa4,buff,msq) || !Status(msq,ViaccessMsg)) {
      printf("failed to select next issuer");
      return false;
      }

	  //Info
	  
      printf("\nProvider Name : (%s)" ,iViaCard->VPrv[iViaCard->NumPrv].name);
 	  printf("\nPrvId : ");dump(iViaCard->VPrv[iViaCard->NumPrv].id,3);
	  printf("SA : ");dump(iViaCard->VPrv[iViaCard->NumPrv].sa,3);
	  printf("Vaild Keys : ");dump(iViaCard->VPrv[iViaCard->NumPrv].availdKeys,16);

	  iViaCard->NumPrv++;
	  if(iViaCard->NumPrv==MAX_PRV){
       printf("no memory for provider");
	   return true;//OK
	  }
    } while(GetSW1(msq)==0x00);

  return true;//OK
}
//--- cViaccessDecode -----------------------------------------------------------------------------
bool cViaccessDecode(_atr *atr,ViaCardInfo *iViaCard,const unsigned char *ecm,unsigned char *cw)
{
  static unsigned char ins88[] = { 0xca,0x88,0x00,0x00,0x00 }; // set ecm
  static unsigned char insf8[] = { 0xca,0xf8,0x00,0x00,0x00 }; // set geographic info 
  static unsigned char insc0[] = { 0xca,0xc0,0x00,0x00,0x12 }; // read dcw
  unsigned char buff[MAX_LEN],msq[2];
  unsigned char keynr;
  const unsigned char *start=vNanoStart(ecm)+5;
  const unsigned char *ecm88Data=start;
  const unsigned char *ident=vProvIdPtr(ecm);
  int ecm88Len=SCT_LEN(ecm)-(start-ecm);
  int ecmf8Len=0;
  int index;

  //Search PrvId 
  for(index=0;index<iViaCard->NumPrv;index++)
  {
	  if(ident[0]==iViaCard->VPrv[index].id[0]&&
		 ident[1]==iViaCard->VPrv[index].id[1]&&
		(ident[2]&0xF0)==(iViaCard->VPrv[index].id[2]&0xF0)) break;
  }
  if(index==iViaCard->NumPrv)
  {
     printf("Provider Not Found on Card\n");
	 return false;
  }
  else
  {
    printf("\nPrvIndex in Card: %d\n",index );
    keynr=vKeyNr(ecm);
    if(vCheckKey(iViaCard,index,keynr)) 
	{
      if(!vSetProvider(atr,vProvIdPtr(ecm))) return false;

      while(ecm88Len>0 && ecm88Data[0]<0xA0) 
	  {
        const int nanoLen=ecm88Data[1]+2;
        ecmf8Len+=nanoLen;
        ecm88Len-=nanoLen; ecm88Data+=nanoLen;
	  }
      if(ecmf8Len)
	  {
        insf8[3]=keynr;
        insf8[4]=ecmf8Len;
        if(!IsoWrite(atr,insf8,start,msq) || !Status(msq,ViaccessMsg)) return false;
	  }
      ins88[2]=ecmf8Len?1:0;
      ins88[3]=keynr;
      ins88[4]=ecm88Len;
      if(!IsoWrite(atr,ins88,ecm88Data,msq) || !Status(msq,ViaccessMsg)) return false; // request dcw

      if(!IsoRead(atr,insc0,buff,msq) || !Status(msq,ViaccessMsg)) return false; // read dcw
      switch(buff[0]) 
	  {
        case 0xE8: // even
          if(buff[1]==8) { memcpy(cw,buff+2,8); return true; }
          break;
        case 0xE9: // odd
          if(buff[1]==8) { memcpy(cw+8,buff+2,8); return true; }
          break;
        case 0xEA: // complete
          if(buff[1]==16) { memcpy(cw,buff+2,16); return true; }
          break;
	  }
	}
    else printf("key not found on card");
  }
  return false;
}
//--- cViaccessUpdate -----------------------------------------------------------------------------
bool cViaccessUpdate(_atr *atr,ViaCardInfo *iViaCard,const unsigned char *data , unsigned char *ident)
{
  static unsigned char insf0[] = { 0xca,0xf0,0x00,0x00,0x00 };
  static unsigned char ins18[] = { 0xca,0x18,0x00,0x00,0x00 };
  unsigned char msq[2];
  const unsigned char *start;
  int updtype,n,index,nanolen;
  unsigned char emm[255];

  for(index=0;index<iViaCard->NumPrv;index++)
  {
	  if(ident[0]==iViaCard->VPrv[index].id[0]&&
		 ident[1]==iViaCard->VPrv[index].id[1]&&
		(ident[2]&0xF0)==(iViaCard->VPrv[index].id[2]&0xF0)) break;
  }
  if(index==iViaCard->NumPrv) return 0;

  if(updtype=vMatchEmm(data,iViaCard->VPrv[index].sa,iViaCard))
  {
	  vAssembledEmm(iViaCard,emm);
      start=vNanoStart(emm);
      nanolen=SCT_LEN(emm)-(start-emm);
      if(start&&start[0]==0x90&&start[1]==0x03&&nanolen>=5&&vSetProvider(atr,&start[2])) 
	  {
        insf0[3]=ins18[3]=vKeyNrFromNano(start);
        start+=5; nanolen-=5;
        if(start[0]==0x9e && nanolen>=(n=start[1]+2)) 
		{
          insf0[4]=n;
          if(!IsoWrite(atr,insf0,start,msq) || !Status(msq,ViaccessMsg)){}
          start+=n; nanolen-=n;
		}
        if(nanolen>0)
		{
          ins18[2]=updtype>0 ? updtype-1 : updtype;
          ins18[4]=nanolen;
          if(IsoWrite(atr,ins18,start,msq))  Status(msq,ViaccessMsg);
		  dump(msq ,2);
		}
	  }
  }
  else printf("Emm Not Complete\n");
  return false;
}

