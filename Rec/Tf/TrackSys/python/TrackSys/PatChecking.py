from Gaudi.Configuration import GaudiSequencer
from TrackSys.Configuration import *
from GaudiKernel.SystemOfUnits import mm

from Configurables import ( TrackAssociator, TrackResChecker, TrackOccupChecker,
                            TrackEffChecker, TrackSelector, MCReconstructible,
                            MCParticleSelector, PrimaryVertexChecker, PatLHCbID2MCParticle,
                            UnpackMCParticle, UnpackMCVertex, DebugTrackingLosses )
                            
def PatChecking():
   
   GaudiSequencer("CheckPatSeq").Members += [ PatLHCbID2MCParticle("PatLHCbID2MCParticle")]

## Define the associators and track efficiency checkers according to the Pattern algorithms

   trackAlgs = TrackSys().getProp("TrackPatRecAlgorithms")

   if "Truth" in trackAlgs :
      GaudiSequencer("RecoTruthSeq").Members +=  [ UnpackMCParticle(), UnpackMCVertex(), PatLHCbID2MCParticle() ]
      
   if not TrackSys().veloOpen() and "FastVelo" not in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocVeloRZ") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("VeloRZ") ]
      TrackAssociator("AssocVeloRZ").TracksInContainer     = "Rec/Track/RZVelo";

   if "Velo" in trackAlgs or "FastVelo" in trackAlgs:
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocVelo") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("Velo") ]
      TrackAssociator("AssocVelo").TracksInContainer       = "Rec/Track/Velo";
      
   if "VeloTT" in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocVeloTT") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("VeloTT") ]
      TrackAssociator("AssocVeloTT").TracksInContainer     = "Rec/Track/VeloTT";

   if "Forward" in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocForward") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("Forward") ]
      TrackAssociator("AssocForward").TracksInContainer    = "Rec/Track/Forward";
      
   if "TsaSeed" in trackAlgs or "PatSeed" in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocTTrack") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("TTrack") ]
      TrackAssociator("AssocTTrack").TracksInContainer     = "Rec/Track/Seed";

   if "Match" in trackAlgs or "PatMatch" in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocMatch") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("Match") ]
      TrackAssociator("AssocMatch").TracksInContainer      = "Rec/Track/Match";
       
   if "Downstream" in trackAlgs :
      GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocDownstream") ]
      GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("Downstream") ]
      TrackAssociator("AssocDownstream").TracksInContainer = "Rec/Track/Downstream";
   
   GaudiSequencer("CheckPatSeq").Members += [ TrackAssociator("AssocBest") ]
   GaudiSequencer("CheckPatSeq").Members += [ TrackEffChecker("BestTracks") ]   
   TrackAssociator("AssocBest").TracksInContainer       = "Rec/Track/Best";

   GaudiSequencer("CheckPatSeq").Members += [ DebugTrackingLosses() ]
   
   GaudiSequencer("CheckPatSeq").Members  += [ TrackResChecker("TrackResChecker")];
   TrackResChecker("TrackResChecker").SplitByType = True;
   ConfigureEffCheckTools(TrackResChecker("TrackResChecker"))
     
   TrackEffChecker("BestTracks").TracksInContainer = "Rec/Track/Best";
   TrackEffChecker("BestTracks").SelectionCriteria = "ChargedLong";
   TrackEffChecker("BestTracks").RequireLongTrack = True;
   ConfigureEffCheckTools(TrackEffChecker("BestTracks"))
   
   TrackEffChecker("VeloRZ").TracksInContainer = "Rec/Track/RZVelo";
   TrackEffChecker("VeloRZ").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("VeloRZ"))
   
   
   TrackEffChecker("Velo").TracksInContainer = "Rec/Track/Velo";
   TrackEffChecker("Velo").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("Velo"))
   
   TrackEffChecker("VeloTT").TracksInContainer = "Rec/Track/VeloTT";
   TrackEffChecker("VeloTT").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("VeloTT"))
   
   TrackEffChecker("Forward").TracksInContainer = "Rec/Track/Forward";
   TrackEffChecker("Forward").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("Forward"))
   
   TrackEffChecker("TTrack").TracksInContainer = "Rec/Track/Seed";
   TrackEffChecker("TTrack").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("TTrack"))
   
   TrackEffChecker("Match").TracksInContainer = "Rec/Track/Match";
   TrackEffChecker("Match").SelectionCriteria = "ChargedLong";
   ConfigureEffCheckTools(TrackEffChecker("Match"))
   
   TrackEffChecker("Downstream").TracksInContainer = "Rec/Track/Downstream";
   TrackEffChecker("Downstream").SelectionCriteria = "ChargedDownstream";
   ConfigureEffCheckTools(TrackEffChecker("Downstream"))
   
   GaudiSequencer("CheckPatSeq").Members  += [TrackOccupChecker("OccupancyCheck")]

   #don't do this for DC06 because there's an FPE.
   if TrackSys().getProp('DataType')!="DC06":
     GaudiSequencer("CheckPatSeq").Members  += [PrimaryVertexChecker("PVOfflineCheck")]

def ConfigureEffCheckTools(EffCheck):
   EffCheck.addTool(MCReconstructible, name="Selector")
   EffCheck.Selector.addTool(MCParticleSelector, name="Selector")
   EffCheck.Selector.Selector.rejectElectrons = True
   EffCheck.Selector.Selector.rejectInteractions = True
   EffCheck.Selector.Selector.zInteraction = 9400.
   EffCheck.HistoPrint = False
   EffCheck.StatPrint = False
   EffCheck.FullDetail = False

