/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <math.h>
#include <graph.h>

#include <tools.h>

double balance[500];
double interest[500];
char line[21],last_char;
double amount;
double rate;
double mth_rate;
double years;
double months;

unsigned int screen,screentype;

void main(void);
void mortgage_comparisons(void);
void mortgage_amortization(void);
void get_params(void);

void main()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
{
   int ch;
   do
   {
      ch = 0;
      clrscr(7);
      gotoxy( 1, 1); printf("�� Mortgage Analysis 敲");
      gotoxy( 1, 2); printf("훤袴袴袴袴袴袴袴袴袴袴�");
      gotoxy( 1, 4); printf("1   - Mortgage comparisons");
      gotoxy( 1, 5); printf("2   - Mortgage amortization");
      gotoxy( 1, 6); printf("Esc - Exit");
      gotoxy( 1, 8); printf("Option =====> ");
      ch = getche();

      if (ch == '1') mortgage_comparisons();
      if (ch == '2') mortgage_amortization();
   } while(ch != 27);
   clrscr(7);
   return;
}


void mortgage_comparisons()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
{
   double amount_inc;
   double rate_inc;
   double nf;
   int i,j,ch;
   double rf,af,payment,p;

   clrscr(7);

   gotoxy( 1, 1); printf("�� Mortgage Comparison 敲");
   gotoxy( 1, 2); printf("훤袴袴袴袴袴袴袴袴袴袴袴�");

   get_params();

   amount_inc = amount*0.05;
   rate_inc = 0.5/1200;
   nf = months;

   clrscr(7);

   gotoxy(30, 2); printf("%3.0f year mortgage loan amounts",years);
   gotoxy( 1, 3);
   printf("旼컴컫컴컴컴컴쩡컴컴컴컫컴컴컴컴쩡컴컴컴컫컴컴컴컴쩡컴컴컴컫컴컴컴컴쩡컴컴컴커");
   gotoxy( 1, 4); printf("쿝ate�");

   gotoxy( 7, 4);
   for (i = 0; i < 8; i++)
   {
      printf("%8.0f�",amount+i*amount_inc);
   }
   gotoxy(1,5);
   printf("쳐컴컵컴컴컴컴탠컴컴컴컵컴컴컴컴탠컴컴컴컵컴컴컴컴탠컴컴컴컵컴컴컴컴탠컴컴컴캑");

   for (i = 0; i <= 14; i++)
   {
      gotoxy(1,6+i); printf("�%4.1f�",(float)((((mth_rate+(i*rate_inc))*120000)+0.5)/100));
   }

   for (i = 0; i <= 14; i++)
   {
      rf = mth_rate+i*rate_inc;
      af = 1.0;
      payment = af*(rf/(1-(1/(pow((1+rf),nf)))));
      gotoxy( 7,6+i);
      for (j = 0; j < 8; j++)
      {
         p = payment*(amount+j*amount_inc);
         p = ((p+5.000001E-03)*100)/100;
         printf("%8.2f�",p);
      }
   }
   gotoxy(1,21);
   printf("읕컴컨컴컴컴컴좔컴컴컴컨컴컴컴컴좔컴컴컴컨컴컴컴컴좔컴컴컴컨컴컴컴컴좔컴컴컴켸");

   gotoxy(1,25); printf("Press any key to continue");
   ch = getche();
   return;
}

void mortgage_amortization()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
{
   double period;
   int i;
   double payment;

   clrscr(7);

   gotoxy( 1, 1); printf("�� Mortgage Amortization 敲");
   gotoxy( 1, 2); printf("훤袴袴袴袴袴袴袴袴袴袴袴袴�");

   get_params();

   payment = amount*(mth_rate/(1-(1/(pow((1+mth_rate),months)))));
   payment = ((payment+5.000001e-03)*100)/100;
   gotoxy( 1, 9); printf("Monthly payments ====> %10.2f",payment);
   gotoxy( 1,10); printf("     Final value ====> %10.2f",(payment*months));
   gotoxy( 1,11); printf("  Total interest ====> %10.2f",(payment*months-amount));
   gotoxy( 1,13); printf("Calculating amortization");

   balance[0] = amount;
   for (i = 1; i <= (int)months; i++)
   {
      interest[i] = ((balance[i-1]*mth_rate+5.000001e-03)*100)/100;
      balance[i] = balance[i-1]-payment+interest[i];
   }

   gotoxy( 1,13); printf("                        ");
   gotoxy(45, 1); printf("旼컴컫컴컴컴컴쩡컴컴컴컫컴컴컴컴컴�");
   gotoxy(45, 2); printf("쿘nth쿛rincple쿔nterest� Balance  �");
   gotoxy(45, 3); printf("쳐컴컵컴컴컴컴탠컴컴컴컵컴컴컴컴컴�");
   period = 1.0;
   do
   {
      for ( i = (int)period; i < ((int)period + 20); i++)
      {
         gotoxy(45,(int)(4+i-period));
         if (i <= (int)months)
         {
            printf("�%4d",i);
            printf("�%8.2f",payment-interest[i]);
            printf("�%8.2f",interest[i]);
            printf("�%10.2f�",balance[i]);
         }
         else
            printf("�    �        �        �          �");
      }
      gotoxy(45,24); printf("읕컴컨컴컴컴컴좔컴컴컴컨컴컴컴컴컴�");

      do
      {
         gotoxy( 1,13); printf("    Month to view ===> ",months);
         strcpy(line,"   ");
	 last_char = (char)edline(line,"999",7);
         period = atof(line);
      } while((period > months) || (period < 0.0));
   } while (period != 0.0);
   return;
}

void get_params()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
{
   do
   {
      gotoxy( 1, 3); printf("  Mortgage amount ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      amount = atof(line);
   } while (amount <= 0.0);
   gotoxy(24, 3); printf("%10.2f",amount);

   do
   {
      gotoxy( 1, 4); printf("    Interest rate ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      rate = atof(line);
   } while ((rate > 35.0) || (rate < 0.0));
   gotoxy(24, 4); printf("%10.2f",rate);

   do
   {
      gotoxy( 1, 5); printf("  Number of years ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      years = atof(line);
   } while ((years >= 40.0) || (years <= 0.0));
   gotoxy(24, 5); printf("%10.2f",years);

   mth_rate = rate/1200;
   months = years * 12;

   gotoxy( 1, 7); printf(" Number of months ===> %10.2f",months);
   return;
}

