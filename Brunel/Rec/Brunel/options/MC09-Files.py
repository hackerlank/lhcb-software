# Example 2009 MC files for Brunel.
# This file must be given as last argument to gaudirun.py, after Brunel<conf>.py

# Syntax is:
#   gaudirun.py Brunel<conf>.py MC09-Files.py
#
from Gaudi.Configuration import *
from Configurables import Brunel, LHCbApp

#-- File catalogs. First one is read-write
FileCatalog().Catalogs = [ "xmlcatalog_file:MyCatalog.xml",
                           "xmlcatalog_file:$BRUNELROOT/job/NewCatalog.xml" ]

#-- Event input
inputType   = Brunel().getProp("InputType").upper()

if inputType == "MDF":
    import sys
    print "**************************************"
    print "Do not use MC09-Files.py for real data"
    print "**************************************"
    sys.exit()

elif inputType == "DST":
    # Example POOL DST (reprocessing)
    datasetName =  "00001820_00000001"
    # No MC09 DST file yet, format is as follows:
    # EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/grid/lhcb/production/DC06/phys-v2-lumi2/00001820/DST/0000/" + datasetName + "_5.dst?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
    import sys
    print "***************************************************"
    print " Please provide a DST input file in MC09-Files.py "
    print "***************************************************"
    sys.exit()

elif inputType == "ETC":
    LHCbApp().DDDBtag   = "MC09-20090602"
    LHCbApp().CondDBtag = "MC09-20100827-vc-md100"
    datasetName = "00012345_00012345_1"
    FileCatalog().Catalogs.append("xmlcatalog_file:$BRUNELROOT/job/" + datasetName + "_catalog.xml")
    EventSelector().Input = ["COLLECTION='TagCreator/EventTuple' DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/MC09-tests/" + datasetName + ".fetc?svcClass=default'  TYP='POOL_ROOT' SEL='(StrippingGlobal==1)'"] # Generated by Stuart from MC09 DST
    
else:
    # Example POOL DIGI (default, MC production)
    LHCbApp().DDDBtag   = "MC09-20090602"
    LHCbApp().CondDBtag = "MC09-20100827-vc-md100"

    # minimum bias events, MC09 default settings, Gauss v17r0 with Boole v18r0

    digiType = Brunel().getProp("DigiType").capitalize()
    if digiType == "Minimal":
        datasetName = '30000000-100ev-20090407-MC09-Minimal'
    else:
        datasetName = '30000000-100ev-20090407-MC09'

    EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/c/cattanem/Boole/v18r0/" + datasetName + ".digi?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]

#-- Add patch tags of the DB
#from Configurables import CondDB
#
#CondDB().LocalTags["SIMCOND"] = ["calo-20100613", "calo-20100215", "lumi-20100423", "rich-20100826"]

# Default output files names are set up using value Brunel().DatasetName property
Brunel().DatasetName = datasetName

# Redefine defaults by uncommenting one or more of options below 

# Monitoring histograms
#HistogramPersistencySvc().OutputFile = "SomeFile.root"

#-- Dst or rDst file
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.dst' TYP='POOL_ROOTTREE' OPT='REC'"
#OutputStream("DstWriter").Output = "DATAFILE='PFN:someFile.rdst' TYP='POOL_ROOTTREE' OPT='REC'"

#-- ETC output in case of ETC input
#TagCollectionSvc("EvtTupleSvc").Output = [ "EVTTAGS2 DATAFILE='some-etc.root' TYP='POOL_ROOTTREE' OPT='RECREATE' " ]
