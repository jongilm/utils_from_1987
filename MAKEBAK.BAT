rem @echo off
set ZIPTO=e:\$NEDBAK
md_date %ZIPTO%\bkyymmdd
if errorlevel 1 goto error

rem echo *.c      >  include.lst
rem echo *.pas    >> include.lst
rem echo *.h      >> include.lst
rem echo *.inc    >> include.lst
rem echo *.sys    >> include.lst
rem echo *.dic    >> include.lst
rem echo *.msk    >> include.lst
rem echo *.rul    >> include.lst
rem echo *.pop    >> include.lst
rem echo *.PRN    >> include.lst
rem echo *.INI    >> include.lst
rem echo MAKEFILE >> include.lst

echo *.exe    >  exclude.lst
echo *.obj    >> exclude.lst
echo *.o0?    >> exclude.lst
echo *.c0?    >> exclude.lst
echo *.lib    >> exclude.lst
echo *.zip    >> exclude.lst
echo *.dll    >> exclude.lst
echo *.map    >> exclude.lst
echo *.res    >> exclude.lst
echo *.exc    >> exclude.lst
echo *.exf    >> exclude.lst
echo *.da1    >> exclude.lst
echo *.z      >> exclude.lst
echo *.pdb    >> exclude.lst
echo *.vcw    >> exclude.lst
echo *.sbr    >> exclude.lst
echo deleted\*.* >> exclude.lst
echo deleted.*   >> exclude.lst
echo index       >> exclude.lst
echo junk*.*     >> exclude.lst

pkzip -urPex -x@exclude.lst %ZIPTO%\admin.zip     \admin\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\adv.zip       \adv\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\atmbal.zip    \atmbal\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\brnutils.zip  \brnutils\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis.zip      \ccis\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis_bd.zip   \ccis_bd\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis_bi.zip   \ccis_bi\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis_bs.zip   \ccis_bs\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis_lg.zip   \ccis_lg\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ccis_sr.zip   \ccis_sr\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\ce.zip        \ce\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\cellgate.zip  \cellgate\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\coms.zip      \coms\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\curracs.zip   \curracs\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\hogan.zip     \hogan\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\host.zip      \host\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\nb.zip        \nb\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\nedinter.zip  \nedinter\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\nedntact.zip  \nedntact\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\openac.zip    \openac\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\openit.zip    \openit\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\permadmn.zip  \permadmn\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\permtt.zip    \permtt\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\sal.zip       \sal\*.*
pkzip -urPex -x@exclude.lst %ZIPTO%\teller.zip    \teller\*.*
goto Done

:error
echo ERROR: MD_DATE failed. Quiting.

:Done
del exclude.lst
echo Done.
