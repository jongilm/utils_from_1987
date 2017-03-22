#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <graph.h>
#include <dos.h>


main()
{
  int x,y,i,j,dx,dy;
  if (_setvideomode(_MAXRESMODE)) /* 640*480 */
  {
     _setcolor(7);
     _floodfill(0,0,7);

     x  = 0;
     y  = 200;
     dx = 100;
     dy = 50;

     for (i=x;i<640-dx;i++)
     {
        _setcolor(14);
        _rectangle(_GBORDER,i,y,i+dx,y+dy);
        _setcolor(4);
        _floodfill(i+(dx/2),y+(dy/2),14);
        /*for (j=0;j<1000;j++)*/

        _setcolor(7);
        _rectangle(_GFILLINTERIOR,i-1   ,y,i-1   ,y+dy);
        _setcolor(4);
        _rectangle(_GFILLINTERIOR,i-1+dx,y+1,i-1+dx,y+dy-1);
        if (kbhit()) break;

     }
     getch();
     _setvideomode(_DEFAULTMODE);
  } else puts("Oops");
}
