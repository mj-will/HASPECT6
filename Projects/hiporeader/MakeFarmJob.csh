#!/bin/csh
#run : AppendFile.csh SUBMIT_SCRIPT DIRNAME FILE_PREFIX_TEMPLATE FILE_TYPE NFILES
 setenv FARMSCRIPT $1
 setenv DATADIR $2
 setenv RUNNO $3
 setenv OUTDIR $4
 
 set j = 0
 while ( $j <= $5 )
   printf "$DATADIR/out_clas_003877.evio.1.hipo " >> $farmscript
#   printf "$TEMPLATE$j$4 " >> $farmscript
   @ j++
 end
