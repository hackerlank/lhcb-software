# Minimal file for running Moore from python prompt
# Syntax is:
#   gaudirun.py ../options/Moore2.py
#
from Gaudi.Configuration import *
from Moore.Configuration import *



# Get the event time (for CondDb) from ODIN 
from Configurables import EventClockSvc
EventClockSvc().EventTimeDecoder = 'OdinTimeDecoder'

# Profiling
AuditorSvc().Auditors  = [ 'NameAuditor', 'ChronoAuditor', 'MemoryAuditor' ]
NameAuditor().Enable   = False
ChronoAuditor().Enable = False
MemoryAuditor().Enable = False

# General output levels 
ToolSvc().OutputLevel                     = INFO
from Configurables import XmlParserSvc
XmlParserSvc().OutputLevel                = WARNING
MessageSvc().OutputLevel                  = INFO
ApplicationMgr().OutputLevel              = ERROR

# Print algorithm name with 40 characters
MessageSvc().Format = '% F%40W%S%7W%R%T %0W%M'

#/-----------------------------------------------------------------
#/ Rerun L0 and HLT - Alleys
#/-----------------------------------------------------------------


#/ Default options to rerun L0 and run Hlt 
importOptions('$HLTSYSROOT/options/L0.opts')
importOptions('$HLTSYSROOT/options/HltInit.opts')
importOptions('$HLTSYSROOT/options/Hlt1.opts')
importOptions('$HLTSYSROOT/options/Hlt2.opts')

# For DAQ studies replace the includes above for these ones:
# importOptions('$HLTSYSROOT/options/L0DAQ.opts')
# importOptions('$HLTSYSROOT/options/HltDAQ.opts')

# include this options to get time or histogram information of alleys
# importOptions('$HLTSYSROOT/options/HltAlleysTime.opts')
# importOptions('$HLTSYSROOT/options/HltAlleysHistos.opts')


#//---------------------------------------------------------------------------
#// Histogram output
#//---------------------------------------------------------------------------
ApplicationMgr().HistogramPersistency = 'ROOT'
HistogramPersistencySvc().OutputFile = 'Moore_minbias.root'

for i in [ 'ToolSvc' , 'AuditorSvc' ] : ApplicationMgr().ExtSvc.append( i ) 
ApplicationMgr().AuditAlgorithms = 1
AuditorSvc().Auditors.append( 'TimingAuditor/TIMER' )


#//---------------------------------------------------------------------------
#// Number of events to process, optionally skipping some events
#//---------------------------------------------------------------------------
ApplicationMgr().EvtMax =  10000 ;

Moore().applyConf()

files= [ '/data/bfys/lhcb/MinBias-L0strip/MBL0-lumi2-' + str(f) +'.dst'  for f in range(1,5) ]


#files = [ '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-1.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol1/dijkstra/Selections/MBL0-lumi2-2.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-3.dst',
#          '/afs/cern.ch/lhcb/group/trigger/vol3/dijkstra/Selections/MBL0-lumi2-4.dst' ]

EventSelector().Input =[ "DATAFILE='PFN:"+ f +"' TYP='POOL_ROOTTREE' OPT='READ'" for f in files ]
EventSelector().PrintFreq = 100;
