// -- cSmartCardSeca -------------------------------------------------------------------------

#include "SmartCard.h"
#include "SerialCom.h"
#include "scSeca.h"

//--- sDate ----------------------------------------------------------------------------------
void sDate (unsigned char *dd)
{ 
 printf("%04d/%02d/%02d",((dd[0]&0xFE)>>1)+1990,((dd[0]&0x01)<<3) + ((dd[1]&0xE0)>>5),dd[1]&0x1F);
}

// -- sCmdLen --------------------------------------------------------------------------------
int sCmdLen(const unsigned char *data)
{
  SecaCmd *s=(SecaCmd *)data;
  return ((s->cmdLen1&0x0f)<<8) | s->cmdLen2;
}
// -- sPayload -------------------------------------------------------------------------------
int sPayload(const unsigned char *data, const unsigned char **payload)
{
  int l;
  switch(TID(data)) {
    case 0x80:
    case 0x81: l=sizeof(SecaEcm); break;
    case 0x82: l=sizeof(SecaEmmUnique); break;
    case 0x84: l=sizeof(SecaEmmShared); break;
    default: return -1;
    }
  if(payload)*payload=&data[l];
  return sCmdLen(data)-l+sizeof(SecaCmd);
}
// -- sSysMode -------------------------------------------------------------------------------
int sSysMode(const unsigned char *data)
{
  switch(TID(data)) {
    case 0x80:
    case 0x81: return ((SecaEcm *)data)->sm; break;
    case 0x82: return ((SecaEmmUnique *)data)->sm; break;
    case 0x84: return ((SecaEmmShared *)data)->sm; break;
    default: return -1;
    }
}
// -- sKeyNr ---------------------------------------------------------------------------------
int sKeyNr(const unsigned char *data)
{
  switch(TID(data)) {
    case 0x80:
    case 0x81: return ((SecaEcm *)data)->keyNr; break;
    case 0x82: return ((SecaEmmUnique *)data)->keyNr; break;
    case 0x84: return ((SecaEmmShared *)data)->keyNr; break;
    default: return -1;
    }
}
//--- cSecaInit ------------------------------------------------------------------------------
bool cSecaInit(_atr *atr ,SecaCardInfo *iSecaCard)
{
   static unsigned char ins0e[] = { 0xC1,0x0e,0x00,0x00,0x08 }; // get serial nr. (UA)
   static unsigned char ins16[] = { 0xC1,0x16,0x00,0x00,0x07 }; // get nr. of providers
   static unsigned char ins12[] = { 0xC1,0x12,0x00,0x00,0x19 }; // get provider info
   static unsigned char ins34[] = { 0xC1,0x34,0x00,0x00,0x03 }; // request provider pbm (data=0x00 0x00 0x00)
   static unsigned char ins32[] = { 0xC1,0x32,0x00,0x00,0x0A }; // get the pbm data (p1=provider)
   const unsigned char ins34data[] = { 0x00,0x00,0x00 };
   const unsigned char atrTester[] = { 0x0E,0x6C,0xB6,0xD6 };
   unsigned char buff[MAX_LEN] ,msq[2];
   char *type;
   int len,provMap,n,i;
  
   //Check SecaCard
   if(atr->T!=0 || atr->histLen<7 || memcmp(&atr->hist[3],atrTester,4)){
    printf("doesn't looks like a SecaCard\n");
	return false;
   }

   printf("CAS : SecaCard\n");

   //CardType
   switch(atr->hist[0]*256+atr->hist[1]) 
   {
    case 0x5084: type="Generic"; break;
    case 0x5384: type="Philips"; break;
    case 0x5130:
    case 0x5430:
    case 0x5760: type="Thompson"; break;
    case 0x5284:
    case 0x5842:
    case 0x6060: type="Siemens";  break;
    case 0x7070: type="Canal+ NL";break;
    default:     type="Unknown";  break;
   }
    printf("CardType: %s %d.%d\n",type,atr->hist[2]&0x0F,atr->hist[2]>>4);

	//Read CardSn 
    if(!IsoRead(atr,ins0e,buff,msq) || !Status(msq,SecaMsq)){
    printf("Reading Card Serial Failed\n");
    return false;
    }
	printf("Card Serial Number Hex: ");dump(&buff[2],6);

	//Card Sn
	memcpy(iSecaCard->CardSN,&buff[2],6);

    if(!IsoRead(atr,ins16,buff,msq) || !Status(msq,SecaMsq)){
    printf("reading provider map failed\n");
    return false;
    }
    provMap=buff[2]*256+buff[3];
//	printf("provMap:%02X\n",provMap); 
    n=0,i=provMap;
    do{n+=i&1;i>>=1;}while(i);
    printf("Card Has %d Providers\n",n);
	iSecaCard->NumPrv=n;

    for(i=0;i<n;i++) 
	{
     if(provMap&(1<<i)) 
	 {
      printf("\nreading info for provider index %d\n",i);
      ins12[2]=i;
      if(!IsoRead(atr,ins12,buff,msq) || !Status(msq,SecaMsq)) {
        printf("reading provider info failed");
        return false;
	  }
      ins32[2]=i;
	  memset(ins34data,0,3);
      if(!IsoWrite(atr,ins34,ins34data,msq) ||
		 !IsoRead(atr,ins32,&buff[ins12[4]],msq) || !Status(msq ,SecaMsq)){
            memset(&buff[ins12[4]],0xFF,ins32[4]); // fake PBM response if card doesn't support command
	  }
	  //Save Prv Info
       memcpy(&iSecaCard->SPrv[i],buff,sizeof(iSecaCard->SPrv[i]));

       printf("PrvId :"); dump(iSecaCard->SPrv[i].prov,2);
       printf("Prv Name (%s)\n",iSecaCard->SPrv[i].name);
       printf("PPUA :"); dump(iSecaCard->SPrv[i].sa ,3);
	   printf("Expire Date ");
	   sDate(iSecaCard->SPrv[i].date);
	 }
	}
     return true; //OK
}
//--- cSecaDecode ------------------------------------------------------------------------------
bool cSecaDecode(_atr *atr ,SecaCardInfo *iSecaCard ,const unsigned char *ecm ,unsigned char *cw)
{
  static unsigned char ins3c[] = { 0xC1,0x3c,0x00,0x00,0x00 }; // coding cw
  static unsigned char ins3a[] = { 0xC1,0x3a,0x00,0x00,0x10 }; // decoding cw    
  static unsigned char ins30[] = { 0xC1,0x30,0x00,0x02,0x09 }; // view ppv (active bx record)
  static unsigned char ins30data[] = { 0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00,0xFF }; 
  const unsigned char *payload;
  unsigned char msq[2];
  int index;
  SecaEcm *cSecaEcm=(SecaEcm *)ecm;

  for(index=0;index<iSecaCard->NumPrv;index++)
  {
    if(cSecaEcm->id[0]==iSecaCard->SPrv[index].prov[0]&&
	   cSecaEcm->id[1]==iSecaCard->SPrv[index].prov[1])break;
  }
  if(index==iSecaCard->NumPrv) {
      printf("\nHaven't Your Subscription to View This Channel");
	  return false;
  }
    printf("\n\nProvider %04x index %d (%.16s)",iSecaCard->SPrv[index].prov[0]*256+iSecaCard->SPrv[index].prov[1],index,iSecaCard->SPrv[index].name);
    //PPV Check
	{
        if(!IsoWrite(atr,ins30,ins30data,msq) || !Status(msq,SecaMsq)) {
        printf("\nPPV : no activate\n");
		}
		else{
        printf("\nPPV activate\n");
		}
	}
       ins3c[2]=index|(sSysMode(cSecaEcm)&0xF0);
       ins3c[3]=sKeyNr(cSecaEcm);
       ins3c[4]=sPayload(cSecaEcm,&payload);
       if(IsoWrite(atr,ins3c,payload,msq)&&Status(msq ,SecaMsq)&&
		   IsoRead(atr,ins3a,cw,msq)&&Status(msq,SecaMsq)){
		   return true; //Decode OK!
	   }
       return false;
}
//--- cSecaUpdate ------------------------------------------------------------------------------
bool cSecaUpdate(_atr *atr ,SecaCardInfo *iSecaCard ,const unsigned char *emm)
{
  static unsigned char ins40[] = { 0xC1,0x40,0x00,0x00,0x00 }; 
  const unsigned char *payload;
  unsigned char msq[2] ,id[2];
  SecaEmmUnique *cSecaEmmUnique;
  SecaEmmShared *cSecaEmmShared;
  int index;

 //if(iSecaCard->blocker==0 || (emm[0]==0x82&&iSecaCard->blocker==2) || (emm[0]==0x84&&iSecaCard->blocker==1)) {}
  switch(emm[0])
  {
    case 0x82: //EMM SN UPDATE
	  cSecaEmmUnique=(SecaEmmUnique*)emm;
	  id[0]=cSecaEmmUnique->id[0];
	  id[1]=cSecaEmmUnique->id[1];
	  break;

    case 0x84://EMM PPUA UPDATE
	  cSecaEmmShared=(SecaEmmShared*)emm;
	  id[0]=cSecaEmmShared->id[0];
	  id[1]=cSecaEmmShared->id[1];
	  break;

	default:return false;
  }

  for(index=0;index<iSecaCard->NumPrv;index++)
  {
    if(id[0]==iSecaCard->SPrv[index].prov[0]&&
	   id[1]==iSecaCard->SPrv[index].prov[1])break;
  }
  if(index==iSecaCard->NumPrv) return false;

  if(emm[0]==0x82)//EMM SN UPDATE
  {
      if(memcmp(iSecaCard->CardSN,cSecaEmmUnique->ua,6))return false;
      ins40[2]=index|(sSysMode(cSecaEmmUnique)&0xF0);
      ins40[3]=sKeyNr(cSecaEmmUnique);
      ins40[4]=sPayload(cSecaEmmUnique,&payload);
  }
  if(emm[0]==0x84)//EMM PPUA UPDATE
  {
	  if(memcmp(iSecaCard->SPrv[index].sa,cSecaEmmShared->sa,3))return false;
      ins40[2]=index|(sSysMode(cSecaEmmShared)&0xF0);
      ins40[3]=sKeyNr(cSecaEmmShared);
      ins40[4]=sPayload(cSecaEmmShared,&payload);
  }

   printf("\nGot %s Update",emm[0]==0x82?"UNIQUE":"SHARED");
    
   if(IsoWrite(atr,ins40,payload,msq)&&Status(msq,SecaMsq)) return cSecaInit(atr,iSecaCard);

   return false;
}


