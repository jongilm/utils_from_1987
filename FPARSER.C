#include <float.h>
#include <math.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>

char input[79];
char opstack[100];
double numstack[100];
double vars[26];

int priority[16]  = {1,1,2,2,3,4,4,4,4,4,4,4,4,4,4,4};
int dotable[4][4] = {1,2,3,2,5,5,4,5,2,6,3,2,2,2,3,7};

double *numptr;
char   *opsptr;
char   *posi;
char   *inptr;
double result;
int    error;
int    pflag;

char   *parse(char *inp);
void   exptanum(void);
void   err(int errno);
int    pos(char *str1, char *str2);
void   popit(void);
double convnum(void);
int    searchunary(void);
int    searchbinary(void);
int    searchvar(void);
void   push(void);

FILE *infile;

void main(int argc, char *argv[])
{
   if (argc <= 1) return;
   if ( (infile = fopen(argv[1],"rt")) == NULL) return;

   do
   {
      if (fgets(input,79,infile) == NULL) break;
      for (posi = input; *posi != '\0'; ++posi)
         if (!isprint(*posi))
         {
            *posi = '\0';
            break;
         }
      printf("%s\n",input);

      for (posi = input; *posi != '\0'; ++posi)
         if (islower(*posi)) *posi -= 0x20;

      if (strcmp(input,"QUIT") != 0)
      {
         error = 0;
         pflag = 0;
         posi  = input;

         while (*posi != '\0' && error == 0) posi = parse(posi);

         if (error == 0 && pflag == 0) printf("Result = %g\n",result);
         else                          printf("\n");
      }
   } while (strcmp(input,"QUIT") != 0);
   fclose(infile);
}

char *parse(char *inp)
{
   int i,j;
   opstack[0] = '\xFF';
   numptr     = numstack;
   opsptr     = opstack;
   inptr      = inp;

   if (isupper(inp[0]) && inp[1] == '=') inptr+=2;
   if (strncmp(inp, "GOTO "  , 5) == 0)  inptr+=5;
   if (strncmp(inp, "PRINT " , 6) == 0)  inptr+=6;
   if (strncmp(inp, "PRINTN ", 7) == 0)  inptr+=7;

   exptanum();

   while (!error)
   {
      while (*inptr == ' ') ++inptr;

      if      (*inptr == ':' || *inptr == '\0') i = 0;
      else if (*inptr == ')')                   i = 1;
      else if (*inptr == ',')                   i = 3;
      else if (pos("+-*/\^",inptr))             i = 2;
      else {err(3); break;}

      if      (*opsptr == '\xFF')               j = 0;
      else if (*opsptr == '(')                  j = 2;
      else if (*opsptr == '[')                  j = 3;
      else if (pos("+-*/\^!@#$%'&?.;:",opsptr)) j = 1;

      switch (dotable[j][i])
      {
         case 1: result = numstack[0];
                 if (inp[1] == '=') vars[inp[0]-65] = result;
                 if (strncmp(inp, "GOTO "  , 5) == 0)
                 {
                    inptr = input;
                    i     = (int)result - 1;    /**** I cast this ****/
                    while (i > 0 && *inptr != '\0')
                    {
                       while (*inptr != ':' && *inptr != '\0') ++inptr;
                       --i;
                       ++inptr;
                    }
                    if (*inptr == ':') ++inptr;
                    return(inptr);
                 }

                 if (strncmp(inp, "PRINT " , 6) == 0)
                 {
                    printf("%g ",result);
                    pflag = 1;
                 }

                 if (strncmp(inp, "PRINTN ", 7) == 0)
                 {
                    printf("\n%g ",result);
                    pflag = 1;
                 }
                 if (*inptr == ':') ++inptr;
                 return(inptr);
                 break;
         case 2: err(1);
                 break;
         case 3: push();
                 break;
         case 4: i = pos("+-*/\^!@#$%'&?.;:",inptr) - 1;
                 i = pos("+-*/\^!@#$%'&?.;:",opsptr) - 1;
                 if (priority[i] <= priority[j]) popit();
                 else                            push();
                 break;
         case 5: popit();
                 break;
         case 6: --opsptr;
                 ++inptr;
                 break;
         case 7: --opsptr;
                 ++inptr;
                 exptanum();
                 break;
      } /* switch */
   } /* while */
} /* function */

void   exptanum(void)
{
   double i;
   int    j;

   if (strncmp(inptr,"HEX(",4) == 0)
   {
      inptr += 4;
      i = 0;
      do
      {
         j = pos("0123456789ABCDEF",inptr);
         if (j==0) {err(1); break;}
         i = i*16 + j - 1;
         ++inptr;
      } while (*inptr != ')');
      ++inptr;
      *numptr = i;
      ++numptr;
   }
   else
   {
      if (*inptr == '(' || *inptr == '[') push();
      else
         if (searchunary() == 0)
            if (searchbinary() == 0)
               if (searchvar() == 0)
               {
                  *numptr = convnum();
                  ++numptr;
               }
   }
}

void   err(int errno)
{
   while (inptr > input)
   {
      printf(" ");
      --inptr;
   }
   switch (errno)
   {
      case 1: printf("^ Syntax error - missing bracket or operand\n"); break;
      case 2: printf("^ Error - divide by zero\n"); break;
      case 3: printf("^ Error - missing operator\n"); break;
      case 4: printf("^ Error - zero to a negative power\n"); break;
   }
   error = 1;
}

int    pos(char *str1, char *str2)
{
   int place;

   place = 0;
   while (str1[place] != '\0' && str1[place] != *str2) ++place;
   if (str1[place] == '\0') return(0);
   else                     return(place+1);
}

void   popit(void)
{
   double temp1, temp2;
   int    opnum, temp3;

   opnum = pos("+-*/\^!@#$%'&?.;:",opsptr);

   if ((numptr == numstack || numptr == &numstack[1]) && opnum < 6) err(1);
   else if (numptr == numstack) err(1);
   else
   {
      temp2 = numptr[-1];
      if (opnum < 6 || opnum > 15)
      {
         --numptr;
         temp1 = numptr[-1];
      }
      switch (opnum)
      {
         case  1: temp1 += temp2; break;
         case  2: temp1 -= temp2; break;
         case  3: temp1 *= temp2; break;
         case  4: if (temp2 == 0) err(2);
                  else temp1 /= temp2; break;
         case  5: if (temp1 == 0 && temp2 < 0) err(4);
                  else temp1 = pow(temp1,temp2); break;
         case  6: temp1 = sin(temp2); break;
         case  7: temp1 = cos(temp2); break;
         case  8: temp1 = tan(temp2); break;
         case  9: temp1 = asin(temp2); break;
         case 10: temp1 = acos(temp2); break;
         case 11: temp1 = atan(temp2); break;
         case 12: temp1 = log(temp2); break;
         case 13: temp3 = temp2; temp1 = temp3; break;
         case 14: temp1 = exp(temp2); break;
         case 15: temp1 = 1;
                  if (temp2 < 2) break;
                  for (temp3 = 2; temp3<=temp2; ++temp3) temp1 *= temp3;
                  break;
         case 16: temp1 = fmod(temp1,temp2); break;
      } /* switch */
      --opsptr;
      numptr[-1] = temp1;
   }
}

double convnum(void)
{
   char tempnum[30];
   char flag;
   int  y;

   y    = 0;
   flag = ' ';

   if (*inptr == '-')
   {
      tempnum[0] = '-';
      y = 1;
      ++inptr;
   }

   while (pos("0123456789.",inptr))
   {
      if (*inptr == '.')
      {
         if (flag != ' ') {err(1); break;}
         else             flag = 'X';
      }
      tempnum[y] = *inptr;
      ++y;
      ++inptr;
   }
   tempnum[y] = '\0';
   if (y==0) err(1);
   else      return(atof(tempnum));
}

int    searchunary(void)
{
   char token;

   token = ' ';
   if      (strncmp(inptr,"SIN(",4) == 0) token = '!';
   else if (strncmp(inptr,"COS(",4) == 0) token = '@';
   else if (strncmp(inptr,"TAN(",4) == 0) token = '#';
   else if (strncmp(inptr,"ASN(",4) == 0) token = '$';
   else if (strncmp(inptr,"ACS(",4) == 0) token = '%';
   else if (strncmp(inptr,"ATN(",4) == 0) token = '\'';
   else if (strncmp(inptr,"LOG(",4) == 0) token = '&';
   else if (strncmp(inptr,"INT(",4) == 0) token = '?';
   else if (strncmp(inptr,"EXP(",4) == 0) token = '.';
   else if (strncmp(inptr,"FAC(",4) == 0) token = ';';
   else return(0);
   inptr += 3;
   ++opsptr;
   *opsptr = token;
   exptanum();
   return(1);
}

int    searchbinary(void)
{
   if (strncmp(inptr,"MOD(",4) == 0)
   {
      inptr += 4;
      ++opsptr;
      *opsptr = ':';
      ++opsptr;
      *opsptr = '(';
      ++opsptr;
      *opsptr = '[';
      exptanum();
      return(1);
   }
   else
      return(0);
}

int    searchvar(void)
{
   if (isupper((int)*inptr))
   {
      *numptr = vars[*inptr - 65];
      ++numptr;
      ++inptr;
      return(1);
   }
   else return(0);
}

void   push(void)
{
   ++opsptr;
   *opsptr = * inptr;
   ++inptr;
   exptanum();
}

