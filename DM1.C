
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

#define       diff_x	(x2-x1+1)*2
#define       diff_y	y2-y1+1

typedef struct
{
   unsigned char screen[4000];
   int cur_x;
   int cur_y;
} save_struct;

save_struct Dos_Screen;
save_struct Tmp_Screen;

unsigned char far *VGA;

unsigned char MemoryData[2000];
unsigned char TempData[2000];
int which_meg = 0;
int save_meg = 1;
int mem_max = 16;
int Cursor_Col=0;
int Cursor_Row=0;
unsigned long MemoryPos=0;
int mode=0;
char StatusData[80];

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



/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void INIT(void)
{
  union REGS regs;
     regs.h.ah = 0x0f;
     int86(0x10,&regs,&regs);
     VGA = regs.h.al == 7 ? MK_FP(0xb000,0x0000)  : MK_FP(0xb800,0x0000);
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void POP_UP( int  x1, int  y1, int  x2, int  y2, int  bkcolor, int  fgcolor,
	int  solid, char *string)
{
	/*   this sets up a double white border on any color
	     background,
	     if solid = 1   it sets the whole background color
	     else the background stays the same
        */
int x,y;
char attr,bk_attr = 0;
char far *ptr;
char shadow = 0x08;

   ptr = VGA;
   ptr+=((y1*160)+((x1-1)*2));
   attr=(unsigned char)(bkcolor  << 4| fgcolor);
   bk_attr = 0x08;
   *(ptr++) = 0xc9;
   *(ptr++) = attr;
   for(x=0;x<(diff_x-4)/2;x++)
     {
	*(ptr++) = 0xcd;
	*(ptr++) = attr;
     }
   *(ptr++) = 0xbb;
   *(ptr++) = attr;
   ptr+=160-diff_x;
   if (solid == 1)
     {
	for(x=0;x<diff_y-2;x++, ptr+=160-diff_x)
	  {
	     for(y=0;y<(diff_x)/2;y++)
	       {
		 if (y==0 | y==((diff_x/2)-1))
		    {
		       *(ptr++) = 0xba;
		       *(ptr+2)=*(ptr+2) & shadow;
		       *(ptr+4)=*(ptr+4) & shadow;
		     }
		 else
		     *(ptr++) = *(string++);
		 *(ptr++) = attr;
	       }
	  }
     }
   else
     {
	for(x=0;x<diff_y-2;x++,ptr+=160-diff_x)
	  {
	     for(y=0;y<(diff_x)/2;y++)
	       {
		 if (y==0 | y==((diff_x/2)-1))
		    {
		       *(ptr++) = 0xba;
		       *ptr++=attr;
		    }
		 else
		 ptr+=2;
	       }
	  }
     }
   *(ptr++) = 0xc8;
   *(ptr++) = attr;

   for(x=0;x<(diff_x-4)/2;x++)
      {
	 *(ptr++) = 0xcd;
	 *(ptr++) = attr;
      }

   *(ptr++) = 0xbc;
   *(ptr++) = attr;
   *(ptr+1)=*(ptr+1) & bk_attr;
   *(ptr+3)=*(ptr+3) & bk_attr;
   ptr+=165-diff_x;
   for(x=0;x<(diff_x-2);ptr+=2,x+=2)
      {
	 *(ptr)=*(ptr) & shadow;
	 *(ptr+2)=*(ptr+2) & shadow;
      }
      return(0);
   }


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int Read_Data_From_HiMemory(ULONG offset,int size,char far *pData)
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
	memcpy(pData,GETDATA_Temp_Buf,size);
     return 0;

}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Save_Screen( save_struct *Screen_Data)
{
int i;
char far *vio;
union REGS r;

   vio = VGA;
   for(i=0;i<4000;i++)
      Screen_Data->screen[i] = *vio++;
   r.h.ah = 3;
   r.h.bh = 0;
   int86(0x10,&r,&r);
   Screen_Data->cur_x  = r.h.dh;
   Screen_Data->cur_y  = r.h.dl;

}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Restore_Screen ( save_struct *Screen_Data)
{
char far *x;
int i;
union REGS r;

   x=VGA;
   for(i=0;i<4000;i++)
     *(x++)=Screen_Data->screen[i];
   r.h.ah = 2;
   r.h.bh = 0;
   r.h.dh = Screen_Data->cur_x;
   r.h.dl = Screen_Data->cur_y;
   int86(0x10,&r,&r);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int GetCursorCol(void)
{
union REGS r;

   r.h.ah = 3;
   r.h.bh = 0;
   int86(0x10,&r,&r);
   return (r.h.dl);
}
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int GetCursorRow(void)
{
union REGS r;

   r.h.ah = 3;
   r.h.bh = 0;
   int86(0x10,&r,&r);
   return (r.h.dh);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Set_Cursor_Pos ( int row, int col)
{
union REGS r;

   r.h.ah = 2;
   r.h.bh = 0;
   r.h.dh = (unsigned char)row;
   r.h.dl = (unsigned char)col;
   int86(0x10,&r,&r);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
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

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
char far * convert(unsigned char x,unsigned int base)
{
  char tmp[9];
  char z;

  switch(base)
    {
     case 0x02	: z=x /16;
		  switch(z)
		    {
		      case 0 : strcpy(tmp,"0000"); break;
		      case 1 : strcpy(tmp,"0001"); break;
		      case 2 : strcpy(tmp,"0010"); break;
		      case 3 : strcpy(tmp,"0011"); break;
		      case 4 : strcpy(tmp,"0100"); break;
		      case 5 : strcpy(tmp,"0101"); break;
		      case 6 : strcpy(tmp,"0110"); break;
		      case 7 : strcpy(tmp,"0111"); break;
		      case 8 : strcpy(tmp,"1000"); break;
		      case 9 : strcpy(tmp,"1001"); break;
		      case 10 : strcpy(tmp,"1010"); break;
		      case 11 : strcpy(tmp,"1011"); break;
		      case 12 : strcpy(tmp,"1100"); break;
		      case 13 : strcpy(tmp,"1101"); break;
		      case 14 : strcpy(tmp,"1110"); break;
		      case 15 : strcpy(tmp,"1111"); break;
		    }
		  z=(char)((unsigned int)x %16);
		  switch(z)
		    {
		      case 0 :	strcat(tmp,"0000"); break;
		      case 1 :	strcat(tmp,"0001"); break;
		      case 2 :	strcat(tmp,"0010"); break;
		      case 3 :	strcat(tmp,"0011"); break;
		      case 4 :	strcat(tmp,"0100"); break;
		      case 5 :	strcat(tmp,"0101"); break;
		      case 6 :	strcat(tmp,"0110"); break;
		      case 7 :	strcat(tmp,"0111"); break;
		      case 8 :	strcat(tmp,"1000"); break;
		      case 9 :	strcat(tmp,"1001"); break;
		      case 10 : strcat(tmp,"1010"); break;
		      case 11 : strcat(tmp,"1011"); break;
		      case 12 : strcat(tmp,"1100"); break;
		      case 13 : strcat(tmp,"1101"); break;
		      case 14 : strcat(tmp,"1110"); break;
		      case 15 : strcat(tmp,"1111"); break;
		    }
		  break;

     case 16  :   tmp[0] = (z=(unsigned int)x /16) < 10 ? (char)(z+0x30) : (char)((z-10)+0x61);
		  tmp[1] = (z=(unsigned int)x %16) < 10 ? (char)(z+0x30) : (char)((z-10)+0x61);
		  break;
    }
return (char far *)tmp;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void convert_hex(char far *pdata,char far *dest)
{
    register i,j,k=0;
    unsigned char *x;

    for(i=0,k=0;i<24*40;i++)
    {
      x=convert(*(pdata+i),16);
      *(dest+k++) = *(x++);
      *(dest+k++) = 0x1f;
      *(dest+k++) = *(x);
      *(dest+k++) = 0x1f;
    }
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void ReverseThisString(char *pdata)
{
  int i,k;
  char far *vio;

     for(i=0;i<1920;i++)
       if(strncmp(&MemoryData[i],pdata,strlen(pdata)) == 0)
       {
	  vio = VGA;
	  for(k=0;k<strlen(pdata);k++)
	     *(vio+(i*2)+1+k) = 0x70;
       }
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
char *GetStringAtReverse(char *pdata)
{
   int i,k;
   char far *vio;

      vio = VGA;
      for(k=0,i=0;i<1920;i++)
      {
	 if(*(vio+(i*2)+1)  == 0x70)
	 {
	    pdata[k++] = *(vio+(i*2));

	 }
      }
    pdata[k] = 0;
    return pdata;
}
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void  ResetScreenAttributes(void)
{
   int i;
   char far *vio;

      vio = VGA;
      for(i=0;i<1920;*(vio+(i*2)+1) = 0x1f,i++);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int Search_Backward (char *text)
{
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
int Search_Foreward (char *text)
{
   int i;
   unsigned char first;
   int ok=0;
   int where;

   first = text[0];
   for(;;)
   {
       if( Read_Data_From_HiMemory(MemoryPos,1920,TempData))
	 break;
       for(i=0;i<1920;i++)
       {
	   if(TempData[i] = first)
	      if (strncmp(&TempData[i],text,strlen(text)) == 0)
		putch(7);
		  //HighlightIT();
       }
   }

      /*
      _asm
      {
	  mov	al,first
	  les	di,TempData
	  cld
	  mov	cx,1920
	  repne scasb
	  jne	end

	  mov	ok,1
	  dec	di
	  mov	where,cx
	end:
      }
     */
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void main(int argc, char **argv)
{
   int i,k,ki;
   int marked = 0;
   int marked_col = 0;
   int marked_row = 0;
   char far *vio;
   int size ;
   int increment ;
   char searchdata[80];
   int Searching=0;
   int found=0;

   INIT();
   Save_Screen(&Dos_Screen);
   vio = VGA;
   mem_max = get_total_mem();

   if( i = Read_Data_From_HiMemory(MemoryPos,1920,MemoryData))
       printf("mem error %d",i);
   for(i=0;i<1920;i++)
     {
       *vio++ = MemoryData[i];
       *vio++ = 0x1f;
     }
   memset(StatusData,0,80);
   sprintf(StatusData,"meg %d  Segment = %x  Offset = %x               total %d Meg", which_meg,(unsigned int)(MemoryPos/16),(unsigned int)(MemoryPos%16),mem_max+1);
   for(k=0,i=1920;i<2000;i++)
     {
       *vio++ = StatusData[k++];
       *vio++ = 0x3f;
     }

   Set_Cursor_Pos (0,0);

   increment=80;

   for(;;)
      {
	 ki = getch();
	 if(ki == 0x1b)
	 {
	    if(!marked)
	       break;
	    else
	    {
	       ResetScreenAttributes();
	       marked = 0;
	    }
	 }
	 if(mode==0)
	    size = 1920;
	 else
	    size = 1920/2;

	 vio = VGA;
	 switch(ki)
	   {

	      case 0x2b :
			   if(which_meg < mem_max)
			      which_meg++;
			   MemoryPos = 0;
			   break;
	      case 0x2d :
			   MemoryPos = 0;
			   if(which_meg)
			       which_meg--;
			   break;
	      case 0:
		       ki=getch();
		       switch(ki)
			  {
			    case 0x3b : mode = 0; increment = 80; break;
			    case 0x3c : mode = 1; increment = 40; break;
			    case 0x3d :
					 Searching = 1;
					 Save_Screen(&Tmp_Screen);
					 if(marked)
					 {
					    GetStringAtReverse(searchdata);
					    POP_UP(10,10,10+strlen(searchdata),12,14,15,1,searchdata);
					 }
					 //  Search_Foreward (GetStringAtReverse(searchdata));
					 //Restore_Screen (&Tmp_Screen);
					 break;
			    case 0x3e :
                                         //if(marked)
                                           //Search_Backward ();
					 break;
			    case 0x1e :    /* ALT a */
					marked = 1;
					marked_col = GetCursorCol();
					marked_row = GetCursorRow();
					break;
			    case 0x51 :
					MemoryPos+=size;
					break;
			    case 0x49 :
					if(MemoryPos>=size)
					   MemoryPos-=size;
					else
					if(which_meg)
					  {
					     which_meg--;
					     MemoryPos = 0xfffff;
					     MemoryPos -= size;
					  }
					break;

			    case 0x4b :
					Cursor_Col--;
					if(Cursor_Col < 0)
					  Cursor_Col=0;
					break;
			    case 0x4d :
					Cursor_Col++;
					if(Cursor_Col > 79)
					  Cursor_Col=79;
					break;
			    case 0x48 :
					Cursor_Row--;
					if(Cursor_Row < 0)
					{
					   Cursor_Row=0;
					   if(MemoryPos>=increment)
					      MemoryPos-=increment;
					   else
					   if(which_meg)
					     {
						which_meg--;
						MemoryPos = 0xfffff;
						//MemoryPos -= increment;
					     }
					}
					break;
			    case 0x50 :
					Cursor_Row++;
					if(Cursor_Row >23)
					{
					   Cursor_Row=23;
					   MemoryPos+=increment;
					}
					break;
			  }
			break;
	   }
	 Set_Cursor_Pos (Cursor_Row,Cursor_Col);
	 if( i = Read_Data_From_HiMemory(MemoryPos,size,MemoryData))
	     printf("mem error %d",i);

	 if(Searching)
	 {
	    for(;;)
	    {
	       for(i=Cursor_Row*80+Cursor_Col;i<1920;i++)
		 if(strncmp(&MemoryData[i],searchdata,strlen(searchdata)) == 0)
		 {
		    putch(7);
		    found = 1;
		    Restore_Screen (&Tmp_Screen);
		    Searching = 0;
		    ResetScreenAttributes();
		    ReverseThisString(searchdata);
		    marked = 0;
		    break;
		  }
	       if(found) break;
	       MemoryPos+=size;
	       if(MemoryPos>0xfffff)
		 {
		   if(which_meg < mem_max)
		      which_meg++;
		   MemoryPos = 0;
		 }

	       if( i = Read_Data_From_HiMemory(MemoryPos,size,MemoryData))
		  break;
	       memset(StatusData,0,80);
	       sprintf(StatusData,"meg %d  Segment = %x  Offset = %x               total %d Meg", which_meg,(unsigned int)(MemoryPos/16),(unsigned int)(MemoryPos%16),mem_max+1);
	       for(k=0,i=1920;i<2000;i++)
		 {
		   *(vio+(i*2)) = StatusData[k++];
		   *(vio+(i*2+1)) = 0x3f;
		 }
	    }
	 }

	 if(mode == 1)
	 {
	     convert_hex(MemoryData,vio);
	 }
	 else
	 {
	    if(!Searching)
	       for(i=0,k=0;i<1920;i++)
		 {
		   *(vio+k++) = MemoryData[i];
		   k++;
		 }
	 }

	 if(marked)
	 {
	    if(marked_col > Cursor_Col)
	    {
		k = marked_row*160 + Cursor_Col*2;
		for(;k<marked_row*160 + marked_col*2;k++)
		{
		  *(vio+(k++)+1) = 0x70;
		}
	    }
	    else
	    {
		k = marked_row*160 + marked_col*2;
		for(;k<marked_row*160 + Cursor_Col*2;k++)
		{
		  *(vio+(k++)+1) = 0x70;
		}

	    }
	 }

	 memset(StatusData,0,80);
	 sprintf(StatusData,"meg %d  Segment = %x  Offset = %x               total %d Meg", which_meg,(unsigned int)(MemoryPos/16),(unsigned int)(MemoryPos%16),mem_max+1);
	 for(k=0,i=1920;i<2000;i++)
	   {
	     *(vio+(i*2)) = StatusData[k++];
	     *(vio+(i*2+1)) = 0x3f;
	   }
	if(MemoryPos>0xfffff)
	  {
	    if(which_meg < mem_max)
	       which_meg++;
	    MemoryPos = 0;
	  }
      }

 Restore_Screen (&Dos_Screen);
}
