
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define IBMPC
#include <dodtype.h>

SYSTEMPARAMS SysParam;
POOLINFOTYPE PoolInfo;
DICINFOTYPE  DicInfo  = {0,0,0,0,0L,0L,0L,0L,0L,0L,0L,0};
SYSATTROFFSETTYPE SysAttrOffset;
SYSATTRTYPE SysAttr;


SHORT *pDicOrd        = NULL;
WORD  *pVarOffset     = NULL;
SHORT *pVarSameAs     = NULL;
SHORT *pVarValidation = NULL;
BYTE  *pVarType       = NULL;
BYTE  *pVarMask       = NULL;
BYTE  *pVarOccurs     = NULL;

SHORT InfoSize;
SHORT OrdSize;
SHORT TypeSize;
SHORT MaskSize;
SHORT OccursSize;
SHORT SameAsSize;
SHORT OffsetSize;
SHORT ValSize;


static FILE *sysfile;
static long filesize;

PMENUARRAY MenuNode = {NULL};

void print_SysParam( void );
void print_PoolInfo( int i );
void print_ExtraChunk( int i );
void print_DicInfo( void );
void print_OtherDicInfo ( void );
void print_MenuNode ( int i );
PNODERECORD pMenuNode ( SHORT Node );
void print_SysAttrOffset ( int i );
void print_SysAttr(int j);

void my_fseek(long offset)
{
   if (fseek(sysfile,offset,SEEK_SET) != 0) exit(255);
   printf(" - - - - - - - - - - - - - - - - - - - - - - - - - - offset = %lu\n",offset);
}

void main ( int argc, char *argv[] )
{
   int     i,j;
   SHORT   LastTableSize;
   SHORT   LastTable;
   SHORT   Nodes;
   SHORT   Node;

   if (argc < 2) exit(0);

   sysfile = fopen(argv[1],"rb");
   if (sysfile == NULL) exit(1);

/* END OF FILE */
   my_fseek(0);
   fread(&filesize,sizeof(long),1,sysfile);
   printf("filesize = %lu\n",filesize );

/* SYSPARAM */
   my_fseek(4);
   fread(&SysParam,sizeof(SysParam),1,sysfile);
   print_SysParam();

/* EXTRA CHUNKS */
/*
   for (i=0;i<SysParam.b.nExtraParams;i++)
   {
      my_fseek(4+sizeof(SysParam)+i*100);
      fread( SysParam.a.ExtraChunk.s[i].t, 100,1,sysfile );
      print_ExtraChunk(i);
   }
*/
/* POOLINFO */
   if (SysParam.b.PoolInfo > 0)
   {
      my_fseek(SysParam.b.PoolInfo);
      fread(&PoolInfo,sizeof(POOLINFOREC),SysParam.b.nPools,sysfile);
      for (i=0;i<SysParam.b.nPools;i++)
         print_PoolInfo(i);
   }


/* EXTRA POOLS */
   if (SysParam.b.nExtraPools > 0)
   {
      my_fseek(SysParam.b.PoolInfo+sizeof(POOLINFOREC)*SysParam.b.nPools);
      fread(&PoolInfo[MAXPOOLS],sizeof(POOLINFOREC),SysParam.b.nExtraPools,sysfile);
      for (i=0;i<SysParam.b.nExtraPools;i++)
         print_PoolInfo(MAXPOOLS+i);
   }

/* DIC INFO */

   my_fseek(SysParam.b.DicInfo);
   fread(&DicInfo,sizeof(DicInfo),1,sysfile);
   print_DicInfo();


   OrdSize        = DicInfo.nOrder  * 2;
   OffsetSize     = DicInfo.nExtern * 2;
   SameAsSize     = DicInfo.nSameAs * 2;
   ValSize        = (DicInfo.nIntern + DicInfo.nExtern + DicInfo.nSameAs) * 2;
   TypeSize       = DicInfo.nIntern + DicInfo.nExtern;
   MaskSize       = DicInfo.nIntern + DicInfo.nExtern + DicInfo.nSameAs;
   OccursSize     = DicInfo.nIntern + DicInfo.nExtern;

   InfoSize       = OrdSize + OffsetSize + SameAsSize + ValSize + TypeSize + MaskSize + OccursSize;

   pDicOrd        = (void *)((char *)calloc(1,InfoSize+2));
   pVarOffset     = (void *)((char *)pDicOrd        + OrdSize);
   pVarSameAs     = (void *)((char *)pVarOffset     + OffsetSize);
   pVarValidation = (void *)((char *)pVarSameAs     + SameAsSize);
   pVarType       = (void *)((char *)pVarValidation + ValSize);
   pVarMask       = (void *)((char *)pVarType       + TypeSize);
   pVarOccurs     = (void *)((char *)pVarMask       + MaskSize);

   my_fseek(DicInfo.oOrder     ); fread(pDicOrd       , OrdSize   ,1,sysfile);
   my_fseek(DicInfo.oOffset    ); fread(pVarOffset    , OffsetSize,1,sysfile);
   my_fseek(DicInfo.oSameAs    ); fread(pVarSameAs    , SameAsSize,1,sysfile);
   my_fseek(DicInfo.oValidation); fread(pVarValidation, ValSize   ,1,sysfile);
   my_fseek(DicInfo.oType      ); fread(pVarType      , TypeSize  ,1,sysfile);
   my_fseek(DicInfo.oMask      ); fread(pVarMask      , MaskSize  ,1,sysfile);
   my_fseek(DicInfo.oOccurs    ); fread(pVarOccurs    , OccursSize,1,sysfile);

   print_OtherDicInfo();


/* NODES */

   LastTable     =  SysParam.b.LastNode / 1024;
   LastTableSize = (SysParam.b.LastNode % 1024) + 1;

   for (i=0; i<=LastTable; i++)
      if ( i < LastTable ) MenuNode [i] = calloc( 1,(WORD)MENUNODESIZE * (WORD)1024         );
      else                 MenuNode [i] = calloc( 1,MENUNODESIZE * LastTableSize);


   Nodes = SysParam.b.LastNode + 1;
   Node  = 0;

   my_fseek(4 + 82 + (SysParam.b.nExtraParams * 100));
   while( Nodes > 0 )
   {
      if( Nodes > 512 )
      {
          fread(pMenuNode ( Node ) , 512*MENUNODESIZE ,1,sysfile);
          Nodes -= 512;
          Node  += 512;
      }
      else
      {
          fread(pMenuNode ( Node ) , Nodes*MENUNODESIZE ,1,sysfile);
          Nodes = 0;
      }
   }



//   for (Node=0;Node<SysParam.b.LastNode + 1;Node++)
//      print_MenuNode(Node);

/* SYSATTR */

   my_fseek(SysParam.b.SysAttrOffset);
   fread(&SysAttrOffset,sizeof(SysAttrOffset),1,sysfile);
   for (i=0;i<MAXHW;i++)
   {
      print_SysAttrOffset(i);
      if (SysAttrOffset[i] != 0)
      {
         my_fseek(SysAttrOffset[i]);
         fread(&SysAttr,sizeof(SysAttr),1,sysfile);
         for (j=0;j<2;j++)
            print_SysAttr(j);
      }
   }


/*
   LoadPools();
*/

   fclose(sysfile);
}

void print_SysParam( void )
{
   printf("컴컴컴컴컴컴컴컴컴컴 SysParam 컴컴컴컴컴컴컴컴컴컴\n");
   printf("[  0] BYTE     BackGround          = 0x%2.2X\n",SysParam.b.BackGround          );
   printf("[  1] BYTE     MenuName            = 0x%2.2X\n",SysParam.b.MenuName            );
   printf("[  2] BYTE     Item                = 0x%2.2X\n",SysParam.b.Item                );
   printf("[  3] BYTE     FirstChar           = 0x%2.2X\n",SysParam.b.FirstChar           );
   printf("[  4] BYTE     Description         = 0x%2.2X\n",SysParam.b.Description         );
   printf("[  5] BYTE     ErrorMsg            = 0x%2.2X\n",SysParam.b.ErrorMsg            );
   printf("[  6] BYTE     Selector            = 0x%2.2X\n",SysParam.b.Selector            );
   printf("[  7] BOOLEAN  fUsesUsageCheck     = %d\n",SysParam.b.fUsesUsageCheck     );
   printf("[  8] WORD     HelpKey             = %u\n",SysParam.b.HelpKey             );
   printf("[ 10] WORD     RecordKey           = %u\n",SysParam.b.RecordKey           );
   printf("[ 12] WORD     PauseKey            = %u\n",SysParam.b.PauseKey            );
   printf("[ 14] WORD     HomeKey             = %u\n",SysParam.b.HomeKey             );
   printf("[ 16] WORD     PrintKey            = %u\n",SysParam.b.PrintKey            );
   printf("[ 18] WORD     QuickKey            = %u\n",SysParam.b.QuickKey            );
   printf("[ 20] WORD     EditKey             = %u\n",SysParam.b.EditKey             );
   printf("[ 22] WORD     BreakKey            = %u\n",SysParam.b.BreakKey            );
   printf("[ 24] WORD     FieldHelpKey        = %u\n",SysParam.b.FieldHelpKey        );
   printf("[ 26] WORD     SysVersion          = %u\n",SysParam.b.SysVersion          );
   printf("[ 28] BYTE     HardWareType        = %d\n",SysParam.b.HardWareType        );
   printf("[ 29] WORD     bMapSpec            = %u\n",SysParam.b.bMapSpec            );
   printf("[ 31] LONG     LineWidth           = %lu\n",SysParam.b.LineWidth           );
   printf("[ 35] SHORT    MaxLevels           = %d\n",SysParam.b.MaxLevels           );
   printf("[ 37] WORD     MapFileName         = %u\n",SysParam.b.MapFileName         );
   printf("[ 39] BYTE     nVarCase            = %d\n",SysParam.b.nVarCase            );
   printf("[ 40] LONG     DicInfo             = %lu\n",SysParam.b.DicInfo             );
   printf("[ 44] BYTE     nPools              = %d\n",SysParam.b.nPools              );
   printf("[ 45] BYTE     NotUsed1            = %d\n",SysParam.b.NotUsed1             );
   printf("[ 46] LONG     PoolInfo            = %lu\n",SysParam.b.PoolInfo            );
   printf("[ 50] WORD     GenVersion          = %d\n",SysParam.b.GenVersion          );
   printf("[ 52] WORD     SystemBufferSize    = %u\n",SysParam.b.SystemBufferSize    );
   printf("[ 54] WORD     StackSize           = %u\n",SysParam.b.StackSize           );
   printf("[ 56] BOOLEAN  DynamicEdit         = %d\n",SysParam.b.DynamicEdit         );
   printf("[ 57] BOOLEAN  SeparateHelp        = %d\n",SysParam.b.SeparateHelp        );
   printf("[ 58] SHORT    MaxMenus1           = %d\n",SysParam.b.MaxMenus1           );
   printf("[ 60] BYTE     nExtraPools         = %d\n",SysParam.b.nExtraPools         );
   printf("[ 61] BYTE     nExtraParams        = %d\n",SysParam.b.nExtraParams        );
   printf("[ 62] WORD     Initialise          = %u\n",SysParam.b.Initialise          );
   printf("[ 64] WORD     PreKey              = %u\n",SysParam.b.PreKey              );
   printf("[ 66] WORD     DuringKey           = %u\n",SysParam.b.DuringKey           );
   printf("[ 68] WORD     PostKey             = %u\n",SysParam.b.PostKey             );
   printf("[ 70] WORD     Spare               = %u\n",SysParam.b.DoddleVersion       );
   printf("[ 72] SHORT    LastNode            = %d\n",SysParam.b.LastNode            );
   printf("[ 74] LONG     QuickScreen         = %lu\n",SysParam.b.QuickScreen         );
   printf("[ 78] LONG     FirstScreen         = %lu\n",SysParam.b.FirstScreen         );

   printf("[ 82] BYTE     DefaultScreenAttr   = 0x%2.2X\n",SysParam.b.DefaultScreenAttr   );
   printf("[ 83] BYTE     u.InitialiseSeg     = %d\n",SysParam.b.u.InitialiseSeg     );
   printf("[ 83] BYTE     u.PreKeySeg         = %d\n",SysParam.b.u.PreKeySeg         );
   printf("[ 84] BYTE     DuringKeySeg        = %d\n",SysParam.b.DuringKeySeg        );
   printf("[ 85] BYTE     PostKeySeg          = %d\n",SysParam.b.PostKeySeg          );
   printf("[ 86] BYTE     MapFileNameSeg      = %d\n",SysParam.b.MapFileNameSeg      );
   printf("[ 87] BOOLEAN  HideDictionaryItems = %d\n",SysParam.b.HideDictionaryItems );
   printf("[ 88] BYTE     PoolLocation        = %d\n",SysParam.b.PoolLocation        );
   printf("[ 89] SHORT    MaxUserMenuDepth    = %d\n",SysParam.b.MaxUserMenuDepth    );
   printf("[ 91] BYTE     nLanguages          = %d\n",SysParam.b.nLanguages          );
   printf("[ 92] WORD     LanguageName[ 0]    = %u\n",SysParam.b.LanguageName[ 0]    );
   printf("[ 94] WORD     LanguageName[ 1]    = %u\n",SysParam.b.LanguageName[ 1]    );
   printf("[ 96] WORD     LanguageName[ 2]    = %u\n",SysParam.b.LanguageName[ 2]    );
   printf("[ 98] WORD     LanguageName[ 3]    = %u\n",SysParam.b.LanguageName[ 3]    );
   printf("[100] WORD     LanguageName[ 4]    = %u\n",SysParam.b.LanguageName[ 4]    );
   printf("[102] WORD     LanguageName[ 5]    = %u\n",SysParam.b.LanguageName[ 5]    );
   printf("[104] WORD     LanguageName[ 6]    = %u\n",SysParam.b.LanguageName[ 6]    );
   printf("[106] WORD     LanguageName[ 7]    = %u\n",SysParam.b.LanguageName[ 7]    );
   printf("[108] WORD     LanguageName[ 8]    = %u\n",SysParam.b.LanguageName[ 8]    );
   printf("[110] WORD     LanguageName[ 9]    = %u\n",SysParam.b.LanguageName[ 9]    );
   printf("[112] BYTE     LanguageSeg[ 0]     = %d\n",SysParam.b.LanguageSeg[ 0]     );
   printf("[113] BYTE     LanguageSeg[ 1]     = %d\n",SysParam.b.LanguageSeg[ 1]     );
   printf("[114] BYTE     LanguageSeg[ 2]     = %d\n",SysParam.b.LanguageSeg[ 2]     );
   printf("[115] BYTE     LanguageSeg[ 3]     = %d\n",SysParam.b.LanguageSeg[ 3]     );
   printf("[116] BYTE     LanguageSeg[ 4]     = %d\n",SysParam.b.LanguageSeg[ 4]     );
   printf("[117] BYTE     LanguageSeg[ 5]     = %d\n",SysParam.b.LanguageSeg[ 5]     );
   printf("[118] BYTE     LanguageSeg[ 6]     = %d\n",SysParam.b.LanguageSeg[ 6]     );
   printf("[119] BYTE     LanguageSeg[ 7]     = %d\n",SysParam.b.LanguageSeg[ 7]     );
   printf("[120] BYTE     LanguageSeg[ 8]     = %d\n",SysParam.b.LanguageSeg[ 8]     );
   printf("[121] BYTE     LanguageSeg[ 9]     = %d\n",SysParam.b.LanguageSeg[ 9]     );
   printf("[122] LONG     MLoffset[ 0]        = %lu\n",SysParam.b.MLoffset[ 0]        );
   printf("[126] LONG     MLoffset[ 1]        = %lu\n",SysParam.b.MLoffset[ 1]        );
   printf("[130] LONG     MLoffset[ 2]        = %lu\n",SysParam.b.MLoffset[ 2]        );
   printf("[134] LONG     MLoffset[ 3]        = %lu\n",SysParam.b.MLoffset[ 3]        );
   printf("[138] LONG     MLoffset[ 4]        = %lu\n",SysParam.b.MLoffset[ 4]        );
   printf("[142] LONG     MLoffset[ 5]        = %lu\n",SysParam.b.MLoffset[ 5]        );
   printf("[146] LONG     MLoffset[ 6]        = %lu\n",SysParam.b.MLoffset[ 6]        );
   printf("[150] LONG     MLoffset[ 7]        = %lu\n",SysParam.b.MLoffset[ 7]        );
   printf("[154] LONG     MLoffset[ 8]        = %lu\n",SysParam.b.MLoffset[ 8]        );
   printf("[158] LONG     MLoffset[ 9]        = %lu\n",SysParam.b.MLoffset[ 9]        );
   printf("[162] BYTE     nZones              = %d\n",SysParam.b.nZones              );
   printf("[163] BYTE     nHardWare           = %d\n",SysParam.b.nHardWare           );
   printf("[164] LONG     SysAttrOffset       = %lu\n",SysParam.b.SysAttrOffset       );
   printf("[168] LONG     ZoneDefOffset       = %lu\n",SysParam.b.ZoneDefOffset       );
   printf("[172] BYTE     Compiler            = %d\n",SysParam.b.Compiler            );
   printf("[173] BYTE     OperatingSystem     = %d\n",SysParam.b.OperatingSystem     );
   printf("[174] LONG     IdIndexOffset       = %lu\n",SysParam.b.IdIndexOffset      );
   printf("[178] BOOLEAN  Blinker             = %d\n",SysParam.b.Blinker            );
   printf("[179] BOOLEAN  Changed             = %d\n",SysParam.b.Changed            );
   printf("[180] BYTE     NotUsed3[0]         = %d\n",SysParam.b.NotUsed3[0]        );
   printf("[180] BYTE     NotUsed3[1]         = %d\n",SysParam.b.NotUsed3[1]        );

   /*
   printf("[180] BYTE     NextSysParam        = %d\n",SysParam.b.NextSysParam        );
   printf("[175] BYTE     UnUsed[0]           = %d\n",SysParam.b.UnUsed[0]           );
   printf("[176] BYTE     UnUsed[1]           = %d\n",SysParam.b.UnUsed[1]           );
   printf("[177] BYTE     UnUsed[2]           = %d\n",SysParam.b.UnUsed[2]           );
   printf("[178] BYTE     UnUsed[3]           = %d\n",SysParam.b.UnUsed[3]           );
   printf("[179] BYTE     UnUsed[4]           = %d\n",SysParam.b.UnUsed[4]           );
   printf("[180] BYTE     UnUsed[5]           = %d\n",SysParam.b.UnUsed[5]           );
   printf("[181] BYTE     EndSysParam         = %d\n",SysParam.b.EndSysParam         );
   */
#if 0
/*[182]*/      WORD                    GUI_ControlBits;        /* 0x0001 GUI_DOSCOMPATIBLESYSTEM                             */
                                                               /* 0x0002 GUI_DOSCOMPATIBLEHELP                               */
                                                               /* 0x0004 GUI_SCREENSWOPBW                                    */
                                                               /* 0x0008 GUI_MENUBUTTONS                                     */
                                                               /* 0x0010 GUI_MENUADJUSTSIZEFORBUTTONS                        */
                                                               /* 0x0020 GUI_STATUSLINEENABLED                               */
                                                               /* 0x0040 GUI_SMARTICONSENABLED                               */
                                                               /* 0x0080 Not used                                            */
                                                               /* 0x0100 Not used                                            */
                                                               /* 0x0200 Not used                                            */
                                                               /* 0x0400 Not used                                            */
                                                               /* 0x0800 Not used                                            */
                                                               /* 0x1000 Not used                                            */
                                                               /* 0x2000 Not used                                            */
                                                               /* 0x4000 Not used                                            */
                                                               /* 0x8000 Not used                                            */
/*[184]*/      SHORT                   GUI_MenuButtonIncrementTop;    /*   >=0, eg 5                                           */
/*[186]*/      SHORT                   GUI_MenuButtonIncrementLeft;   /*   >=0, eg 5                                           */
/*[188]*/      SHORT                   GUI_MenuButtonIncrementBottom; /*   >=0, eg 20                                          */
/*[190]*/      SHORT                   GUI_MenuButtonIncrementRight;  /*   >=0, eg 10                                          */
/*[192]*/      SHORT                   GUI_MenuBorders;        /* -2 = Force Windows menus                                    */
                                                               /* -1 = Force no border menus                                  */
                                                               /*  0 = No borders                                             */
                                                               /*  1 = Thin borders                                           */
                                                               /*  2 = Thin borders with caption                              */
                                                               /*  3 = Thick Borders (sizable)                                */
                                                               /*  4 = Thick Borders (sizable) with caption                   */
                                                               /*  5 = Thick Borders (sizable) with caption and min/max boxes */
/*[194]*/      SHORT                   GUI_ScreenBorders;      /*  0 = No borders                                             */
                                                               /*  1 = Thin borders                                           */
                                                               /*  2 = Thin borders with caption                              */
                                                               /*  3 = Thick Borders (sizable)                                */
                                                               /*  4 = Thick Borders (sizable) with caption                   */
                                                               /*  5 = Thick Borders (sizable) with caption and min/max boxes */
/*[196]*/      SHORT                   GUI_StatusLineHeight;   /*  >=0,  eg 20                                                */
/*[198]*/      CHAR                    GUI_SmartIconSequence[16]; /* Smarticons sequence eg 1,2,3,4,5,6,7,8,9,-1              */
/*[214]*/      SHORT                   GUI_FrameBorders;       /*  0 = No borders                                             */
                                                               /*  1 = Thin borders                                           */
                                                               /*  2 = Thin borders, caption                                  */
                                                               /*  3 = Thin borders, caption, sysmenu, minbox                 */
                                                               /*  4 = Thin borders, caption, sysmenu, minbox, menu           */
                                                               /*  5 = Thick Borders (sizable), caption                       */
                                                               /*  6 = Thick Borders (sizable), caption, sysmenu, min/max boxes */
                                                               /*  7 = Thick Borders (sizable), caption, sysmenu, min/max boxes, menu */
/*[216]*/      LONG                    pIconPoolOffset;
/*[220]*/      LONG                    pFontPoolOffset;
/*[224]*/      LONG                    pBmpPoolOffset;
/*[228]*/      LONG                    pCurPoolOffset;
/*[232]*/      BYTE                    NotUsed4[50];
#endif
}

void print_ExtraChunk( int i )
{
   int j;
   printf("컴컴컴컴컴컴컴컴컴컴 ExtraChunk %d 컴컴컴컴컴컴컴컴컴컴\n",i);

   for (j=0;j<100;j++)
      printf("%c",SysParam.a.ExtraChunk.s[i].t[j]);
   printf("\n");
}

void print_PoolInfo( int i )
{
   printf("컴컴컴컴컴컴컴컴컴컴 PoolInfo %d 컴컴컴컴컴컴컴컴컴컴\n",i);
   printf("[  0] WORD     MaxPoolSize         = %u\n",PoolInfo[i].MaxPoolSize          );
   printf("[  2] WORD     PoolSize            = %u\n",PoolInfo[i].PoolSize             );
   printf("[  4] LONG     PoolOffset          = %lu\n",PoolInfo[i].PoolOffset           );
   printf("[  8] LONG     IndexOffset         = %lu\n",PoolInfo[i].IndexOffset          );
   printf("[ 12] SHORT    IndexSize           = %d\n",PoolInfo[i].IndexSize            );
   printf("[ 14] WORD     PoolName            = %u\n",PoolInfo[i].PoolName             );
}


void print_DicInfo( void )
{
   printf("컴컴컴컴컴컴컴컴컴컴 DicInfo 컴컴컴컴컴컴컴컴컴컴\n");
   printf("      SHORT    nIntern             = %d\n" ,DicInfo.nIntern               );
   printf("      SHORT    nExtern             = %d\n" ,DicInfo.nExtern               );
   printf("      SHORT    nSameAs             = %d\n" ,DicInfo.nSameAs               );
   printf("      SHORT    nOrder              = %d\n" ,DicInfo.nOrder                );
   printf("      LONG     oOrder              = %lu\n",DicInfo.oOrder                );
   printf("      LONG     oOffset             = %lu\n",DicInfo.oOffset               );
   printf("      LONG     oSameAs             = %lu\n",DicInfo.oSameAs               );
   printf("      LONG     oValidation         = %lu\n",DicInfo.oValidation           );
   printf("      LONG     oType               = %lu\n",DicInfo.oType                 );
   printf("      LONG     oMask               = %lu\n",DicInfo.oMask                 );
   printf("      LONG     oOccurs             = %lu\n",DicInfo.oOccurs               );
   printf("      WORD     SegSize             = %u\n" ,DicInfo.SegSize               );
}


void print_OtherDicInfo ( void )
{
   int i;
   for (i=0;i<OrdSize   /2;i++) printf("pDicOrd       [%d] = %d\n",i,pDicOrd       [i]);
   for (i=0;i<OffsetSize/2;i++) printf("pVarOffset    [%d] = %u\n",i,pVarOffset    [i]);
   for (i=0;i<SameAsSize/2;i++) printf("pVarSameAs    [%d] = %d\n",i,pVarSameAs    [i]);
   for (i=0;i<ValSize   /2;i++) printf("pVarValidation[%d] = %d\n",i,pVarValidation[i]);
   for (i=0;i<TypeSize    ;i++) printf("pVarType      [%d] = %d\n",i,pVarType      [i]);
   for (i=0;i<MaskSize    ;i++) printf("pVarMask      [%d] = %d\n",i,pVarMask      [i]);
   for (i=0;i<OccursSize  ;i++) printf("pVarOccurs    [%d] = %d\n",i,pVarOccurs    [i]);
}

PNODERECORD pMenuNode ( SHORT Node )
{
   return (     &    (*MenuNode[Node / 1024])[Node % 1024]      );
}

void print_MenuNode ( int i )
{
   printf("컴컴컴컴컴컴컴컴컴컴 MenuNode %d 컴컴컴컴컴컴컴컴컴컴\n",i);
   printf("BYTE  a.MenuType                  = %d\n",pMenuNode(i)->a.MenuType      );
   printf("CHAR  a.Key                       = %c\n",pMenuNode(i)->a.Key           );
   printf("WORD  a.Name                      = %u\n",pMenuNode(i)->a.Name          );
   printf("SHORT a.NamePos.c                 = %d\n",pMenuNode(i)->a.NamePos.c     );
   printf("SHORT a.NamePos.l                 = %d\n",pMenuNode(i)->a.NamePos.l     );
   printf("SHORT a.MenuPos.c                 = %d\n",pMenuNode(i)->a.MenuPos.c     );
   printf("SHORT a.MenuPos.l                 = %d\n",pMenuNode(i)->a.MenuPos.l     );
   printf("BYTE  a.MenuSize.c     (b.dummy1 )= %d\n",pMenuNode(i)->a.MenuSize.c    ); //BYTE   b.dummy1
   printf("BYTE  a.MenuSize.align (b.NameSeg)= %d\n",pMenuNode(i)->a.MenuSize.align); //BYTE   b.NameSeg
   printf("BYTE  a.MenuSize.l     (b.dummy2 )= %d\n",pMenuNode(i)->a.MenuSize.l    ); //BYTE   b.dummy2
   printf("BYTE  a.MenuSize.dummy (b.DescSeg)= %d\n",pMenuNode(i)->a.MenuSize.dummy); //BYTE   b.DescSeg
   printf("WORD  a.Desc                      = %u\n",pMenuNode(i)->a.Desc          );
   printf("SHORT a.NextPeer                  = %d\n",pMenuNode(i)->a.NextPeer      );
   printf("SHORT a.PrevPeer                  = %d\n",pMenuNode(i)->a.PrevPeer      );
   printf("SHORT a.Child                     = %d\n",pMenuNode(i)->a.Child         );
   printf("SHORT a.Parent                    = %d\n",pMenuNode(i)->a.Parent        );
   printf("LONG  a.Help                      = %lu\n",pMenuNode(i)->a.Help          );
   printf("LONG  a.Screen                    = %lu\n",pMenuNode(i)->a.Screen        );
   printf("SHORT a.Action.Ptr                = %d\n",pMenuNode(i)->a.Action.Ptr    );
   printf("SHORT a.Action.Tag                = %d\n",pMenuNode(i)->a.Action.Tag    );
   printf("SHORT a.ScreenData.Ptr            = %d\n",pMenuNode(i)->a.ScreenData.Ptr);
   printf("SHORT a.ScreenData.Tag            = %d\n",pMenuNode(i)->a.ScreenData.Tag);
   printf("SHORT a.Validation.Ptr            = %d\n",pMenuNode(i)->a.Validation.Ptr);
   printf("SHORT a.Validation.Tag            = %d\n",pMenuNode(i)->a.Validation.Tag);
}

void print_SysAttrOffset ( int i )
{
   printf("컴컴컴컴컴컴컴컴컴컴 SysAttrOffset %d 컴컴컴컴컴컴컴컴컴컴\n",i);
   printf("LONG SysAttrOffset[%d]  = %lu\n",i,SysAttrOffset[i] );
}

void print_SysAttr(int j)
{
   int k;
   printf("컴컴컴컴컴컴컴컴컴컴 SysAttr %d 컴컴컴컴컴컴컴컴컴컴\n",j);
   printf("BYTE SysAttr[%d].Menu.a.BackGround  = 0x%2.2X\n",j,SysAttr[j].Menu.a.BackGround );
   printf("BYTE SysAttr[%d].Menu.a.MenuName    = 0x%2.2X\n",j,SysAttr[j].Menu.a.MenuName   );
   printf("BYTE SysAttr[%d].Menu.a.Item        = 0x%2.2X\n",j,SysAttr[j].Menu.a.Item       );
   printf("BYTE SysAttr[%d].Menu.a.FirstChar   = 0x%2.2X\n",j,SysAttr[j].Menu.a.FirstChar  );
   printf("BYTE SysAttr[%d].Menu.a.Description = 0x%2.2X\n",j,SysAttr[j].Menu.a.Description);
   printf("BYTE SysAttr[%d].Menu.a.ErrorMsg    = 0x%2.2X\n",j,SysAttr[j].Menu.a.ErrorMsg   );
   printf("BYTE SysAttr[%d].Menu.a.Selector    = 0x%2.2X\n",j,SysAttr[j].Menu.a.Selector   );
   printf("BYTE SysAttr[%d].Menu.a.NotUsed     = 0x%2.2X\n",j,SysAttr[j].Menu.a.NotUsed    );
   for (k=0;k<MAXZONE+1;k++)
   {
      printf("컴컴컴컴컴컴컴컴컴컴 SysAttr %d Zone %d컴컴컴컴컴컴컴컴컴컴\n",j,k);
      printf("BYTE SysAttr[%d].Scr[%d].a.Txt        = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Txt       );
      printf("BYTE SysAttr[%d].Scr[%d].a.Single     = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Single    );
      printf("BYTE SysAttr[%d].Scr[%d].a.Thick      = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Thick     );
      printf("BYTE SysAttr[%d].Scr[%d].a.Double     = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Double    );
      printf("BYTE SysAttr[%d].Scr[%d].a.Disp       = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Disp      );
      printf("BYTE SysAttr[%d].Scr[%d].a.Selected   = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Selected  );
      printf("BYTE SysAttr[%d].Scr[%d].a.UnSelected = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.UnSelected);
      printf("BYTE SysAttr[%d].Scr[%d].a.Error      = 0x%2.2X\n",j,k,SysAttr[j].Scr[k].a.Error     );
   }
}
