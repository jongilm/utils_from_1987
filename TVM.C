#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <tools.h>

double BAL[500];
double INT[500];
double INT_TODATE[500];

double PV;
double FV;
double I;
double N;
double PMT;
double P;
double INF;
double accum[6];

void main()
/*
зддддддддддддддддддддддддддддддддддддддддддддд©
Ё                                             Ё
юддддддддддддддддддддддддддддддддддддддддддддды
*/
{
   int i;

   PV            = 100000;
   FV            = 0;
   I             = 21;
   INF           = 15;
   N             = 240;
   PMT           = -1777.64294039;
   P             = 1; /* beginning of period */
   P             = 0; /* end of period       */

   PMT           = tvm_pmt(PV,FV,I,N,PMT,P);
/* PV            = tvm_pv (PV,FV,I,N,PMT,P);  */
/* FV            = tvm_fv (PV,FV,I,N,PMT,P);  */

   BAL[0]        = PV;
   INT[0]        = 0;
   INT_TODATE[0] = 0;

   for (i = 1; i <= (int)N; i++)
   {
      INT[i]        = -BAL[i-1] * I/1200;
      BAL[i]        =  BAL[i-1] + PMT - INT[i];
      INT_TODATE[i] = INT_TODATE[i-1] + INT[i];
   }

   printf("Present Value  PV ==> %10.2f\n",PV);
   printf("Future Value   FV ==> %10.2f\n",FV);
   printf("Interest        I ==> %10.2f\n",I);
   printf("Periods         N ==> %10.2f (%10.2f yrs)\n",N,N/12.0);
   printf("Payment       PMT ==> %10.2f\n",PMT);
   printf("Inflation     INF ==> %10.2f\n",INF);
   printf("Timing          P ==> %s\n", P ? "Beginning":"End");

   printf("\n");


   printf("здддрдддддддддддддддддддддддддддддддддддддддддддрддддддддддддддддддддддддддддддддддддддддддд©\n");
   printf("Ё   ╨ Real amount                               ╨Value of real amount in todays money       Ё\n");
   printf("Ё   гддддддддддбддддддддддбддддддддддбддддддддддвддддддддддбддддддддддбддддддддддбдддддддддд╢\n");
   printf("ЁMth╨ Payment  Ё Princple Ё Interest Ё Balance  ╨ Payment  Ё Princple Ё Interest ЁBalance   Ё\n");
   printf("цдддвддддддддддеддддддддддеддддддддддеддддддддддвддддддддддеддддддддддеддддддддддедддддддддд╢\n");

   accum[0]      = 0;
   accum[1]      = 0;
   accum[2]      = 0;
   accum[3]      = 0;
   accum[4]      = 0;
   accum[5]      = 0;

   for (i = 1; i <= (int)N; i++)
   {
      accum[0]      += PMT;
      accum[1]      += PMT-INT[i];
      accum[2]      += INT[i];
      accum[3]      += tvm_pv(PV,-(PMT)       ,INF,(double)i,0,1);
      accum[4]      += tvm_pv(PV,-(PMT-INT[i]),INF,(double)i,0,1);
      accum[5]      += tvm_pv(PV,-(INT[i])    ,INF,(double)i,0,1);

      printf("Ё%3d"   ,i);
      printf("╨%10.2f" ,PMT);
      printf("Ё%10.2f" ,PMT-INT[i]);
      printf("Ё%10.2f" ,INT[i]);
      printf("Ё%10.2f" ,BAL[i]);
      printf("╨%10.2f" ,tvm_pv(PV,-(PMT)       ,INF,(double)i,0,1));
      printf("Ё%10.2f" ,tvm_pv(PV,-(PMT-INT[i]),INF,(double)i,0,1));
      printf("Ё%10.2f" ,tvm_pv(PV,-(INT[i])    ,INF,(double)i,0,1));
      printf("Ё%10.2f" ,tvm_pv(PV,-(BAL[i])    ,INF,(double)i,0,1));
      printf("Ё\n");

      if ((i%12) == 0)
      {
         printf("цдддвддддддддддеддддддддддеддддддддддеддддддддддвддддддддддеддддддддддеддддддддддедддддддддд╢\n");
         printf("ЁTOT");
         printf("╨%10.2f" ,accum[0]);
         printf("Ё%10.2f" ,accum[1]);
         printf("Ё%10.2f" ,accum[2]);
         printf("Ё          ");
         printf("╨%10.2f" ,accum[3]);
         printf("Ё%10.2f" ,accum[4]);
         printf("Ё%10.2f" ,accum[5]);
         printf("Ё          ");
         printf("Ё\n");
         if (i != (int)N)
            printf("цдддвддддддддддеддддддддддеддддддддддеддддддддддвддддддддддеддддддддддеддддддддддедддддддддд╢\n");
      }
   }
   printf("юдддпддддддддддаддддддддддаддддддддддаддддддддддпддддддддддаддддддддддаддддддддддадддддддддды\n");
   return;
}

