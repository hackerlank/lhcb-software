
# set -x

# get the location of this very script


a=`history 1 | awk '{print $3}'`
#b=`basename $a`
l=`readlink $a`
if [ "$l" = "" ]; then
	l=$a
	scriptsdir=`dirname $a`
else
	pushd `dirname $a` &> /dev/null
	pushd `dirname $l` &> /dev/null
	scriptsdir=`pwd`
	popd &> /dev/null
	popd &> /dev/null
fi

if [ "$scriptsdir" = "." ]; then
  scriptsdir=`pwd`
fi

echo $scriptsdir


#################################################################

if [ ! -e ${HOME}/.rhosts ]; then
	echo "Creating a ${HOME}/.rhosts to use CMT"
	echo " "
	echo "Joel.Closier@cern.ch"
	echo "+ ${USER}" > ${HOME}/.rhosts
fi

#################################################################
# parsing command line arguments

debug=0
mysiteroot=0
cmtconfig=0
userarea=0
cmtvers=0

TEMP=`getopt -o  d,m:,c:,u:,v: --long debug,mysiteroot:,cmtconfig:,userarea:,cmtvers: -- "$@"`

if [ $? != 0 ] ; then echo "Terminating..." >&2 ; exit 1 ; fi

# Note the quotes around `$TEMP': they are essential!
eval set -- "$TEMP"

while true ; do
	case "$1" in
	-d|--debug) debug=1 ; shift ;;
	-m|--mysiteroot) mysiteroot=$2 ; shift 2 ;;
	-c|--cmtconfig) cmtconfig=$2 ; shift 2 ;;
	-u|--userarea)  userarea=$2 ; shift 2 ;; 
	-v|--cmtvers)  cmtvers=$2 ; shift 2 ;; 
	--) shift ; break ;;
	*) echo "Internal error!" ; exit 1 ;;
	esac
done
echo "Remaining arguments:"
for arg do echo '--> '"\`$arg'" ; done

###################################################################################
echo "debug $debug"
echo "mysiteroot $mysiteroot"
echo "cmtconfig $cmtconfig"
echo "userarea $userarea"
echo "cmtvers $cmtvers"

echo "Returning ..."
return 
echo "Returned"

#set -x

# set default values for CMT and gcc versions
comp=`gcc --version | grep gcc | awk '{print $3}' | tr -d "."`

if [ "$comp" -ge "340" ]; then
	gcc=`gcc --version | grep gcc | awk '{print $3}' | awk -F. '{for(i=1; i<=2; i++){print $i}}'`
	comp=`echo $gcc | tr -d " "`
fi

compdef=gcc$comp
cmtvers="v1r20p20070208"
if [ `uname` = Darwin ]; then
	cmtvers="v1r20p20070524"
fi
newcomp=""
debug=""

# get CMT and/or gcc versions from arguments if any  

for args 
do
	if [ "$args" = "HEAD" ]; then
		cmtvers="HEAD"
	else
		if [ "$args" = "debug" ] ; then
			debug=$args
		else
			v=`echo $args | grep 'v1r'`
			if [ "$v" != "" ] ; then
				cmtvers=$v
			else
				newcomp=$args
			fi
		fi
	fi
done


CMTSITE="CERN"; export CMTSITE
AFSROOT="/afs"; export AFSROOT
SITEROOT="/afs/cern.ch"; export SITEROOT

# get .rootauthrc file if not yet there
if [ ! -e $HOME/.rootauthrc ] ; then
	cp /afs/cern.ch/lhcb/scripts/.rootauthrc $HOME/.
fi

origin="$HOME"
work="cmtuser"

# remove .cmtrc file if it exists
if [ -f ${HOME}/.cmtrc ] ; then
	/bin/rm  ${HOME}/.cmtrc
fi

# User_release_area
if [ ! -d $origin/${work} ] ; then
	mkdir -p $origin/${work}
fi
User_release_area="${origin}/${work}"; export User_release_area
if [ -d ${User_release_area}/cmt ] ; then
	/bin/rm -fr ${User_release_area}/cmt
fi

# Gaudi release area and cvs repository
GAUDISOFT="$SITEROOT/sw/Gaudi/releases"; export GAUDISOFT
GAUDIDEV="$SITEROOT/sw/Gaudi/dev"; export GAUDIDEV
GAUDIKSERVER=":kserver:isscvs.cern.ch:/local/reps/Gaudi"; export GAUDIKSERVER
Gaudi_release_area="$GAUDISOFT"; export Gaudi_release_area

# LCG release area
LCG_release_area="/afs/cern.ch/sw/lcg/app/releases"; export LCG_release_area

# DIM release area
DIM_release_area="/afs/cern.ch/lhcb/online/control"; export DIM_release_area

# LHCb release area and cvs repository
LHCBKSERVER=":kserver:isscvs@cern.ch:/local/reps/lhcb"; export LHCBKSERVER
LHCBRELEASES="$SITEROOT/lhcb/software/releases"; export LHCBRELEASES
LHCBDEV="$SITEROOT/lhcb/software/DEV"; export LHCBDEV
LHCBDOC="$LHCBRELEASES/DOC"; export LHCBDOC

# LHCb project path
LHCBPROJECTPATH="$LHCBRELEASES:$GAUDISOFT:$LCG_release_area"; export LHCBPROJECTPATH

#================================================================================================
# set CMTPATH or CMTPROJECTPATH
# if CMTPROJECTPATH is set beforehand keep it ( test)
if [ "${CMTPROJECTPATH}" != "" ]; then
	unset CMTPATH
else
	cvers=`echo $cmtvers | grep v1r20`
	if  [ "W${cvers}" = "W" ]; then
		CMTPATH="${User_release_area}"; export CMTPATH  
		unset CMTPROJECTPATH
	else
		unset CMTPATH
		CMTPROJECTPATH="${User_release_area}:${LHCb_release_area}:${Gaudi_release_area}:${LCG_release_area}"; export CMTPROJECTPATH
	fi
fi

#==============================================================================================
# new emacs environment
EMACSDIR="$LHCBRELEASES/TOOLS/Tools/Emacs/pro"; export EMACSDIR

# LHCb styles configuration
LHCBSTYLE="$LHCBRELEASES/TOOLS/Tools/Styles/pro"; export LHCBSTYLE

# clear PATH and LD_LIBRARY_PATH
if [ "x$SAVEPATH" != "x" ]; then
	PATH="$SAVEPATH"; export PATH
fi
unset LD_LIBRARY_PATH
unset COMPILER_PATH


# configure CMT =================================================
. $SITEROOT/sw/contrib/CMT/$cmtvers/mgr/setup.sh

# deal with different linux distributions ======================
if [ -e /etc/redhat-release ] ; then
	distrib=`cat /etc/redhat-release | awk '{print $1}'`
	rhv=`cat /etc/redhat-release | tr -d '[a-z][A-Z]'`
	if [ "$distrib" = "Scientific" ] ; then
		nativehw=`uname -i`
		hw="$nativehw"
		if [ "$hw" = "i386" ] ; then
			hw="ia32"
		else
			if [ "$hw" = "x86_64" ] ; then
				hw="amd64"
			fi
		fi
		rhv=`echo ${rhv} | awk -F "." '{print $1}'`
		rh="slc"${rhv}"_"${hw}
	else
		rhv=`echo ${rhv} | tr -d "."` 
		rh="rh$rhv"
	fi
fi

# deal with OS type ===========================================
if [ `uname -s` = "Darwin" ] ; then
	rh=`sw_vers | grep ProductVersion | awk '{print $2}' | awk -F . '{print $1 $2}'`
	rh="osx$rh"
	comp=`gcc --version | grep gcc | awk '{print $3}' | tr -d "."`
	comp="gcc$comp"
	if [ `uname -p` = 'powerpc' ]; then
		hw='ppc'
	else
		hw='ia32'
	fi
	rh="${rh}_${hw}"
	CMTOPT=${rh}_${comp} ; export CMTOPT
elif [ "$OSTYPE" = "linux-gnu" -o "$OSTYPE" = "linux" ] ; then
	# get the compiler from the arguments
	if [ "$newcomp" = "" ] ; then
		comp=$compdef
	else
		comp=$newcomp
	fi

  	#========if gcc323 is requested on SLC4 compile on SLC4 pretending it is SLC3
	if [ "$comp" = "gcc323" -a "$rhv" != "3" ] ; then
		if [ "$nativehw" = "x86_64" -o "$nativehw" = "i386" ] ; then
    		COMPILER_PATH="/afs/cern.ch/lhcb/externallib/SLC3COMPAT/slc3_ia32_gcc323" ; export COMPILER_PATH
    		CMTOPT="slc3_ia32_${comp}" ; export CMTOPT
    		rh="slc3_ia32"
    		if  [ ! -d ${COMPILER_PATH} ] ; then
      			echo "$comp compiler is not available on this node"
      			return 
    		fi
    		PATH=${COMPILER_PATH}/bin:$PATH; export PATH
    		LD_LIBRARY_PATH="${COMPILER_PATH}/lib" ; export LD_LIBRARY_PATH
		fi
	fi
  #============================================
	binary=${rh}_${comp}
	if [ "$hw" = "ia64" ] ; then
		binary=`echo $binary | sed -e 's/ia64/amd64/'`
	fi
	CMTOPT=$binary ; export CMTOPT
fi

CMTCONFIG="${CMTOPT}"; export CMTCONFIG
CMTDEB="${CMTCONFIG}_dbg"; export CMTDEB
if [ "$debug" = "debug" ] ; then
	CMTCONFIG="${CMTDEB}" ; export CMTCONFIG
fi
set -


if [ !  ${LD_LIBRARY_PATH} ];then
	LD_LIBRARY_PATH="" ; export LD_LIBRARY_PATH
fi
if [ ! ${ROOTSYS} ]; then
	ROOTSYS=""; export ROOTSYS
fi

echo "******************************************************"
echo "*           WELCOME to the $comp on $rh system       *"
echo "******************************************************"
echo " --- "\$CMTROOT " is set to $CMTROOT "
echo " --- "\$CMTCONFIG " is set to $CMTCONFIG "
if [ "$debug" != "debug" ] ; then
	echo " --- to compile and link in debug mode : export CMTCONFIG="\$CMTDEB "; gmake"
fi
if [ "$CMTPATH" != "" ]; then
	echo " --- "\$CMTPATH " is set to ${origin}/${work}"
else
	echo " --- "\$User_release_area " is set to ${origin}/${work}"
	echo " --- "\$CMTPROJECTPATH " is set to "\$User_release_area":"\$LHCb_release_area":"\$Gaudi_release_area":"\$LCG_release_area
	echo " --- projects will be searched in "\$CMTPROJECTPATH
fi 
	echo " -------------------------------------------------------------------- "

#==========================================================================

unset newcomp rh rhv comp cmtvers

#set -
