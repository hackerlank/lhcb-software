""" General LHCb platform configuration """

import sys, platform, os
import re
# CMTCONFIG extraction

def isNewStyleBinary(cmtconfig):
    """ check if the CMTCONFIG value is new styled """
    newstyle = False
    if len(cmtconfig.split("-")) > 1 :
        newstyle = True
    return newstyle

def isBinaryDbg(cmtconfig):
    """ check if the CMTCONFIG value is a debug one """
    bindbg = True
    if isNewStyleBinary(cmtconfig) :
        if not cmtconfig.endswith("-dbg") :
            bindbg = False
    else :
        if not cmtconfig.endswith("_dbg") :
            bindbg = False
    return bindbg

def isBinaryOpt(cmtconfig):
    """ check if the CMTCONFIG value is an optimized one """
    binopt = True
    if isBinaryDbg(cmtconfig) :
        binopt = False
    return binopt


def getBinaryDbg(cmtconfig):
    """ convert CMTCONFIG to debug """
    cmtdbg = cmtconfig
    if not isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            if cmtconfig.endswith("-opt") :
                cmtdbg = "-".join(cmtconfig.split("-")[:-1]) + "-dbg"
            else :
                cmtdbg += "-dbg"
        else :
            cmtdbg += "_dbg"
    return cmtdbg

def getBinaryOpt(cmtconfig):
    """ convert CMTCONFIG to optimized """
    cmtopt = cmtconfig
    if isBinaryDbg(cmtconfig) :
        if isNewStyleBinary(cmtconfig) :
            cmtopt = "-".join(cmtconfig.split("-")[:-1]) + "-opt"
        else :
            cmtopt = "_".join(cmtconfig.split("_")[:-1])
    return cmtopt

def getCompiler(cmtconfig):
    """ extract compiler from CMTCONFIG """
    compdef = None
    if isNewStyleBinary(cmtconfig) :
        compdef = cmtconfig.split("-")[2]
    else :
        if not cmtconfig.startswith("win") :
            compdef = cmtconfig.split("_")[2]
        else :
            compdef = cmtconfig.split("_")[1]
    return compdef

def getPlatformType(cmtconfig):
    """ extract platform type (slc5, slc4, etc) from CMTCONFIG """
    platformtype = None
    if isNewStyleBinary(cmtconfig) :
        platformtype = cmtconfig.split("-")[1]
    else :
        platformtype = cmtconfig.split("_")[0]
    if platformtype == "sl5" :
        platformtype = "slc5"
    if platformtype == "sl4" :
        platformtype = "slc4"
    if platformtype == "sl3" :
        platformtype = "slc3"
    return platformtype


def getArchitecture(cmtconfig):
    """ extract architecture from CMTCONFIG """
    architecture = None
    if isNewStyleBinary(cmtconfig) :
        architecture = cmtconfig.split("-")[0]
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
    else :
        archlist = cmtconfig.split("_")
        if not archlist[0].startswith("win") :
            architecture = archlist[1]
            if architecture == "i686" :
                architecture = "ia32"
            if architecture == "x86_64" :
                architecture = "i686"
    return architecture

def getConfig(architecture, platformtype, compiler, debug=False):
    cmtconfig = None
    if platformtype == "slc5" :
        if architecture == "ia32" :
            architecture = "i686"
        if architecture == "amd64" :
            architecture = "x86_64"
        cmtconfig = "-".join([architecture, platformtype, compiler, "opt"])
    elif not platformtype.startswith("win") :
        if architecture in [ "i686", "i586", "i486", "i386"] :
            architecture = "ia32"
        if architecture == "x86_64" :
            architecture = "amd64"
        cmtconfig = "_".join([platformtype, architecture, compiler])
    else :
        cmtconfig = "_".join([platformtype, compiler])
    if debug :
        cmtconfig = getBinaryDbg(cmtconfig)
    return cmtconfig

# officially supported binaries
binary_opt_list = ["slc3_ia32_gcc323",
                   "slc4_ia32_gcc34", "slc4_amd64_gcc34",
                   "x86_64-slc5-gcc43-opt",
                   "win32_vc71"]
# future possible supported binaries
extra_binary_opt_list = ["x86_64-slc5-gcc34-opt", "i686-slc5-gcc34-opt",
                         "i686-slc5-gcc43-opt",
                         "i686-winxp-vc90-opt", "x86_64-winxp-vc90-opt",
                         "osx105_ia32_gcc41", "x86_64-osx106-gcc42-opt"]

binary_dbg_list = [ getBinaryDbg(x) for x in binary_opt_list ]
extra_binary_dbg_list = [ getBinaryDbg(x) for x in extra_binary_opt_list ]

binary_list = binary_opt_list + binary_dbg_list
extra_binary_list = extra_binary_opt_list + extra_binary_dbg_list


# supported shells
supported_shells = ["csh", "sh", "bat"]


# Native platform guessing


linux_release_files = [ "/etc/redhat-release" ,
                        "/etc/system-release",
                        "/etc/SuSE-release",
                        "/etc/issue", "/etc/issue.net" ]

linux_flavour_aliases = {
                         "slc"  : ["Scientific Linux"],
                         "rhel" : ["Redhat Enterprise", "Red Hat Enterprise"],
                         "rh"   : ["Redhat", "Red Hat"],
                         "fc"   : ["Fedora", "Fedora Core"],
                         "suse" : ["SuSE"],
                         "co"   : ["CentOS"],
                         "deb"  : ["Debian"],
                         "ub"   : ["Ubuntu"],
                         "ml"   : ["Mandriva Linux"]
                        }

flavor_runtime_compatibility = {
                                "slc5"  : ["slc5", "slc4"],
                                "slc4"  : ["slc4", "slc3"],
                                "slc3"  : ["slc3"],
                                "rh73"  : ["rh73"],
                                "win32" : ["win32"],
                                "osx105": ["osx105"],
                                "osx106": ["osx105", "osx106"]
                                }

arch_runtime_compatiblity = {
                                "x86_64" : ["x86_64", "ia64", "i686", "i586", "i486", "i386"],
                                "ia64" :   ["x86_64", "ia64", "i686", "i586", "i486", "i386"],
                                "i686" :   ["i686", "i586", "i486", "i386"],
                                "i586" :   ["i586", "i486", "i386"],
                                "i486" :   ["i486", "i386"],
                                "i386" :   ["i386"]
                                }

flavor_runtime_equivalence = {
                              "slc5"  : ["slc5", "co5", "rhel5", "ub9", "fc12", "fc11", "fc10"],
                              "slc4"  : ["slc4", "co4", "rhel4", "deb4"],
                              "slc3"  : ["slc3", "suse90", "suse100"],
                              "rh73"  : ["rh73", "suse80", "suse81", "suse82", "suse83"],
                              "win32" : ["win32"],
                              "osx105": ["osx105"],
                              "osx106": ["osx106"]
                             }

supported_compilers = {
                       "slc5"   : ["gcc43"],
                       "slc4"   : ["gcc34"],
                       "slc3"   : ["gcc323"],
                       "win32"  : ["vc71"],
                       "osx105" : ["gcc41"],
                       "osx106" : ["gcc42"]
                       }
class NativeMachine:
    def __init__(self):
        self._arch = None
        self._ostype = None
        self._machine = None
        self._osflavor = None
        self._osversion = None
        self._sysinfo = platform.uname()
        if sys.platform == "win32" :
            self._arch = "32"
            self._ostype = "Windows"
            self._machine = "i686"
        elif sys.platform == "win64" :
            self._arch = "64"
            self._ostype = "Windows"
            self._machine = "x86_64"
        else :
            self._ostype = self._sysinfo[0]
            self._machine = self._sysinfo[4]
            if self._ostype in ["Linux", "LynxOS", "Darwin"] :
                if self._machine == "x86_64" or self._machine == "ia64" :
                    self._arch = "64"
                else :
                    self._arch = "32"
    def sysInfo(self):
        """ full platform.uname() list """
        return self._sysinfo
    def arch(self):
        """ returns 32 or 64 """
        return self._arch
    def OSType(self):
        """ returns Linux, Darwin, Windows """
        return self._ostype
    def machine(self):
        """ returns i386, i486, i686, x86_64, ia64, power mac """
        return self._machine
    def system(self):
        """ return Linux-i386, Windows-x86_64 ... """
        return "%s-%s" % (self._ostype, self._machine)
    # OS extraction
    def OSFlavour(self, teststring=None):
        if not self._osflavor or teststring:
            if self._ostype == "Windows" :
                self._osflavor = self._sysinfo[2]
                self._osversion = self._sysinfo[3]
            elif self._ostype == "SunOS" :
                self._osflavor = "sun"
                self._osversion = "4.x"
            elif self._ostype == "Darwin" :
                verList = [ int(v) for v in self._sysinfo[2].split('.') ]
                if len(verList) > 2:
                    osMajRelease = verList[0] - 4
                    osMinRelease = verList[1]
                    if osMajRelease == 3:
                        self._osflavor = 'Panther'
                    elif osMajRelease == 4:
                        self._osflavor = 'Tiger'
                    elif osMajRelease == 5:
                        self._osflavor = 'Leopard'
                    elif osMajRelease == 6:
                        self._osflavor = 'Snow Leopard'
                    self._osversion = "10.%d.%d" % (osMajRelease, osMinRelease)

            elif self._ostype == "Linux" :
                for f in linux_release_files :
                    if os.path.exists(f) :
                        cont = " ".join(open(f).readlines())
                        break
                if teststring :
                    cont = teststring
                found = False
                for f in linux_flavour_aliases :
                    if not found :
                        for s in linux_flavour_aliases[f]:
                            if not found :
                                if cont.upper().find(s.upper()) != -1 :
                                    self._osflavor = linux_flavour_aliases[f][0]
                                    found = True
                                    break
                            else :
                                break
                    else :
                        break
                vmatch = re.compile("\ +(\d+(?:\.\d+)*)")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        return self._osflavor

    def OSVersion(self, position=None, teststring=None):
        if not self._osversion :
            if self._ostype == "Windows" :
                self._osversion = self._sysinfo[3]
            elif self._ostype == "SunOS" :
                self._osversion = "4.x"
            elif self._ostype == "Darwin" :
                verList = [ int(v) for v in self._sysinfo[2].split(".") ]
                if len(verList) > 2:
                    osMajRelease = verList[0] - 4
                    osMinRelease = verList[1]
                    self._osversion = "10.%d.%d" % (osMajRelease, osMinRelease)
            elif self._ostype == "Linux" :
                for f in linux_release_files :
                    if os.path.exists(f) :
                        cont = " ".join(open(f).readlines())
                        break
                if teststring :
                    cont = teststring
                vmatch = re.compile("\ +(\d+(?:\.\d+)*)")
                m = vmatch.search(cont)
                if m :
                    self._osversion = m.group(1)

        osver = self._osversion

        # returns at most the number of position specified. 
        if position :
            osver = ".".join(self._osversion.split(".")[:position])

        return osver
    # CMT derived informations
    def CMTArchitecture(self):
        """ returns the CMT architecture """
        arch = "ia32"
        if re.compile('i\d86').match(self.machine()) :
            arch = "ia32"
        elif re.compile('x86_64').match(self.machine()) :
            arch = "amd64"
        elif re.compile('ia64').match(self.machine()) :
            arch = "ia64"
        elif re.compile('power mac', re.I).match(self.machine()) :
            arch = "ppc"
        elif self.OSType() == "Windows" :
            arch = sys.platform
        return arch

    def CMTSystem(self):
        """ returns the CMTBIn variable used by CMT itself """
        cmtsystem = None
        if self.OSType() == "Windows" :
            cmtsystem = "VisualC"
        elif self.OSType() == "Darwin" :
            cmtsystem = "Darwin-i386"
        else :
            if self.machine() in arch_runtime_compatiblity["i586"] :
                cmtsystem = "%s-i386" % self.OSType()
            else :
                cmtsystem = "%s-%s" % (self.OSType(), self.machine())
        return cmtsystem

    def CMTOSFlavour(self):
        """ returns the CMT short name for the OS flavour and version """
        cmtflavour = None
        if self.OSType() == "Windows" :
            cmtflavour = "win%s" % self.arch()
        elif self.OSType() == "SunOS" :
            cmtflavour = self.OSFlavour() + self.OSVersion(position=2).replace(".", "")
        elif self.OSType() == "Darwin" :
            cmtflavour = "osx%s" % self.OSVersion(position=2).replace(".", "")
        elif self.OSType() == "Linux" :
            for f in linux_flavour_aliases :
                if self.OSFlavour() == linux_flavour_aliases[f][0] :
                    cmtflavour = f + self.OSVersion(position=1)
                    if self.OSFlavour() == "SuSE" or self.OSFlavour() == "Redhat" :
                        cmtflavour = f + self.OSVersion(position=2)
                    if self.OSFlavour() == "SuSE" and int(self.OSVersion(position=1)) > 10 :
                        cmtflavour = f + self.OSVersion(position=1)
        return cmtflavour

    def CMTOSEquivalentFlavour(self):
        """ returns the CMT short name for the OS compatible flavour and version """
        cmtflavour = None
        for f in flavor_runtime_equivalence :
            if self.CMTOSFlavour() in flavor_runtime_equivalence[f] :
                cmtflavour = f
                break
        return cmtflavour

    def CMTCompatibleConfig(self, debug=False):
        """ return the list of compatible CMT configs """
        compatibles = []
        equiv = self.CMTOSEquivalentFlavour()
        machine = self.machine()
        if equiv in flavor_runtime_compatibility :
            for f in flavor_runtime_compatibility[equiv] :
                for m in arch_runtime_compatiblity[machine] :
                    for c in supported_compilers[f] :
                        n = getConfig(m, f, c, debug=False)
                        if n not in compatibles :
                            compatibles.append(n)
                        if debug :
                            n = getConfig(m, f, c, debug=True)
                            if n not in compatibles :
                                compatibles.append(n)

        return compatibles

    def CMTSupportedConfig(self, debug=False):
        """ returns the list of supported CMT configs among the compatible ones"""
        compatibles = self.CMTCompatibleConfig(debug)
        supported = []
        for c in compatibles :
            if c in binary_list and c not in supported:
                supported.append(c)
        return supported
    def CMTNativeConfig(self, debug=False):
        """ 
        Returns the native configuration if possible. Guess also the compiler
        on linux platforms 
        """
        natconf = None
        return natconf

