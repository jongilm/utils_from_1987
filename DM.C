
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

unsigned char dos_screen[4000];
int dos_cur_x  = 0;
int dos_cur_y  = 0;
unsigned char far *VGA;

unsigned char data[2000];
int which_meg = 0;
int save_meg = 1;
int mem_max = 16;

typedef unsigned long ULONG;
typedef void far *FP;
#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))
#endif

#define GDT_SIZE 2048

#pragma pack(1)
struct gdt1
{
   unsigned int     seglim;
   unsigned int     lw_address;
   unsigned char    hw_address;
   unsigned char    acess;
   unsigned int     reserved;
};
#pragma pack()

struct gdt1 gdt[6];
char far *gdts;
int gdt_to_initialized=0;
int gdt_from_initialized=0;
ULONG HIMEM_Srce=0;
char GETDATA_Temp_Buf[4096];	    /* Do not change it is a temp buffer for the move */


void INIT(void)
{
  union REGS regs;
     regs.h.ah = 0x0f;
     int86(0x10,&regs,&regs);
     VGA = regs.h.al == 7 ? MK_FP(0xb000,0x0000)  : MK_FP(0xb800,0x0000);
}

void srcdest(void)
{
    char far *x1;
    ULONG addr=0;
    int x;
    unsigned char addrh;
    unsigned int addrl;

	save_meg = which_meg;
	x = which_meg;
	HIMEM_Srce = (unsigned long)x;
	HIMEM_Srce = HIMEM_Srce << 20;

	memset(gdt,0,sizeof(gdt));

	gdt[2].seglim	     = GDT_SIZE*2;
	gdt[2].acess	     = 0x93;
	gdt[2].reserved      = 0;

	gdt[3].seglim	     = GDT_SIZE*2;
	x1 = &GETDATA_Temp_Buf[0];
	addr =	FP_SEG(x1);
	addr = addr << 4;
	addr +=  FP_OFF(x1);
	addrh = (unsigned char)(addr>>16);
	addrl = (unsigned int)addr;
	gdt[3].lw_address    = addrl;
	gdt[3].hw_address    = addrh;	/* destination */
	gdt[3].acess	     = 0x93;
	gdt[3].reserved      = 0;
	gdt_to_initialized = 1;
	gdt_from_initialized = 0;
}

int Read_Data_From_HiMemory(ULONG offset,int size,char far *Data)
{
  union REGS r;
  struct SREGS s;
  register x,y,z,i;
  ULONG mem_src;

	if(save_meg != which_meg)
	   srcdest();
	mem_src = HIMEM_Srce;

	mem_src += offset;
	gdt[2].lw_address = (unsigned int)mem_src;
	gdt[2].hw_address = (unsigned char)(mem_src>>16);

	r.h.ah = 0x87;		   /*	This chunk of code    */
	r.x.cx = size;		   /*	gets a section of     */
	gdts = (char far *)gdt;    /*	memory from the       */
	s.es = FP_SEG(gdts);	   /*	card and puts it      */
	r.x.si = FP_OFF(gdts);	   /*	into  GETDATA_Temp_Buf*/
	int86x(0x15,&r,&r,&s);	   /*			      */
	if(r.x.cflag)		   /*			      */
	    return(r.h.ah);	   /*			      */
	memcpy(Data,GETDATA_Temp_Buf,size);
     return 0;

}

void save_dos_screen(void)
{
int i;
char far *vio;
union REGS r;

   vio = VGA;
   for(i=0;i<4000;i++)
      dos_screen[i] = *vio++;
   r.h.ah = 3;
   r.h.bh = 0;
   int86(0x10,&r,&r);
   dos_cur_x  = r.h.dh;
   dos_cur_y  = r.h.dl;

}
void restore_dos_screen(void)
{
int i;
char far *vio;
union REGS r;

   vio = VGA;
   for(i=0;i<4000;i++)
      *vio++ = dos_screen[i];
   r.h.ah = 2;
   r.h.bh = 0;
   r.h.dh = dos_cur_x;
   r.h.dl = dos_cur_y;
   int86(0x10,&r,&r);

}

int get_total_mem(void)
{
  union REGS r;
  unsigned int retval=0;

  _asm
	{
	  mov al,031h	   /* get EMS size  from CMOS	*/
	  out 070h,al	   /* becos himem will break it */
	  nop
	  nop
	  in  al,071h
	  xchg al,ah
	  mov al,030h
	  out 070h,al
	  nop
	  nop
	  in al,071h
	  mov retval,ax
	}
  return(retval / 1024);
}




void main(int argc, char **argv)
{
   int i,k,ki;
   unsigned long j=0;
   char far *vio;

   INIT();
   save_dos_screen();
   vio = VGA;
   mem_max = get_total_mem();
   //get_total_mem();
   if( i = Read_Data_From_HiMemory(j,1920,data))
       printf("mem error %d",i);
   for(i=0;i<1920;i++)
     {
       *vio++ = data[i];
       *vio++ = 0x1f;
     }
   memset(data,0,80);
   sprintf(data,"meg %d  Segment = %x  Offset = %x               total %d Meg", which_meg,(unsigned int)(j/16),(unsigned int)(j%16),mem_max+1);
   for(k=0,i=1920;i<2000;i++)
     {
       *vio++ = data[k++];
       *vio++ = 0x3f;
     }

   while((ki = getch()) != 0x1b)
      {
	 vio = VGA;
	 switch(ki)
	   {
	      case 0x2b :
			   if(which_meg < mem_max)
			      which_meg++;
			   j = 0;
			   break;
	      case 0x2d :
			   j = 0;
			   if(which_meg)
			       which_meg--;
			   break;
	      case 0:
		       ki=getch();
		       switch(ki)
			  {
			    case 0x51 :
					j+=1920;
					break;
			    case 0x49 :
					if(j>=1920)
					   j-=1920;
					else
					if(which_meg)
					  {
					     which_meg--;
					     j = 0xfffff;
					     j -= 1920;
					  }
					break;
			    case 0x48 :
					if(j>=80)
					   j-=80;
					else
					if(which_meg)
					  {
					     which_meg--;
					     j = 0xfffff;
					     //j -= 80;
					  }
					break;
			    case 0x50 :
					j+=80;
					break;
			  }
			break;
	   }
	 if( i = Read_Data_From_HiMemory(j,1920,data))
	     printf("mem error %d",i);
	 for(i=0;i<1920;i++)
	   {
	     *vio++ = data[i];
	     *vio++ = 0x1f;
	   }
	 memset(data,0,80);
	 sprintf(data,"meg %d  Segment = %x  Offset = %x               total %d Meg", which_meg,(unsigned int)(j/16),(unsigned int)(j%16),mem_max+1);
	 for(k=0,i=1920;i<2000;i++)
	   {
	     *vio++ = data[k++];
	     *vio++ = 0x3f;
	   }
	if(j>0xfffff)
	  {
	    if(which_meg < mem_max)
	       which_meg++;
	    j = 0;
	  }
      }
 restore_dos_screen();

}
