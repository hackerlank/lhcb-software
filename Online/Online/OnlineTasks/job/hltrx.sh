rm -f /dev/shm/*
killall  Gaudi.exe
killall  test.exe
#export MSGSVC=MessageSvc
export MSGSVC=LHCb::DimMessageSvc
export WIDETERM='xterm  -sl 10000 -ls -132 -geometry 160x50 -title '

export TAN_PORT=YES
export TAN_NODE=$(hostname -f)
export test_exe=${ONLINEKERNELROOT}/${CMTCONFIG}/test.exe
export gaudi_exe="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=${MSGSVC} -auto"  
export gaudi_exe2="${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe libGaudiOnline.so OnlineTask -msgsvc=MessageSvc -auto"  

export UTGID=MBMInit   && ${gaudi_exe} -main=../options/MBMinit.opts  -opt=../options/Daemon.opts  -auto &
export UTGID=ErrServ   &&  ${gaudi_exe} -opt=../options/ErrorSrv.opts -auto &
sleep 2
$WIDETERM ErrorLogger@${HOST}    -e "export UTGID=ErrLog; $gaudi_exe2 -opts=../options/ErrorLogger.opts" &
sleep 3
$MINITERM ErrServ@${HOST} -e "setenv UTGID ErrServ   ; $gaudi_exe -opt=../options/ErrorSrv.opts" &
#
export UTGID=EvtHolder &&  ${gaudi_exe} -opt=../options/MEPHolder.opts    -auto &
#export UTGID=Moore_0   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts      -auto &
export UTGID=Moore_1   &&  ${gaudi_exe} -opt=../options/ReadMBM.opts       -auto &
export UTGID=MBMMon    && xterm -geometry 120x30 -e "${test_exe} mbm_mon" &
sleep 1
echo $# $*
sleep 1

export UTGID=EvtProd   && ${gaudi_exe} -opt=../options/MEPConverter.opts -msgsvc=${MSGSVC} -auto &

if [ $# -eq 1 ] ; then

echo  libGaudiOnline.so OnlineTask -opt=../options/MEPRxSvc.opts -msgsvc=${MSGSVC} -auto &
export UTGID=MEPRx      &&  gdb ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe 
else 
export UTGID=MEPRx      &&  ${gaudi_exe} -opt=../options/MEPRxSvc.opts -msgsvc=${MSGSVC} -auto &
fi

echo libGaudiOnline.so OnlineTask  -opt=../options/ReadMBM.opts      -msgsvc=${MSGSVC} -auto

#export UTGID=Moore_2 && gdb -x meprx.gdb ${GAUDIONLINEROOT}/${CMTCONFIG}/Gaudi.exe
