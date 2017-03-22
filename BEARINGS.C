#include <stdlib.h>
#include <stdio.h>

int i;
int x[13];

int compare(int a, int b)
{
   if (a == b)     return(0);
   else if (a > b) return(1);
   else            return(2);
}

void main(int argc,char *argv[])
{
   printf("BEARINGS - Jonathan Gilmore 1990\n\n");
   if (argc < 13)
   {
      printf("PROBLEM  : Given 12 identically looking bearings and a simple scale\n");
      printf("           and being told ONE bearing is faulty (either heavier or \n");
      printf("           lighter), establish which is the faulty bearing (and    \n");
      printf("           whether it is lighter or heavier) in no more than THREE \n");
      printf("           weighings.\n\n");
      printf("USAGE    : BEARINGS Wn\n");
      printf("           where Wn is the weight of the nth bearing for n = 1..12\n\n");
      return;
   }
   for (i=1;i<=12;i++) x[i] = atoi(argv[i]);

   printf("compare 1,2,3,4 with 5,6,7,8\n");
   switch (compare(x[1]+x[2]+x[3]+x[4],x[5]+x[6]+x[7]+x[8]))
   {
      case 0 : printf("equal\n");
               printf("9,10,11,12 heavier or lighter\n\n");
               printf("compare 1,2,3 with 9,10,11\n");
               switch (compare(x[1]+x[2]+x[3],x[9]+x[10]+x[11]))
               {
                  case 0 : printf("equal\n");
                           printf("12 heavier or lighter\n\n");
                           printf("compare 1 with 12\n");
                           switch (compare(x[1],x[12]))
                           {
                              case 0 : printf("equal\n");
                                       printf("all the same\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("12 is lighter\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("12 is heavier\n");
                                       break;
                           }
                           break;
                  case 1 : printf("heavier\n");
                           printf("9,10,11 lighter\n\n");
                           printf("compare 9 with 10\n");
                           switch (compare(x[9],x[10]))
                           {
                              case 0 : printf("equal\n");
                                       printf("11 is lighter\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("10 is lighter\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("9 is lighter\n");
                                       break;
                           }
                           break;
                  case 2 : printf("lighter\n");
                           printf("9,10,11 heavier\n\n");
                           printf("compare 9 with 10\n");
                           switch (compare(x[9],x[10]))
                           {
                              case 0 : printf("equal\n");
                                       printf("11 is heavier\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("9 is heavier\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("10 is heavier\n");
                                       break;
                           }
                           break;
               }
               break;
      case 1 : printf("heavier\n");
               printf("1,2,3,4 heavier or 5,6,7,8 lighter\n\n");
               printf("compare 1,2,5 with 3,4,6\n");
               switch (compare(x[1]+x[2]+x[5], x[3]+x[4]+x[6]))
               {
                  case 0 : printf("equal\n");
                           printf("7,8 lighter\n\n");
                           printf("compare 7 with 8\n");
                           switch (compare(x[7],x[8]))
                           {
                              case 0 : printf("equal\n");
                                       printf("all the same\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("8 is lighter\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("7 is lighter\n");
                                       break;
                           }
                           break;
                  case 1 : printf("heavier\n");
                           printf("1,2 heavier or 6 lighter\n\n");
                           printf("compare 1 with 2\n");
                           switch (compare(x[1],x[2]))
                           {
                              case 0 : printf("equal\n");
                                       printf("6 is lighter\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("1 is heavier\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("2 is heavier\n");
                                       break;
                           }
                           break;
                  case 2 : printf("lighter\n");
                           printf("3,4 heavier or 5 lighter\n\n");
                           printf("compare 3 with 4\n");
                           switch (compare(x[3],x[4]))
                           {
                              case 0 : printf("equal\n");
                                       printf("5 is lighter\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("3 is heavier\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("4 is heavier\n");
                                       break;
                           }
                           break;
               }
               break;
      case 2 : printf("lighter\n");
               printf("1,2,3,4 lighter or 5,6,7,8 heavier\n\n");
               printf("compare 1,2,5 with 3,4,6\n");
               switch (compare(x[1]+x[2]+x[5], x[3]+x[4]+x[6]))
               {
                  case 0 : printf("equal\n");
                           printf("7,8 heavier\n\n");
                           printf("compare 7 with 8\n");
                           switch (compare(x[7],x[8]))
                           {
                              case 0 : printf("equal\n");
                                       printf("all the same\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("7 is heavier\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("8 is heavier\n");
                                       break;
                           }
                           break;
                  case 1 : printf("heavier\n");
                           printf("3,4 lighter or 5 heavier\n\n");
                           printf("compare 3 with 4\n");
                           switch (compare(x[3],x[4]))
                           {
                              case 0 : printf("equal\n");
                                       printf("5 is heavier\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("4 is lighter\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("3 is lighter\n");
                                       break;
                           }
                           break;
                  case 2 : printf("lighter\n");
                           printf("1,2 lighter or 6 heavier\n\n");
                           printf("compare 1 with 2\n");
                           switch (compare(x[1],x[2]))
                           {
                              case 0 : printf("equal\n");
                                       printf("6 is heavier\n");
                                       break;
                              case 1 : printf("heavier\n");
                                       printf("2 is lighter\n");
                                       break;
                              case 2 : printf("lighter\n");
                                       printf("1 is lighter\n");
                                       break;
                           }
                           break;
               }
               break;
   }
}
