#include <stdlib.h>
#include <stdio.h>
#include <tools.h>

double PV, FV, I, N, PMT, P;

void main()
{

   P             = MORTGAGE;

   PV            = 100000;
   FV            = 0;
   N             = 240;
   I             = 21;
   PMT           = -1777.64294039;

/* PV            = tvm_pv (PV,FV,I,N,PMT,P); */
/* FV            = tvm_fv (PV,FV,I,N,PMT,P); */
/* N             = tvm_n  (PV,FV,I,N,PMT,P); */
   I             = tvm_i  (PV,FV,I,N,PMT,P);
/* PMT           = tvm_pmt(PV,FV,I,N,PMT,P); */

   printf("PV   ==> %-16.4f\n",PV  );
   printf("FV   ==> %-16.4f\n",FV  );
   printf("N    ==> %-16.4f\n",N   );
   printf("I    ==> %-16.4f\n",I   );
   printf("PMT  ==> %-16.4f\n",PMT );
}

