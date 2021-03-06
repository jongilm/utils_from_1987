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

/*
                                     mth_rate
   payment = amount   *     컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴
                                        1
                            1  -   컴컴컴컴컴컴컴
                                               months
                                   (1+mth_rate)


   payment = payment   +  0.005   컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴

*/
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
void mortgage_amortization(void);
void get_params(void);

void main()
/*
旼컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
�                                             �
읕컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴컴�
*/
{
   double period;
   int i;
   double payment;

   get_params();

   mth_rate = rate/1200;
   months = years * 12;
   payment = amount*(mth_rate/(1-(1/(pow((1+mth_rate),months)))));
   payment = ((payment+5.000001e-03)*100)/100;

   printf("  Mortgage amount ==> %10.2f\n",amount);
   printf("    Interest rate ==> %10.2f\n",rate);
   printf("  Number of years ==> %10.2f\n",years);
   printf(" Number of months ==> %10.2f\n",months);
   printf(" Monthly payments ==> %10.2f\n",payment);
   printf("      Final value ==> %10.2f\n",payment*months);
   printf("   Total interest ==> %10.2f\n",payment*months-amount);

   printf("旼컴컫컴컴컴컴쩡컴컴컴컫컴컴컴컴컴�\n");
   printf("쿘nth쿛rincple쿔nterest� Balance  �\n");
   printf("쳐컴컵컴컴컴컴탠컴컴컴컵컴컴컴컴컴�\n");

   balance[0] = amount;
   for (i = 1; i <= (int)months; i++)
   {
      interest[i] = ((balance[i-1]*mth_rate+5.000001e-03)*100)/100;
      balance[i] = balance[i-1]-payment+interest[i];
      printf("�%4d",i);
      printf("�%8.2f",payment-interest[i]);
      printf("�%8.2f",interest[i]);
      printf("�%10.2f�",balance[i]);
      printf("\n");
   }
   printf("읕컴컨컴컴컴컴좔컴컴컴컨컴컴컴컴컴�\n");

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
      printf("  Mortgage amount ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      amount = atof(line);
      printf("\r");
   } while (amount <= 0.0);
   printf("\n");

   do
   {
      printf("    Interest rate ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      rate = atof(line);
      printf("\r");
   } while ((rate > 35.0) || (rate < 0.0));
   printf("\n");

   do
   {
      printf("  Number of years ===> ");
      strcpy(line,"      ");
      last_char = (char)edline(line,"999999",7);
      years = atof(line);
      printf("\r");
   } while ((years > 40.0) || (years <= 0.0));
   printf("\n");

   return;
}

