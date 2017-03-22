
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>
#include <math.h>
#include <graph.h>

char *descr = NULL;
char answer[200];

void plot ( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, char *Heading, char *SubHeading )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    if( !_setvideomode( _MAXRESMODE ) ) /* Find a valid graphics mode */
        exit( 1 );                      /* No graphics available      */

   if (Heading)
   {
      _settextposition(1,1);
      printf(Heading);
   }
   if (SubHeading)
   {
      _settextposition(2,1);
      printf(SubHeading);
   }

   x1*=40; y1*=40; x1+=50; y1=450-y1;
   x2*=40; y2*=40; x2+=50; y2=450-y2;
   x3*=40; y3*=40; x3+=50; y3=450-y3;
   x4*=40; y4*=40; x4+=50; y4=450-y4;

   _moveto(50,50);
   _lineto(50,450);
   _moveto(50,450);
   _lineto(450,450);

   _moveto(x1,y1);
   _lineto(x2,y2);
   _moveto(x3,y3);
   _lineto(x4,y4);
   getch();
    _setvideomode( _DEFAULTMODE );
}

int intersect ( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double *px, double *py )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   double m1,m2,c1,c2,x,y,i;


   // A: do the rectangles overlap
   // this will also exclude non-skew and parallel lines
   if ( (x3>x1 && x4>x1 && x3>x2 && x4>x2) ||
        (y3>y1 && y4>y1 && y3>y2 && y4>y2) ||
        (x3<x1 && x4<x1 && x3<x2 && x4<x2) ||
        (y3<y1 && y4<y1 && y3<y2 && y4<y2) )
   {
      sprintf(answer,"NO: Rectangles don't overlap");
      return 0;
   }

   // B: if both lines are vertical (infinite gradient or points)
   if ((x2==x1) && (x4==x3))
   {
      sprintf(answer,"YES: Both lines are vertical and overlap");
      *px = x1;
      *py = (y1+y2+y3+y4)/4;
      return 1;
   }

   // B: if either line is vertical (infinite gradient)
   if (x2==x1)
   {
       x = x1;
       m2 = (y4-y3)/(x4-x3);
       c2 = y3-m2*x3;
       y = m2*x + c2;
   }
   else if (x4==x3)
   {
       x = x3;
       m1 = (y2-y1)/(x2-x1);
       c1 = y1-m1*x1;
       y = m1*x + c1;
   }
   else
   {
       //y = m1x+c1
       //m=dy/dx
       m1 = (y2-y1)/(x2-x1);
       m2 = (y4-y3)/(x4-x3);
       //c=y-mx
       c1 = y1-m1*x1;
       c2 = y3-m2*x3;
       if (m1 == m2) // the lines are skew and parallel
       {
          if (c1 == c2) // the lines are the same, a dashed line is excluded in A: above
          {
             sprintf(answer,"YES: The lines are the same");
             *px = (x1+x2+x3+x4)/4;
             *py = (y1+y2+y3+y4)/4;
             return 1;
          }
          sprintf(answer,"NO: The lines are skew and parallel");
          return 0;
       }
       //printf("m1=%lf\t",m1);
       //printf("m2=%lf\t",m2);
       //printf("c1=%lf\t",c1);
       //printf("c2=%lf\n",c2);
     //y = m1x+c1
     //y = m2x+c2
     //m1x+c1 = m2x+c2
     //m1x-m2x = c2-c1
     //(m1-m2)x = c2-c1
       x = (c2-c1)/(m1-m2);
       y = m1*x + c1;
   }
   // We now have the point at which the two lines would meet if they
   // were of infinite length.
   // Check if the point falls within the bounds of each line
   // Check if the point falls within the bounding rectangle of each line

   // normalise all the coords
   if (x1>x2) i=x2,x2=x1,x1=i;
   if (y1>y2) i=y2,y2=y1,y1=i;
   if (x3>x4) i=x4,x4=x3,x3=i;
   if (y3>y4) i=y4,y4=y3,y3=i;

   if ( x<x1 || x>x2 || x<x3 || x>x4 )
   {
      sprintf(answer,"NO: The lines would intersect at [%lf,%lf] if they were infinite",x,y);
      return 0;
   }
   *px = x;
   *py = y;
   sprintf(answer,"YES: The lines intersect at [%lf,%lf]\n",x,y );
   return 1;
}

int intersect1 ( double x1, double y1, double x2, double y2, double x3, double y3, double x4, double y4, double *px, double *py )
{
   intersect ( x1, y1, x2, y2, x3, y3, x4, y4, px, py );
   plot ( x1,  y1,  x2,  y2,  x3,  y3,  x4,  y4, descr, answer );
}

int main ( int argc, char *argv[] )
{
   double x,y;
   //intersect ( x1, y1, x2, y2, x3, y3, x4, y4, &x, &y );
   descr = "Two identical points                           ";    intersect1 ( 0, 0, 0, 0, 0, 0, 0, 0, &x, &y );
   descr = "Two identical points                           ";    intersect1 ( 1, 2, 1, 2, 1, 2, 1, 2, &x, &y );
   descr = "Two identical lines                            ";    intersect1 ( 1, 2, 3, 4, 1, 2, 3, 4, &x, &y );
   descr = "Two parallel horoz lines                       ";    intersect1 ( 1, 2, 1, 4, 2, 2, 2, 4, &x, &y );
   descr = "Two parallel vert lines                        ";    intersect1 ( 1, 2, 2, 2, 2, 3, 4, 3, &x, &y );
   descr = "Two skew lines that would intersect if infinite";    intersect1 ( 0, 4, 6, 10, 5, 5, 9, 0, &x, &y );
   descr = "Two skew lines that do intersect               ";    intersect1 ( 1, 3, 4, 4, 0, 7, 5, 0, &x, &y );
   descr = "One vert, one skew that do intersect           ";    intersect1 ( 1, 0, 1, 5, 0, 2, 2, 4, &x, &y );
   descr = "One vert, one skew that do not intersect       ";    intersect1 ( 4, 0, 4, 5, 0, 2, 2, 3, &x, &y );
   descr = "Two horoz lines that partially overlap         ";    intersect1 ( 0, 5, 5, 5, 1, 5, 6, 5, &x, &y );
   descr = "Two vert lines that partially overlap          ";    intersect1 ( 5, 0, 5, 5, 5, 1, 5, 6, &x, &y );
   return(0);
}
