#ifndef scViccessH
	#define scViccessH
//------------------------------------------------------------------------------------------
#define MAX_PRV 32

typedef struct {
  unsigned char id[3];
  unsigned char sa[3];
  unsigned char availdKeys[16];
  char name[33];
}ViaccessProvInfo;

typedef struct{
unsigned int NumPrv;
ViaccessProvInfo VPrv[MAX_PRV];
unsigned char EmmBuf[255];
unsigned char EmmCheck;
int Emm8DLen;
}ViaCardInfo;

static const struct StatusMsg ViaccessMsg[] = {
  { { 0x6b,0x00 }, "Instruction not supported", false },
  { { 0x6d,0x00 }, "Instruction not supported", false },
  { { 0x90,0x00 }, "Instruction executed without errors", true },
  { { 0x90,0x08 }, "Instruction executed without errors", true },
  { { 0x90,0x40 }, "Instruction executed with errors", false },
  { { 0x91,0x00 }, "", true },
  { { 0xFF,0xFF }, 0, false }
  };
//------------------------------------------------------------------------------------------
#endif
