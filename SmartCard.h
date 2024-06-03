//---------------------------------------------------------------------------
#ifndef SmartCardH
	#define SmartCardH
//---------------------------------------------------------------------------
#define rISO_TIMEOUT 100
#define wISO_TIMEOUT 1


#define MAX_LEN     256        // max. response length
#define CMD_LEN     5          // command length
#define INS_IDX     1          // INS index
#define LEN_IDX     4          // LEN index

#define SB_LEN      2          // status byte (SB) len
#define MAX_ATR_LEN 33         // max. ATR length
#define MAX_HIST    15         // max. number of historical characters
#define IDSTR_LEN   25         // lenght of card identify string

#define MAKE_SC_ID(a,b,c,d) (((a)<<24)+((b)<<16)+((c)<<8)+(d))
// ----------------------------------------------------------------
#define SM_NONE 0
#define SM_8E2  1
#define SM_8O2  2
#define SM_8N2  3

#define SM_MAX  4
#define SM_MASK 0x1F
#define SM_1SB  0x80

#define SM_DIRECT   0
#define SM_INDIRECT 1

#define ISO_FREQ 3571200 // Hz

struct {
  int SerMode;
  int SendTO, ReceiveTO; // values in ms
  }CardConfig;

struct StatusMsg {
  unsigned char sb[SB_LEN];
  char *message; 
  int retval;
  };

typedef struct Atr {
  int T, F, N, WI, BWI, CWI, TA1, Tspec;
  float D;
  int wwt, bwt;
  int atrLen, histLen;
  int convention;
  unsigned char atr[MAX_ATR_LEN];
  unsigned char hist[MAX_HIST];
} _atr;

static const int Ftable[16] = {
  372,372,558,744,1116,1488,1860,0,0,512,768,1024,1536,2048,0,0
  };

static const float Dtable[16] = {
  0.0,1.0,2.0,4.0,8.0,16.0,0.0,0.0,
  0.0,0.0,0.5,0.25,0.125,0.0625,0.03125,0.015625
  };
  
#define NEED(x) { \
                if(len+(x)>Atr->atrLen) { \
                    printf("SHORT ATR"); \
                    return 0; \
                    } \
                }


#endif
