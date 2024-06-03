#ifndef scSecaH
	#define scSecaH
//------------------------------------------------------------------------------------------
#define MAX_PRV_ID 16
typedef struct {
  unsigned char prov[2];
  char name[16];
  unsigned char sa[3];
  unsigned char cb;
  unsigned char date[2];
  unsigned char rr;
  unsigned char rstart;
  unsigned char pbm[8];
  unsigned char rend;
}SecaProvInfo;

typedef struct {
unsigned int NumPrv;
unsigned char CardSN[6];
SecaProvInfo SPrv[MAX_PRV_ID];
}SecaCardInfo;

typedef struct{
  unsigned char tid;
  unsigned char cmdLen1;
  unsigned char cmdLen2;
  }SecaCmd ;

typedef struct {
  SecaCmd cmd;
  unsigned char id[2];
  unsigned char unknown;
  unsigned char sm;
  unsigned char keyNr;
  }SecaEcm;

typedef struct {
  SecaCmd cmd;
  unsigned char id[2];
  unsigned char sa[3];
  unsigned char sm;
  unsigned char keyNr;
  }SecaEmmShared;

typedef struct  {
  SecaCmd cmd;
  unsigned char ua[6];
  unsigned char id[2];
  unsigned char sm;
  unsigned char keyNr;
  }SecaEmmUnique;

#define TID(x) ((( SecaCmd *)(x))->tid)
#define SA(x)  ((( SecaEmmShared *)(x))->sa)
#define SID(x) ((( SecaEmmShared *)(x))->id)
#define UA(x)  ((( SecaEmmUnique *)(x))->ua)
#define UID(x) ((( SecaEmmUnique *)(x))->id)
/*
#define SECADATE(odate){ \
                         const unsigned char *dd=(odate); \
                         printf("Expires->%04d/%02d/%02d", \
                         ((dd[0]&0xFE)>>1)+1990, \
                         ((dd[0]&0x01)<<3) + ((dd[1]&0xE0)>>5), \
                           dd[1]&0x1F); \
                       }
*/
static const struct StatusMsg SecaMsq[] = {
  // ECM status messages
  { { 0x90,0x00 }, "Instruction executed without errors", true },
  { { 0x90,0x02 }, "Signature failed", false },
  { { 0x90,0x27 }, "Decoding the preview (non-error)", true },
  { { 0x93,0x02 }, "No access, check your subscription", false },
  { { 0x96,0x00 }, "Chain of nonvalid entrance or null event or all the nanos process, none decoding", false },
  // EMM update status messages
  { { 0x90,0x09 }, "Card update was not meant for this card", false },
  { { 0x90,0x19 }, "Card update successfull, PPUA updated", true },
  { { 0x97,0x00 }, "Card update was successful", true },
  { { 0x97,0x40 }, "Card update was successful", true },
  { { 0x97,0x50 }, "Card update was successful", true },
  { { 0x97,0x78 }, "Card update was not necessary", false },
  { { 0x97,0xe0 }, "EEPROM update was not necessary", false },
  // unknown message
  { { 0xFF,0xFF }, 0, false }
  };
//------------------------------------------------------------------------------------------
#endif
