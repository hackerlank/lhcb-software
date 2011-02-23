# $Id: $
# Test your line(s) of the stripping

from Gaudi.Configuration import *
from Configurables import SelDSTWriter, DaVinci
from StrippingConf.Configuration import StrippingConf

# Now build the stream
from StrippingConf.StrippingStream import StrippingStream
stream = StrippingStream("Test")

# Import your stripping lines
from StrippingSelections import StrippingBd2DstarMuNu
confBd2DstarMuNu = StrippingBd2DstarMuNu.Bd2DstarMuNuAllLinesConf("Bd2DstarMuNu", StrippingBd2DstarMuNu.confdict)
stream.appendLines( confBd2DstarMuNu.lines() )

# Import your stripping lines
from StrippingSelections import StrippingB0q2DplusMuX 
confStrippingB0q2DplusMuX  = StrippingB0q2DplusMuX.B0q2DplusMuXAllLinesConf("B0q2DplusMuX",StrippingB0q2DplusMuX.confdict)
stream.appendLines( confStrippingB0q2DplusMuX.lines() )

# Import your stripping lines
from StrippingSelections import StrippingBd2DstarTauNu
confBd2DstarTauNu = StrippingBd2DstarTauNu.Bd2DstarTauNuAllLinesConf("Bd2DstarTauNu", StrippingBd2DstarTauNu.confdict)
stream.appendLines( confBd2DstarTauNu.lines() )

# Import your stripping lines
from StrippingSelections import StrippingB2XuMuNu
confB2XuMuNu = StrippingB2XuMuNu.B2XuMuNuBuilder("B2XuMuNu", StrippingB2XuMuNu.confdict)
stream.appendLines( confB2XuMuNu.lines() )

# Import your stripping lines
from StrippingSelections import StrippingB2DMuNuX
confB2DMuNuX = StrippingB2DMuNuX.B2DMuNuXAllLinesConf("B2DMuNuX", StrippingB2DMuNuX.confdict)
stream.appendLines( confB2DMuNuX.lines() )



from Configurables import  ProcStatusCheck
filterBadEvents =  ProcStatusCheck()

# Configure the stripping using the same options as in Reco06-Stripping10
sc = StrippingConf( Streams = [ stream ],
                    MaxCandidates = 2000,
                    AcceptBadEvents = False,
                    BadEventSelection = filterBadEvents )
sc.OutputType = "ETC"                    # Can be either "ETC" or "DST"

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

# Configure the ETC writing step
from Configurables import EventTuple, TupleToolSelResults
from Configurables import TupleToolStripping

tag = EventTuple("TagCreator")
tag.EvtColsProduce = True
tag.ToolList = [ "TupleToolEventInfo", "TupleToolRecoStats", "TupleToolStripping"  ]
tag.addTool( TupleToolStripping )
tag.TupleToolStripping.StrippigReportsLocations = "/Event/Strip/Phys/DecReports"

# Remove the microbias and beam gas etc events before doing the tagging step
regexp = "HLT_PASS_RE('Hlt1(?!ODIN)(?!L0)(?!Lumi)(?!Tell1)(?!MB)(?!NZS)(?!Velo)(?!BeamGas)(?!Incident).*Decision')"
from Configurables import LoKi__HDRFilter
filterHLT = LoKi__HDRFilter("FilterHLT",Code = regexp )

seq = GaudiSequencer("TagSeq")
seq.Members = [tag]

MessageSvc().Format = "% F%60W%S%7W%R%T %0W%M"

from Configurables import AuditorSvc, ChronoAuditor
AuditorSvc().Auditors.append( ChronoAuditor("Chrono") )

from Configurables import StrippingReport
sr = StrippingReport(Selections = sc.selections())

from Configurables import AlgorithmCorrelationsAlg
ac = AlgorithmCorrelationsAlg(Algorithms = sc.selections())

from Configurables import CondDB
CondDB().IgnoreHeartBeat = True

from Configurables import FilterDesktop

MakePionsEtc = FilterDesktop('MakePionsEtc')
MakePionsEtc.InputLocations=["Phys/StdLooseKaons","Phys/StdLoosePions","Phys/StdLooseMuons","Phys/StdLooseDstarWithD02KPi"]
MakePionsEtc.Code="ALL"

DaVinci().PrintFreq = 1000
DaVinci().HistogramFile = 'DV_stripping_histos.root'
DaVinci().ETCFile = "etc.root"
DaVinci().EvtMax = 100000
DaVinci().EventPreFilters = [ filterHLT ]
DaVinci().appendToMainSequence( [MakePionsEtc] )
DaVinci().appendToMainSequence( [ sc.sequence() ] )
DaVinci().appendToMainSequence( [ sr ] )
DaVinci().appendToMainSequence( [ ac ] )
DaVinci().MoniSequence += [ seq ]            # Append the TagCreator to DaVinci
DaVinci().DataType = "2010"
DaVinci().InputType = 'SDST'
importOptions("$STRIPPINGSELECTIONSROOT/tests/data/RUN_81430_RealData+Reco08-Stripping12_90000000_SDST.py")
