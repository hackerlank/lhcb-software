#! /bin/csh -f
# $Header: /afs/cern.ch/project/cvs/reps/lhcb/Ex/DetCondExample/doc/makedoc.csh,v 1.1.1.1 2001-09-14 15:45:03 andreav Exp $

#------------------------------
# Make Doxygen documentation
#------------------------------
if ( ! (${?DETCONDEXAMPLE_DOXYGEN} && ${?DETCONDEXAMPLE_DOXYURL}) ) then
  echo Environment variables not set
  echo Source ${DETCONDEXAMPLEROOT}/cmt/setup.csh and try again
  exit
endif

cd ${DETCONDEXAMPLEROOT}/doc

if ( ${?GAUDI_DOXYURL} && ${?GAUDI_DOXYGEN} ) then
else
  setenv GAUDI_DOXY_UNDEFINED 1
  setenv GAUDI_DOXYURL http://cern.ch/gaudi/Doxygen/v8/doc/html
  setenv GAUDI_DOXYGEN /afs/cern.ch/sw/Gaudi/releases/Gaudi/v8/doc/
endif
echo Using dOxygen documentation for the Gaudi framework taken from URL
echo ${GAUDI_DOXYURL}
echo Using dOxygen tag for the Gaudi framework taken from 
echo ${GAUDI_DOXYGEN}  

/bin/rm -f Doxyfooter.html
/bin/rm -f Doxyheader.html
if ( ${?MAILSERV_GEM} ) then
  sed s/USERNAME/${USER}/g DoxyfooterSchema.html | \
  sed s/USERMAIL/${MAILSERV_GEM}/g > Doxyfooter.html
else
  sed s/USERNAME/${USER}/g DoxyfooterSchema.html | \
  sed /USERMAIL/d > Doxyfooter.html
endif
cat DoxyheaderSchema.html \
  | sed "s|CONDDB_DOXYURL|${CONDDB_DOXYURL}|g" \
  | sed "s|DETCOND_DOXYURL|${DETCOND_DOXYURL}|g" \
  | sed "s|GAUDI_DOXYURL|${GAUDI_DOXYURL}|g" \
  > Doxyheader.html

${GAUDISOFT}/../extsoft/doxygen-1.2.5/bin/doxygen DoxyFile.cfg
cp gaudiSF.gif ${DETCONDEXAMPLE_DOXYGEN}

/bin/rm -f Doxyfooter.html
/bin/rm -f Doxyheader.html

${DETCONDEXAMPLE_DOXYGEN}/installdox \
  -l Gaudi.tag@${GAUDI_DOXYURL} \
  -l ConditionsDB.tag@${CONDDB_DOXYURL} \
  -l DetCond.tag@${DETCOND_DOXYURL}

if ( ${?GAUDI_DOXY_UNDEFINED} ) then
  unsetenv GAUDI_DOXY_UNDEFINED
  unsetenv GAUDI_DOXYURL
  unsetenv GAUDI_DOXYGEN
endif

echo You can now browse the documentation under 
echo ${DETCONDEXAMPLE_DOXYURL}/index.html

