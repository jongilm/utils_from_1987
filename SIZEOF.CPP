#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <iostream.h>

void DoCheck( int TempCount , int ThisSize )
{
    short FillSize;
    
    FillSize = TempCount % ThisSize;
    if ( FillSize )
    {
        cout << "********Filler [" << ThisSize - FillSize << "]" << endl;
    }
}

short CountBytes( FILE *ptr )
{
    char   TempString[255] , OtherTemp[255] , ope[10];
    char   *p;
    short  TempCount; 
    char   *TempOPos , *TempCPos;

    fgets( TempString , 254 , ptr );

    cout << TempString << endl;

    TempCount = 0;

    if ( strchr( TempString , '{' ) )
    {
        while ( !strchr ( TempString , '}' ) )
        {
           fgets( TempString , 254 , ptr );
           strcpy( OtherTemp , TempString );
           p = strtok( TempString , " " );

           if ( !strcmp( p , "char" ) )
           {
               p = strtok( NULL, " " );
               cout << "   char " << p << endl;

               TempOPos = strchr( OtherTemp , '[' );
               TempCPos = strchr( OtherTemp , ']' );

               if ( TempOPos && TempCPos )
               {
                   strcpy( ope , "   " );
                   strncpy( ope , ( TempOPos + 1 ) , ( TempCPos - TempOPos ) - 1 );
                   TempCount += atoi( ope );
               }
               else
               {
                   TempCount++;
               }
           }

           if ( !strcmp( p , "int" ) )
           {
               p = strtok( NULL, " " );
               DoCheck( TempCount , sizeof( int ));
               TempCount += sizeof( int );
               cout << "   int  " << p << endl;
           }

           if ( !strcmp( p , "long" ) )
           {
               p = strtok( NULL, " " );
               DoCheck( TempCount , sizeof( long ));
               TempCount += sizeof( long );
               cout << "   long " << p << endl;
           }

           if ( !strcmp( p , "short" ) )
           {
               p = strtok( NULL, " " );
               DoCheck( TempCount , sizeof( short ));
               TempCount += sizeof( short );
               cout << "   short " << p << endl;
           }

           if ( !strcmp( p , "float" ) )
           {
               p = strtok( NULL, " " );
               DoCheck( TempCount , sizeof( float ));
               TempCount += sizeof( float );
               cout << "   float " << p << endl;
           }

           if ( !strcmp( p , "double" ) )
           {
               p = strtok( NULL, " " );
               DoCheck( TempCount , sizeof( double ));
               TempCount += sizeof( double );
               cout << "   double " << p << endl;
           }
        }
    }
    cout << TempString << endl;
    return ( TempCount );
}

void ReadFile( char *FileName )
{
    char TempString[255];
    FILE *FilePtr;
    short Count;

    FilePtr = fopen( FileName , "rt" );

    if ( FilePtr )
    {
        while ( !feof( FilePtr ) )
        {
            fgets( TempString , 254 , FilePtr );
            if ( strstr( TempString , " struct " ) )
            {
                cout << TempString << endl;
                Count = CountBytes( FilePtr );
                cout << "Total Size = [" << Count << "]" << endl;
                cout << endl;
            }
        }
        fclose( FilePtr );
        return;
    }

    cout << "Error reading File" << endl;
}

int main( int argc , char *argv[] )
{
    if ( argc > 1 )
    {
        clrscr(  );
        ReadFile( argv[1] );
    }
    else
    {
        cout << " Usage : sizeof filename " << endl;
    }
    return ( 0 );
}
