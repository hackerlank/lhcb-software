from GaudiKernel.ProcessJobOptions import importOptions


from Gaudi.Configuration import *

from Configurables import Boole, LHCbApp,CondDB#, GaudiSequencer
from Configurables import MCFTDepositCreator, MCFTDigitCreator,FTClusterCreator
#from Configurables import MCFTDepositMonitor, MCFTDigitMonitor, FTClusterMonitor
from Configurables import FTRawBankEncoder,  FTRawBankDecoder
from Configurables import L0Conf, DDDBConf
L0Conf().TCK = '0x002A'

#CondDB().Upgrade    = True
DDDBConf().DbRoot   = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/test_DDDBupgrade_FT_v20/lhcb.xml"
#DDDBConf().DbRoot   = "/afs/cern.ch/user/c/cogneras/scratch0/DDDBUpgrade-v20/lhcb.xml"
LHCbApp().DDDBtag   = "dddb-20130408"
LHCbApp().CondDBtag = "simcond-20121001-vc-md100" 


#-- Event input
#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/test_DDDBupgrade_FT_v20/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/test_DDDB_FT_v20/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/c/cogneras/public/forFT/DDDB/lhcb.xml"
#DDDBConf().DbRoot = "/afs/cern.ch/user/s/szczypka/public/upgrade/forFT/DDDB/lhcb.xml"
#LHCbApp().DDDBtag   = "dddb-20130408"
#LHCbApp().CondDBtag = "simcond-20121001-vc-md100"



#Datasetname="Bs_mumu_v6_nu325"
#Datasetname="Bs_mumu_v6_nu65"

#Datasetname="MiniBias_v6_nu325"
#Datasetname="MiniBias_v6_nu65"
Datasetname="MiniBias_v20_nu25"

#EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/"+Datasetname+".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
#EventSelector().Input = ["DATAFILE='PFN:root://castorlhcb.cern.ch//castor/cern.ch/user/o/ogruenbe/ganga/22/"+Datasetname+".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
EventSelector().Input = ["DATAFILE='/afs/cern.ch/user/o/ogruenbe/public/FT_upgrade/gauss/output/"+Datasetname+".sim?svcClass=default' TYP='POOL_ROOTTREE' OPT='READ'"]
Boole().DatasetName = Datasetname+"_EC"
#Boole().OutputLevel = DEBUG
#Boole().DetectorDigi = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[]}
Boole().DetectorDigi = ['FT']

#Boole().DetectorDigi = {'VELO':['Velo'], 'MUON':[''], 'RICH':[''], 'TT':['TT'], 'IT':['IT'], 'L0':[''], 'CALO':[''], 'OT':['OT']}

Boole().DigiSequence = ['FT']
#Boole().LinkSequence = ['Velo', 'TT', 'Tr' ,'FT' ]
#Boole().DetectorMoni = {'VELO':[], 'MUON':[], 'RICH':[], 'TT':[], 'IT':[], 'L0':[], 'CALO':[], 'OT':[],'MC':[]}
#Boole().DatasetName = "FTTest"
#Boole().LinkSequence = ['FT' ]

Boole().Outputs = []


myAlgDeposit = MCFTDepositCreator()
#myAlgDeposit.ReflexionCoefficient = 0.8
#myAlgDeposit.ShortAttenuationLength = 1260
#myAlgDeposit.OutputLevel = DEBUG


myAlgDigit = MCFTDigitCreator()
#myAlgDigit.ChannelNoiseEnabled = True
##myAlgDigit.SiPMGain = 4
#myAlgDigit.OutputLevel = DEBUG


myAlgCluster = FTClusterCreator()
#myAlgCluster.OutputLevel = DEBUG
#myAlgCluster.ADCThreshold = 1
#myAlgCluster.ClusterMaxWidth = 8
#myAlgCluster.ClusterMinWidth = 1
#myAlgCluster.ClusterMinCharge = 8
#myAlgCluster.ClusterMinADCPeak = 5


#myAlgEncoder = FTRawBankEncoder()
#myAlgEncoder.OutputLevel = DEBUG

#myAlgDecoder = FTRawBankDecoder()
#myAlgDecoder.OutputLevel = DEBUG




GaudiSequencer("DigiFTSeq").Members = [myAlgDeposit,
                                       myAlgDigit,
                                      myAlgCluster]
                                       #myAlgEncoder,
                                       #myAlgDecoder]

#GaudiSequencer("LinkTrSeq").Members = ["BuildMCTrackInfoUpgrade"]

#OutputStream("DigiWriter").ItemList = ["/Event/Gen/Header#1", "/Event/MC/Header#1", "/Event/MC/DigiHeader#1", "/Event/DAQ/RawEvent#1", "/Event/pSim/MCVertices#1", "/Event/pSim/MCParticles#1", "/Event/Link/Raw/Velo/Clusters#1", "/Event/Link/Raw/TT/Clusters#1", "/Event/Link/Raw/FT/Clusters#1", "/Event/MC/TrackInfo#1" ]


#Boole().EvtMax = 1



#########################################################################################
####################  GaudiPython; Other Imports; Type Shortcuts  #######################
#########################################################################################
#import GaudiPython

#svc = gaudi.histosvc()


