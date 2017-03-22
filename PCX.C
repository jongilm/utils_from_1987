#include <esiproto.h>
#include <pcx.h>

void main(int argc, char *argv[]);
void esgetkey(int*);

void main(int argc, char *argv[])
{
   #pragma pack(1)
   struct pcx_hdr info;
   #pragma pack()
   int i,k;

   if (argc == 2)
   {
      i = pcx_info(argv[1], &info);
      setvga();
      initgraf(info.vid_mode, info.pal_num, info.backgrd);
      i = pcx_load(argv[1]);
      esgetkey(&k);
      initgraf(3,0,0);
   }
}
