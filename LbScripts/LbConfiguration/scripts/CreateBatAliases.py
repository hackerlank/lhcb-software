#!/usr/bin/env python
from LbUtils.Script import Script
from LbUtils.CVS import CVS2Version
from LbConfiguration.Project import project_list
import sys, os
import logging

__version__ = CVS2Version("$Name: not supported by cvs2svn $", "$Revision: 1.4 $")

class CreateBatAliasesScript(Script):
    _version = __version__
    def defineOpts(self):
        parser = self.parser
        parser.set_defaults(outputdir=None)
        parser.add_option("-o", "--output-dir", 
                          action="store", 
                          metavar="DIR",
                          type = "string",
                          dest = "outputdir",
                          help="output directory for the generated alias scripts")

    def main(self):
        log = logging.getLogger()
        opts = self.options
        
        if not opts.outputdir:
            log.error("No output directory defined -- exiting")
            return 1
        for p in project_list :
            alist = p.Aliases()
            for a in alist.keys():
                aliascmd = alist[a].split()
                cmd = aliascmd[0]
                args = " ".join(aliascmd[1:])
                filename = os.path.join(opts.outputdir, "%s.bat" % a)
                log.info("Writing %s : call %%~d0%%~p0\%s.bat %s" % (filename, cmd , args))
                f = open(filename, "w")
                f.write(" call %%~d0%%~p0\%s.bat %s %%1 %%2 %%3 %%4 %%5 %%6\n\n" % ( cmd , args))
                f.close()
        return 0

if __name__ == '__main__':
    sys.exit(CreateBatAliasesScript(usage="%prog [options]").run())