#!/usr/bin/env python
# $Id$

from LbUtils.Script import SourceScript
from LbConfiguration.Project import project_list
from LbConfiguration.Version import ParseSvnVersion

import sys

__version__ = ParseSvnVersion("$Id$", "$URL$")


class AllProjectsSetupScript(SourceScript):
    _version = __version__
    def getEnv(self):
        ev = self.Environment()
        al = self.Aliases()
        for p in project_list :
            key = "%s_release_area" % p.Name()
            value = p.ReleaseArea()
            if key and value :
                ev[key] = value
        for p in project_list :
            alist = p.Aliases()
            for a in alist.keys():
                al[a] = alist[a]


    def main(self):
        self.getEnv()
        self.flush()
        return 0

if __name__ == '__main__':
    sys.exit(AllProjectsSetupScript(usage="%prog [options]").run())

