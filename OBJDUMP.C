#include <stdlib.h>
#include <stdio.h>
#include <io.h>
#include <string.h>
#include <fcntl.h>
#include <malloc.h>

#pragma pack(1)

struct hdrtype
{
   unsigned char type;
   unsigned int  length;
};

union rectype
{
   char buffer[10000];
   struct
   {
      unsigned char length;
      char          modulename[1];
   } theadr;
   struct
   {
      unsigned char attrib;
      unsigned char class;
      char          comment[1];
   } coment;
};

void Usage(void);
void AlphaObj(void);
void DumpObj(void);
char *ObjRecordName( int n );

int Mode = 'D';
unsigned char CurrentByte;
unsigned char RecordType;
int ObjFileHandle;
int CurrentLineLength;
int ObjRecordNumber = 0;
int ObjRecordLength;
int ObjRecordOffset = 0;
char ASCIIEquiv[17];
char FormatStr[24];
char ObjFileName[129] = {0};
struct hdrtype hdr;
union rectype *buf;

int main (int argc, char *argv[])
{
   argc--;
   argv++;

   if (!argc) Usage();

   while(argc)
   {
      if (argv[0][0] == '-' || argv[0][0] == '/')
      {
         switch (toupper(argv[0][1]))
         {
            case 'D' : Mode = 'D'; break;
            case 'A' : Mode = 'A'; break;
            default  : printf("Invalid switch : -%c\n",toupper(argv[0][1]));
         }
      }
      else if (!ObjFileName[0]) strcpy(ObjFileName,argv[0]);
      else printf("Extra argument ignored : %s\n",argv[0]);
      argc--;
      argv++;
   }

   if (!ObjFileName[0]) Usage();

   ObjFileHandle = open(ObjFileName, O_BINARY);
   if (ObjFileHandle == -1)
   {
      printf("Cannot open object file\n");
      exit(1);
   }

   if (Mode == 'D') DumpObj();
   if (Mode == 'A') AlphaObj();

   close(ObjFileHandle);
   printf("\n%d object records\n", ObjRecordNumber);
   return(0);
}

void Usage(void)
{
   printf("USAGE: OBJDUMP [-|/switch] <ObjFileName>\n");
   printf("       switch : D = Dump (default)\n");
   printf("                A = Alpha\n");
   exit(1);
}

void DumpObj(void)
{
   while( read( ObjFileHandle, &CurrentByte, 1))
   {
      switch(ObjRecordOffset)
      {
         case 0 :
            printf("\nRecord %d:  %02Xh %s",++ObjRecordNumber, CurrentByte, ObjRecordName(CurrentByte));
            printf("\n%02X ", CurrentByte);
            RecordType = CurrentByte;
            ++ObjRecordOffset;
            break;
         case 1 :
            ObjRecordLength = CurrentByte;
            ++ObjRecordOffset;
            break;
         case 2 :
            ObjRecordLength += CurrentByte << 8;
            printf("%04X ", ObjRecordLength);
            CurrentLineLength = 0;
            memset ( ASCIIEquiv, '\0', 17 );
            ++ObjRecordOffset;
            break;
         default:
            printf("%02X ", CurrentByte);
            if (CurrentByte<0x20 || CurrentByte>0x7F) CurrentByte = '.';
            ASCIIEquiv[CurrentLineLength++] = CurrentByte;
            if (CurrentLineLength==16 || ObjRecordOffset == ObjRecordLength+2)
            {
               sprintf(FormatStr, "%%%ds%%s\n        ",3*(16-CurrentLineLength)+2);
               printf(FormatStr, " ", ASCIIEquiv);
               memset ( ASCIIEquiv, '\0', 17 );
               CurrentLineLength = 0;
            }
            if (++ObjRecordOffset == ObjRecordLength+3)
               ObjRecordOffset = 0;
           break;
      }
   }
   if (CurrentLineLength)
      printf("  %s", ASCIIEquiv);
}

void AlphaObj(void)
{
   buf = malloc(sizeof(union rectype));
   if (buf == NULL)
   {
      printf("Insufficient memory\n");
      exit(2);
   }

   while (read( ObjFileHandle, &hdr, 3))
   {
      printf("Record %4d : Type %02Xh %s : %04Xh (%6u) bytes\n",++ObjRecordNumber, hdr.type, ObjRecordName(hdr.type), hdr.length, hdr.length);

      read( ObjFileHandle, buf, hdr.length);
      switch(hdr.type)
      {
         case 0x80 :
            printf("MODULENAME=\"%*.*s\"\n",buf->theadr.length,buf->theadr.length,buf->theadr.modulename);
            break;
         case 0x88 :
            printf("ATTRIB=%2.2Xh   CLASS=%2.2X\n",buf->coment.attrib,buf->coment.class);
            printf("COMMENT=\"%*.*s\"\n",hdr.length-3,hdr.length-3,buf->coment.comment);
            break;
      }
   }
}

char *ObjRecordName( int n )
{
   int i;
   static struct
   {
      int RecordNumber;
      char *RecordName;
   }   RecordStruct[] = { 0x80, "THEADR",
                          0x88, "COMENT",
                          0x8A, "MODEND",
                          0x8C, "EXTDEF",
                          0x8E, "TYPDEF",
                          0x90, "PUBDEF",
                          0x94, "LINNUM",
                          0x96, "LNAMES",
                          0x98, "SEGDEF",
                          0x9A, "GRPDEF",
                          0x9C, "FIXUPP",
                          0xA0, "LEDATA",
                          0xA2, "LIDATA",
                          0xB0, "COMDEF",
                          0x00, "******" };
   int RecordTableSize = sizeof(RecordStruct)/sizeof(RecordStruct[0]);

   for (i=0;i<RecordTableSize-1; i++)
      if (RecordStruct[i].RecordNumber == n) break;
   return(RecordStruct[i].RecordName);
}
