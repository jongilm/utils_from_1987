
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <stdarg.h>

void _cdecl Display_line (unsigned int z, char *fmt, ... );
unsigned int DeAlloc_Mem(void);

#define PGUP	1
#define UP	2
#define PGDOWN	3
#define DOWN	4

typedef unsigned long ULONG;
typedef void far *FP;
#ifndef MK_FP
#define MK_FP(seg,ofs)  ((FP)(((ULONG)(seg) << 16) | (ofs)))
#endif

FILE *file1;

typedef struct ALIST LIST;
struct ALIST
{
   char far *next;
   char far *prev;
   char far *offset;
   unsigned long File_Offset;
};

LIST far *currentchain;
LIST far FirstChain;

char far *Startreadchain;
char far *StartChain;
char far *prev;
long count=0L;


/******************* variables **********************/
union REGS regs;
char *Vio,tmp_screen[4000],buffer1[72][80];
unsigned int num_items_read,num_to_read;
unsigned long readpos;
unsigned int mode=0;	/* 0-norm 1=hex 2=text 3=bin 4=hex/text 5=bin/hex 6=bin/text */
char line[80];
unsigned int start=0;
unsigned int end1=0;
long bufpos,tmppos,uppos,downpos;
long row,startrow,rrow;
unsigned int col,startcol,ccol;
unsigned int update=1;
unsigned long pos=0;
long Mem_Avail;
long File_Size;
unsigned char huge *Malloc_Buffer=0;
unsigned int Mallocked_Seg=0;
unsigned int ALL_DONE;
unsigned int update_this = 0;
unsigned int num_lines = 0;
unsigned int scroll_lines=0;
int direction=0;



/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void cursor(unsigned int row,unsigned int col)
{
  regs.h.ah = 2;
  regs.h.bh = 0;
  regs.h.dh = (char)row;
  regs.h.dl = (char)col;
  int86(0x10,&regs,&regs);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Init(void)
 {

     regs.h.ah = 3;
     regs.h.bh = 0;
     int86(0x10,&regs,&regs);
     startrow = regs.h.dh;
     startcol = regs.h.dl;

     regs.h.ah = 0x0f;
     int86(0x10,&regs,&regs);
     Vio = regs.h.al == 7 ? MK_FP(0xb000,0x0000)  : MK_FP(0xb800,0x0000);
     regs.h.ah = 0x01;
     regs.h.ch = 0x00;
     regs.h.cl = 12;
     int86(0x10,&regs,&regs);
     cursor(0,0);
 }

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void SAV_SCR(char far *ptr)
{
   char far *x;
   unsigned int i;

   x=Vio;
   for(i=0;i<4000;i++)
   *(ptr++)=*(x++);
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void PUT_SCR(char far *ptr)
{
   char far *x;
   unsigned int i;

   x=Vio;
   for(i=0;i<4000;i++)
   *(x++) =*(ptr++);
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void _cdecl Terminate (char *fmt, ... )
{
 va_list marker;

 va_start ( marker, fmt );
 sprintf (fmt,marker);
 va_end   ( marker );

 if(Mallocked_Seg)
   {
    row = DeAlloc_Mem();
    if(row == 7)
       printf("Memory control blocks destroyed");
    if(row == 9)
       printf("Invalid deallocate address");
   }
 else
    free(Malloc_Buffer);
 cursor((unsigned int)startrow,startcol);
 fclose(file1);
 PUT_SCR(tmp_screen);

}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Init_File_Position(char far *position)
{
     currentchain = &FirstChain;
     currentchain->prev    = NULL;
     currentchain->offset  = position;
     currentchain->File_Offset = 0;

}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void Save_File_Position(char far *position)
{
char far *prev;

     prev = (char far *)currentchain;
     currentchain->next = (char far *) malloc (sizeof(LIST));
     if(currentchain->next == NULL)
	Terminate ("Error allocating memory %u ",sizeof(LIST));
     currentchain	   = (LIST far *)currentchain->next;
     currentchain->prev    = prev;
     currentchain->offset  = position;
     currentchain->File_Offset = pos;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
char far * Get_File_Position(void)
{
  char far * retval;

     retval = currentchain->offset;
     currentchain = (LIST far *)currentchain->prev;
     free(&currentchain->next);
     pos = currentchain->File_Offset;
     return(retval);
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
long Get_Avail_Mem(void)
{
   unsigned int retval;

   _asm mov ah,048h
   _asm mov bx,0ffffh
   _asm int 021h
   _asm mov retval,bx
   return((long)((long)retval*16L));
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
unsigned int DeAlloc_Mem(void)
{
   unsigned int retval=0;

   _asm
    {
	mov ah,049h
	mov ES,Mallocked_Seg
	int 021h
	jnc endit
	mov retval,ax
	endit:
    }
   return(retval);
}
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
unsigned int Alloc_Mem(long mem)
{
   unsigned int retval=0;
   unsigned int para=0;

   para = (unsigned int)(mem / 16L)+2;

   _asm
    {
	mov ah,048h
	mov bx,para
	int 021h
	jc  error
	mov Mallocked_Seg,ax
	jmp end
      error:
	mov retval,1
      end:
    }
   if(retval)
      return(retval);
   Malloc_Buffer = MK_FP (Mallocked_Seg,0);
   return(retval);
}



/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
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
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void convert_text(char far *data,char far *dest,unsigned int lines)
{
    register i=0,j=0,k=0,num;
    memset(dest,' ',lines*80);
    num_lines = 72;
    while(i<lines)
	{
	  if(pos+k>=File_Size)
	     {
		num_lines = i;
		break;
	     }
	  if(*(data+k) == '\t')
	     {
	       j += 8-(j%8);
	       k++;
	       continue;
	     }
	  if((*(data+k) == '\r')  || (*(data+k) == '\n'))
	    {
	      k++; i++; j=0;
	      if((*(data+k) == '\r')  || (*(data+k) == '\n'));
	      k++;
	    }
	  else
	   {
	    *(dest+(i*80)+j++) = *(data+k++);
	    if(j>79)
	      {
	       j=0; i++;
	      }
	    if(i==47)
	      tmppos = k;
	   }
	}
    switch(direction)
	 {
	   case PGUP  : pos -= k; break;
	   case UP    : pos -= k; break;
	   case PGDOWN: pos += k; break;
	   case DOWN  : pos += k; break;
//	     otherwise	: pos += k; break;
	 }
//    pos += k;
    bufpos = k;
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void convert_bin(char far *data,char far *dest,unsigned int lines)
{
    register i,j,k=0,l;
    unsigned char *x;

    for(i=0;i<lines;i++)
      for(j=0;j<80;)
	{
	  if(pos+k>=File_Size)
	     break;
	  x=convert(*(data+k++),2);
	  for(l=0;l<8;l++)
	      *(dest+(i*80)+j++) = *(x++);
	  if(j<80)
	      *(dest+(i*80)+j++) = ' ';
	}
    pos += k;
    tmppos = 423;
    bufpos = k;

}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void convert_hex(char far *data,char far *dest,unsigned int lines)
{
    register i,j,k=0;
    unsigned char *x;

    for(i=0;i<lines;i++)
      for(j=0;j<80;j++)
	{
	  if(pos+k>=File_Size)
	     break;
	  x=convert(*(data+k++),16);
	  *(dest+(i*80)+j++) = *(x++);
	  *(dest+(i*80)+j++) = *(x);
	  if(j<79)
	     *(dest+(i*80)+j) = ' ';
	}
    pos += k;
    tmppos = 47*80;
    bufpos = k;
}


/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void convert_hex_text(char far *data,char far *dest,unsigned int lines)
{
    register i,j,k=0,l=0;
    char *x;

    for(i=0;i<lines;i++)
     {
      if(pos+k>=File_Size)
	 break;
      for(j=0;j<60;j)
	{
	  x=convert(*(data+k++),16);
	  *(dest+(i*80)+j++) = *(x++);
	  *(dest+(i*80)+j++) = *(x++);
	  if(j<60)
	    *(dest+(i*80)+j++) = ' ';
	}
      for(j=0;j<20;j++)
	 *(dest+(i*80)+60+j) = *(data+l++);
     }
    pos += k;
    tmppos = 47*60;
    bufpos = k;
}

/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
//void Display_line(char *x,unsigned int z)
void _cdecl Display_line (unsigned int z, char *fmt, ... )
{
 char str[80]={0};
 va_list marker;
 register i,j;
 char *y=Vio;

 va_start ( marker, fmt );
 vsprintf (str,fmt,marker);
 va_end   ( marker );

 for(i=0,j=3840,y=Vio;i<80;i++)
   {
    if(str[i] != 0)
       *(y+j++) = str[i];
    else
       *(y+j++) = 0x20;
    *(y+j++) = 0x70;
   }
if(z)
  getch();
}



/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void display(unsigned int mode,char *buf,unsigned int offset,unsigned int len) /* 0-norm 1=hex 2=text 3=bin 4=hex/text 5=bin/hex 6=bin/text */
{
 register i,j,k,l;
 char *y=Vio;
 char far *ptr;

 if(update)
   {
      switch(mode)
	{
	 case 0: memcpy(&buffer1[offset][0],buf,len*80);
		 break;

	 case 1: convert_hex(buf,&buffer1[offset][0],len);
		 break;

	 case 2:// memset(y,0,4000);
		 convert_text(buf,&buffer1[offset][0],len);
		 break;

	 case 3: convert_bin(buf,&buffer1[offset][0],len);
		 break;

	 case 4: convert_hex_text(buf,&buffer1[offset][0],len);
		 break;

	 /*
	 case 5:

	 case 6:

	 case 7:  */
	}
    update =0;
    return;
    /*buf = &buffer1[0][0];*/
   }
 Display_line(0,"scroll_lines  %d  num_lines  %d row %ld   rrow %ld  dir = %d",scroll_lines,num_lines,row,rrow,direction);
// sprintf(line,"F1-Normal F2-Hex F3=text F4=Binary F5=Hex/Text F6=Binary/Hex row %d col %d  ",rrow,col);
 ptr = &buffer1[offset][0];

 for(i=0;i<24;i++)
   for(j=0;j<80;j++)
     {
      *(y++) = *(ptr++);
      *(y++) = 0x3f;
     }
 //Display_line(0,line);
}




/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³							    ³*/
/*³							    ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
void main(int argc, char **argv)
{

char key;
long posn=0L;
unsigned int text_mode = 0;
unsigned long temp;
unsigned int first = 1;

 if(argc < 2)
   {
    puts("USAGE : DF <File Name> /h /b /t\n");
    puts("   /h  Hex    Dump");
    puts("   /b  Binary Dump");
    puts("   /t  Text   Dump");
    puts("   default ASCII - no translation");
    exit(0);
   }
 else
   {
    if ((file1=fopen(argv[1],"rb"))==NULL)
      {
       printf("File not found : %s \n",argv[1]);
       exit(0);
      }
   }
 Init();

 if(argc > 1)
   {
      if(toupper(argv[2][1])=='H') mode=1;
      if(toupper(argv[2][1])=='T') mode=2;
      if(toupper(argv[2][1])=='B') mode=3;
   }

 SAV_SCR(tmp_screen);


 fseek (file1,0,SEEK_END);
 File_Size = ftell (file1);
 fseek (file1,0,SEEK_SET);
 Mem_Avail = Get_Avail_Mem();
 if(File_Size < Mem_Avail - 100)
   {
      if(Alloc_Mem(File_Size+100))
	   printf("error alloc mem %ld bytes",File_Size);
      if(File_Size > 64000)
	 {
	    for(;;)
	      {
		memset(Malloc_Buffer+num_items_read,' ',10000);
		if(!(num_items_read = fread(Malloc_Buffer+num_items_read,1,10000,file1)));
		   break;
	      }
	 }
      else
	 {
	    memset(Malloc_Buffer,' ',(unsigned int)File_Size);
	    num_items_read = (fread(Malloc_Buffer,(unsigned int)File_Size,1,file1));
	    ALL_DONE = 1;
	    strcpy(Malloc_Buffer+File_Size,"<EOF>");
	 }
   }
 else
   {
     if((Malloc_Buffer = malloc (6000)) == 0)
	{
	  printf("error alloc mem %ld bytes",6000);
	  cursor(24,0);
	  exit(0);
	}

     memset(Malloc_Buffer,' ',6000);
     fseek(file1,0L,0);
     num_items_read = (fread(Malloc_Buffer,1,6000,file1));   /* 3 pages */
     if(num_items_read < 5990)
	strcat(Malloc_Buffer+num_items_read,"<EOF>");
     ALL_DONE = 0;
   }
 rrow=0;
 ccol=0;

 Init_File_Position(Malloc_Buffer);
 update =1;
 display(mode,(char *)Malloc_Buffer,0,72);
 update =0;
 display(mode,(char *)Malloc_Buffer,0,72);
 pos += bufpos;

 //Save_File_Position(Malloc_Buffer);

 for(;;)
   {
     key=(char)getch();
     if (key==0x1b) break;
     direction = 0;
     switch(key)
      {
       case 0x00:
	     switch(key=getch())
	       {
		case 0x3b : mode = 0; update =1; break;
		case 0x3c : mode = 1; update =1; break;
		case 0x3d : if(text_mode)
				Display_line(1,"Invalid text format : Any key to continue");
			    else
			       {
				  mode = 2;
				  update =1;
			       }
			    break;
		case 0x3e : mode = 3; update =1; break;
		case 0x3f : mode = 4; update =1; break;
		case 0x48 : direction = UP    ; break;
		case 0x50 : direction = DOWN  ; break;
		case 0x51 : direction = PGDOWN; break;
		case 0x49 : direction = PGUP  ; break;
		case 0x4b : if(col>0)  col--; break;  /*  left arrow  */
		case 0x4d : if(col<79) col++; break;  /*  right arrow	*/
	      }
	      break;
      }

       switch(direction)
	 {
	   case PGUP  : if(rrow < 24) break;
			   if(scroll_lines > 24)
			      {
				 scroll_lines-=24;
				 rrow -= 24;
				 row = 0;
			      }
			   else
			      {
				 Malloc_Buffer =  Get_File_Position();
				 Malloc_Buffer-=bufpos;
			      }
			 break;
	   case UP    : if(rrow < 1) break;
			if(row > 0)
			   {
			      rrow--;
			      row--;
			      break;
			   }
			if(scroll_lines)
			   {
			      scroll_lines--;
			      rrow--;
			   }
			else
			   {
			      rrow--;
			      scroll_lines  = 47;
			      Get_File_Position();
			      temp = pos;
			      memcpy (&buffer1[48][0],&buffer1[0][0],24*80);
			      update = 1;
			      display(mode,(char *)(Malloc_Buffer+pos),0,48);
			      update = 0;
			      pos = temp;
			      //Malloc_Buffer -= bufpos;
			   }
			break;
	   case PGDOWN: if(scroll_lines+24 > num_lines)
			  {
			      rrow+=(long)((unsigned long)num_lines-(unsigned long)scroll_lines);
			      scroll_lines+=num_lines-scroll_lines;
			      break;
			  }
			else
			  {
			      rrow+=24;
			      scroll_lines+=24+row;
			      row=0;
			  }
			if(scroll_lines >= 48)
			   {
			      scroll_lines = 0;
			      memcpy (&buffer1[0][0],&buffer1[48][0],24*80);
			      //Malloc_Buffer+=bufpos;
			      update = 1;
			      display(mode,(char *)(Malloc_Buffer+pos),24,48);
			      update = 0;
			      Save_File_Position(Malloc_Buffer);
			   }
			break;

	   case DOWN  : rrow++;
			if(row<23)
			   row++;
			else
			   scroll_lines++;
			if(scroll_lines > 48)
			   {
			      scroll_lines = 1;
			      memcpy (&buffer1[0][0],&buffer1[48][0],24*80);
			      //Malloc_Buffer+=bufpos;
			      update = 1;
			      display(mode,(char *)(Malloc_Buffer+pos),24,48);
			      update = 0;
			      Save_File_Position(Malloc_Buffer);
			   }
			if(scroll_lines > num_lines)
			   {
			      scroll_lines--;
			      rrow--;
			   }
			break;
	 }

       if(update)
	 {
	    display(mode,(char *)(Malloc_Buffer),scroll_lines,72);
	    display(mode,(char *)(Malloc_Buffer),scroll_lines,72);
	  }
       else
	  display(mode,(char *)(Malloc_Buffer),scroll_lines,0);
       cursor((unsigned int)row,col);
   }

  Terminate ("");
}
