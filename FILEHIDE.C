
#include <stdio.h>
#include <dos.h>

void main(int argc, char **argv)
{
int mode=0,i;
union REGS regs;
struct SREGS segs;

puts("File Hide        version 1.00      June 1992\n");
puts("Copyrite SOFTNET computer services .\n");
if(argc < 3)
   puts("usage: FileHide <file name> <mode ([+-][RASH])>\n");
for(i=2;i<=argc-1;i++)
  {
   if(argv[i][0] == '+')
     {
	if(toupper(argv[i][1]) == 'R')
	   mode = mode | 1;
	if(toupper(argv[i][1]) == 'A')
	   mode = mode | 0x20;
	if(toupper(argv[i][1]) == 'S')
	   mode = mode | 4;
	if(toupper(argv[i][1]) == 'H')
	   mode = mode | 2;
     }
   if(argv[i][0] == '-')
     {
	if(toupper(argv[i][1]) == 'R')
	   mode = mode & 0xfe;
	if(toupper(argv[i][1]) == 'A')
	   mode = mode & 0xdf;
	if(toupper(argv[i][1]) == 'S')
	   mode = mode & 0xfb;
	if(toupper(argv[i][1]) == 'H')
	   mode = mode & 0xfd;
     }
  }

regs.x.dx = FP_OFF(argv[1]);
segs.ds   = FP_SEG(argv[1]);
regs.x.cx = mode;
regs.h.ah = 0x43;
regs.h.al = 0x01;

int86x(0x21,&regs,&regs,&segs);
switch (regs.x.ax)
  {
   case 1 : puts("Invalid function code\n"); break;
   case 2 : puts("File not found\n"); break;
   case 3 : puts("Path not found\n"); break;
   case 5 : puts("Access denied\n"); break;
   otherwise: break;
  }

}
