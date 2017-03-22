/*          RestoreAndExpand ( SystemFile,
                             ByteFilePos,
                             pSaveData,
                             cbSaveData,
                             Inc,
                             MaxPos1,
                             (GPSHORT)pBytesExpandedRet );

*/

#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <dodtype.h>
#include <move.h>
#include <mam.h>
#include <bytefile.h>
#include <compact.h>

#define MINBYTEFILEBUFFERSIZE     1024 + BUFFERDEFSIZE + BUFFERCTLSIZE

FILE *fSys;
FILE *fHelp;
char         Chunk[101];
SYSTEMPARAMS SysParams;
NODERECORD   NodeRec;

char DosScrn[4000];
char far *vio=0xb8000000L;
char Version[36]  ={"SYSHELP Version 1.00   - June 1992."};
char CopyWrit[42]  ={"Copyright SOFTNET computer services 1992."};
int  quiet=1,num_done=0;
char name1[15];

char screen[8000];

PBYTEFILECTL BC;


int ExtractScreen (long offset)
{
    int read,i,j;
    long where = offset;
    int x,y,z,a;
    int x1,y1,x2,y2,bytes;

    ReadByteFile ( BC, &where ,&x, 2);
    ReadByteFile ( BC, &where ,&y, 2);
    ReadByteFile ( BC, &where ,&z, 2);
    ReadByteFile ( BC, &where ,&a, 2);

    x1    = (z % 80) + 1;
    y1    = (z / 80) + 1;
    x2    = (a % 80) + 1;
    y2    = (a / 80) + 1;
    bytes = (x2-x1+1) * (y2-y1+1);


    RestoreAndExpand ( BC,
                       &where,
                       screen,
                       bytes*2,
                       1,
                       0,
                       &read );
    x1 -=1;
    y1 -=1;
    x2 -=1;
    y2 -=1;

    if(!quiet)
    {
       for(i=0;i<4000;i++)
         *(vio+i) = 0;

       for(i=y1,a=0;i<y2+1;i++)
         for(j=x1;j<x2+1;j++)
           *(vio+((i*160)+j*2)) = screen[a++];

       for(i=y1;i<y2+1;i++)
         for(j=x1;j<x2+1;j++)
           *(vio+((i*160)+(j*2)+1)) = screen[a++];

    }

    return 0;

}


void main(int argc ,char **argv)
{
   int i;
   long FilePos=0;

   printf("\n%s\n",Version);
   printf("%s\n",CopyWrit);
   if(argc <3)
   {
      printf("\nUSAGE :  SYSHELP <doddle system name> <Output file> </v> \n");
      exit(0);
   }
   if((fHelp = fopen(argv[2],"wb")) == 0)
   {
      printf("Error opening %s\n",argv[2]);
      exit(0);
   }

   if (argc > 3)
      if(strnicmp(argv[3],"/V",2) ==0)
         quiet =0;
   strcpy(name1,argv[1]);
   if (strcmp(&name1[strlen(name1)-4],".sys") != 0)
      strcat(name1,".sys");

   if( AllocMemorySL ( MINBYTEFILEBUFFERSIZE,(GPPOINTER)&BC ))
   {
      printf("Error allocating Buffer for file");
      exit(0);
   }

   BlockFillByte(0,(GPBYTE)BC,MINBYTEFILEBUFFERSIZE);

   OpenByteFile ( BC, MINBYTEFILEBUFFERSIZE, 1024,
                  name1, strlen(name1), 0 );
   if ( BC->ERRS != 0 )
   {
      printf("Error opening file %s\n",name1);
      exit(0);
   }

   for(i=0;i<4000;DosScrn[i] = *(vio+i++));

   FilePos = 4;
   ReadByteFile ( BC, &FilePos, &SysParams,sizeof(SYSTEMPARAMS) );

   if(SysParams.b.nExtraParams > 1)
      for(i=1;i<SysParams.b.nExtraParams;i++)
        ReadByteFile ( BC, &FilePos, &Chunk,100 );

   printf("\nExtracting Help Screens %d.\n",SysParams.b.LastNode);
   for(i=0;i<SysParams.b.LastNode;i++)
   {
      ReadByteFile ( BC, &FilePos ,&NodeRec, sizeof(NODERECORD) );
      if(NodeRec.b.Help > 0)
      {
         if(ExtractScreen (NodeRec.b.Help))
         {
           if(quiet)
           {
              *(vio+i*2) = '.';
              *(vio+(i*2)+1) = 0x4f;
           }
         }
         else
         {
           if(quiet)
           {
              *(vio+i*2) = '.';
              *(vio+(i*2)+1) = 0x14;
           }
         }
         num_done++;
         //printf(".");
      }
      else
      {
         if(quiet)
         {
            *(vio+i*2) = '.';
            *(vio+(i*2)+1) = 0x1f;
         }
       if(!quiet)
         if(getch() == 0x1b)
            break;
      }
   }
   for(i=0;i<4000;*(vio+i) = DosScrn[i++]);
   CloseByteFile(BC);
   fclose(fHelp);
   printf("Extracted %d Help Screens .\n",num_done);
}
