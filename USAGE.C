
#include <stdlib.h>
#include <stdio.h>
#include <conio.h>
#include <string.h>
#include <dos.h>

int  NormalPath = 0;
long BytesPerCluster = 0;
long TotalDiskSpace = 0;
long DiskSize   = 0;
long DiskSpace  = 0;
long FreeSpace  = 0;

char *fmt ( long value, int Which )
{
   //       0123456789012
   // 1G = "1 000 000 000"
   // 1G = "   1000000000"
   static char Str1[14];
   static char Str2[14];
   char *Str;

   if (Which == 1)
      Str = Str1;
   else
      Str = Str2;

   sprintf( Str, "%13ld", value );
   Str[ 0] = Str[ 3];
   Str[ 1] = ' ';
   Str[ 2] = Str[ 4];
   Str[ 3] = Str[ 5];
   Str[ 4] = Str[ 6];
   Str[ 5] = ' ';
   Str[ 6] = Str[ 7];
   Str[ 7] = Str[ 8];
   Str[ 8] = Str[ 9];
   Str[ 9] = ' ';
   return Str;
}
int Slack ( long Space, long Size )
{
   double Perc;

   if (!NormalPath)
      return 0;
   if (Space == 0)
      return 0;
   Perc = ( (double)(Space-Size)*100 ) / Space;

   return (int)Perc;
}

long SpaceUsed ( long size )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   long Used;

   if (size == 0)
      return 0;

   Used = (size / BytesPerCluster) * BytesPerCluster;

   if (Used != size)
      Used += BytesPerCluster;

   return Used;
}

void GetBytesPerCluster ( char * aPath )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    struct _diskfree_t drvinfo;
    unsigned drive;

    if (aPath && aPath[0])
       drive = toupper(aPath[0]) - 'A' + 1;
    else
       _dos_getdrive( &drive );

    _dos_getdiskfree( drive, &drvinfo );

   BytesPerCluster = (long)drvinfo.sectors_per_cluster * drvinfo.bytes_per_sector;
   TotalDiskSpace = (long)drvinfo.total_clusters * drvinfo.sectors_per_cluster * drvinfo.bytes_per_sector;
   if (BytesPerCluster == 0)
      exit(99);
}

void GetFreeSpace ( char * aPath )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
    struct _diskfree_t drvinfo;
    unsigned drive;

    if (aPath && aPath[0])
       drive = toupper(aPath[0]) - 'A' + 1;
    else
       _dos_getdrive( &drive );

    _dos_getdiskfree( drive, &drvinfo );

    FreeSpace = (long)drvinfo.avail_clusters *
                drvinfo.sectors_per_cluster *
                drvinfo.bytes_per_sector;
}


void SizeofTree ( char *aPath, long *pTreeSize, long *pTreeSpace  )
/*ÚÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄ¿*/
/*³                                                  ³*/
/*ÀÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÄÙ*/
{
   struct _find_t find;
   char FullPath [_MAX_PATH+13];

   strcpy(FullPath,aPath);
   strcat(FullPath,"\\*.*");

   if ( _dos_findfirst( FullPath, 0xffff, &find ) == 0 )
   {
      *pTreeSize  += find.size;
      if (NormalPath)
         *pTreeSpace += SpaceUsed(find.size);
      if (( find.attrib & _A_SUBDIR ) && (find.name[0] != '.' ) )
      {
         strcpy(FullPath,aPath);
         strcat(FullPath,"\\");
         strcat(FullPath,find.name);
         SizeofTree ( FullPath, pTreeSize, pTreeSpace );
      }
      while( _dos_findnext( &find ) == 0 )
      {
         *pTreeSize  += find.size;
         if (NormalPath)
            *pTreeSpace += SpaceUsed(find.size);
         if (( find.attrib & _A_SUBDIR ) && (find.name[0] != '.' ) )
         {
            strcpy(FullPath,aPath);
            strcat(FullPath,"\\");
            strcat(FullPath,find.name);
            SizeofTree ( FullPath, pTreeSize, pTreeSpace );
         }
      }
   }
}

void ShowDirectoriesAndUsage ( char *aPath )
{
   struct _find_t find;
   char FullPath [_MAX_PATH+13];
   long DirSize    = 0;
   long DirSpace   = 0;
   long TreeSize   = 0;
   long TreeSpace  = 0;

   strcpy(FullPath,aPath);
   strcat(FullPath,"\\*.*");

   if ( _dos_findfirst( FullPath, 0xffff, &find ) == 0 )
   {
      DirSize   += find.size;
      if (NormalPath)
         DirSpace  += SpaceUsed(find.size);
      DiskSize  += find.size;
      if (NormalPath)
         DiskSpace += SpaceUsed(find.size);
      if (( find.attrib & _A_SUBDIR ) && (find.name[0] != '.' ) )
      {
         strcpy(FullPath,aPath);
         strcat(FullPath,"\\");
         strcat(FullPath,find.name);
         TreeSize = 0;
         TreeSpace  = 0;
         SizeofTree ( FullPath, &TreeSize, &TreeSpace );
         DiskSize += TreeSize;
         if (NormalPath)
            DiskSpace += TreeSpace;
         printf( "%-35s %s %s %3d%%\n", FullPath, fmt(TreeSize,1), fmt(TreeSpace,2), Slack(TreeSpace,TreeSize) );
      }
      while( _dos_findnext( &find ) == 0 )
      {
         DirSize   += find.size;
         if (NormalPath)
            DirSpace  += SpaceUsed(find.size);
         DiskSize  += find.size;
         if (NormalPath)
            DiskSpace += SpaceUsed(find.size);
         if (( find.attrib & _A_SUBDIR ) && (find.name[0] != '.' ) )
         {
            strcpy(FullPath,aPath);
            strcat(FullPath,"\\");
            strcat(FullPath,find.name);
            TreeSize   = 0;
            TreeSpace  = 0;
            SizeofTree ( FullPath, &TreeSize, &TreeSpace );
            DiskSize  += TreeSize;
            if (NormalPath)
               DiskSpace += TreeSpace;
            printf( "%-35s %s %s %3d%%\n", FullPath, fmt(TreeSize,1), fmt(TreeSpace,2), Slack(TreeSpace,TreeSize) );
         }
      }
   }
   strcpy(FullPath,aPath);
   strcat(FullPath,"\\*.*");
   printf( "%-35s %s %s %3d%%\n", FullPath, fmt(DirSize,1), fmt(DirSpace,2), Slack(DirSpace,DirSize) );
   printf( "%-29s Total %s %s %3d%%\n", aPath, fmt(DiskSize,1), fmt(DiskSpace,2), Slack(DiskSpace,DiskSize) );
}

int main ( int argc, char *argv[] )
{
   if (argc != 2)
   {
       printf( "  SYNTAX: USAGE <Disk>" );
       exit( 1 );
   }
   if (toupper(argv[1][0]) >= 'A' && toupper(argv[1][0]) <= 'Z' )
   {
      argv[1][0] = toupper(argv[1][0]);
      NormalPath = 1;
   }
   if (NormalPath)
   {
      GetBytesPerCluster      ( argv[1] );
      GetFreeSpace            ( argv[1] );
   }
   printf( "%-35s %13s %13s %-3s\n", "Path", "Size", "Space", "Slack" );
   printf( "%-35s %13s %13s %-3s\n", "----", "----", "-----", "-----" );
   ShowDirectoriesAndUsage ( argv[1] );
   if (NormalPath)
   {
      printf( "%c%-34s               %s\n", argv[1][0], ": Directories etc.", fmt(TotalDiskSpace-(DiskSpace+FreeSpace),1) );
      printf( "%c%-34s               %s\n", argv[1][0], ": FREE"       , fmt(FreeSpace,1)      );
      printf( "%c%-34s               %s\n", argv[1][0], ": TOTAL"      , fmt(TotalDiskSpace,1) );
   }
   return(0);
}
