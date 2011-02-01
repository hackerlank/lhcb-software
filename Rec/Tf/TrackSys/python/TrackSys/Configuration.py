
## @package TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008

__version__ = "$Id: Configuration.py,v 1.17 2009-12-10 19:11:14 smenzeme Exp $"
__author__  = "Marco Cattaneo <Marco.Cattaneo@cern.ch>"

from LHCbKernel.Configuration import *

## @class TrackSys
#  High level configuration for LHCb Tracking software
#  @author Marco Cattaneo <Marco.Cattaneo@cern.ch>
#  @date   15/08/2008
class TrackSys(LHCbConfigurableUser):

    # Steering options
    __slots__ = {
        "ExpertHistos": False # set to True to write out expert histos
       ,"SpecialData"  : []   # Various special data processing options.
       ,"ExpertTracking":  []  # list of expert Tracking options, see KnownExpertTracking
       ,"TrackPatRecAlgorithms": []  # List of track pattern recognition algorithms to run
       ,"TrackExtraInfoAlgorithms": []  # List of track 'extra info' algorithms to run
       ,"WithMC":       False # set to True to use MC truth
       ,"OutputType": "" # set to "RDST" for special RDST sequence
       ,"FilterBeforeFit": True  #Clone kill before fit of the Best container only. False = fit before clone killing
       ,"DataType": "2010" # propagated from Brunel(), used to determine which monitors to run
       ,"ForceNewSeq" : False  # use new tracking sequence independent of DataType
       ,"ForceOldSeq" : False  # use old tracking sequence independent of DataType 
        }
    
    ## Possible expert options
    KnownExpertTracking        = ["noDrifttimes", "simplifiedGeometry", "kalmanSmoother", "noMaterialCorrections",
                                  "fastSequence", "timing"]

    ## Default track pattern recognition algorithms to run in 2010
    DefaultPatRecAlgorithms_old    = ["Velo","Forward","TsaSeed","Match","Downstream","VeloTT"]
    ## Default track pattern recognition algorithms to run in 2011
    DefaultPatRecAlgorithms    = ["FastVelo","Forward","PatSeed","PatMatch","Downstream","VeloTT"]
    ## Default track 'extra info' algorithms to run
    DefaultExtraInfoAlgorithms = ["CloneFlagging","TrackLikelihood","GhostProbability"]
    ## Cosmic track pattern recognition algorithms to run
    CosmicPatRecAlgorithms    = ["PatSeed"]
    ## Cosmic expert swithces
    CosmicExpertTracking      = ["noDrifttimes"] 

    ## @brief Check the options are sane etc.
    def defineOptions(self):
        
      if self.getProp( "ForceNewSeq" ) and self.getProp( "ForceOldSeq" ) :
         raise RuntimeError("Cannot force both old and new tracking sequence at once")

      if self.getProp( "ForceNewSeq" ) :
         self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms)

      if self.getProp( "ForceOldSeq" ):
         self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms_old)
    
      if "cosmics" not in self.getProp("SpecialData"):
           if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
               if "MC09" == self.getProp("DataType") or "2008" == self.getProp("DataType") or "2009" == self.getProp("DataType") or "2010" == self.getProp("DataType") or "Upgrade" == self.getProb("DataType") :  
                 self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms_old)
               else:
                 self.setProp("TrackPatRecAlgorithms",self.DefaultPatRecAlgorithms)
           if len(self.getProp("TrackExtraInfoAlgorithms")) == 0 :
               self.setProp("TrackExtraInfoAlgorithms",self.DefaultExtraInfoAlgorithms)
           for prop in self.getProp("ExpertTracking"):
               if prop not in self.KnownExpertTracking:
                   raise RuntimeError("Unknown expertTracking option '%s'"%prop)
      else:         
           if len(self.getProp("TrackPatRecAlgorithms")) == 0 :
               self.setProp("TrackPatRecAlgorithms",self.CosmicPatRecAlgorithms)
           if len(self.getProp("ExpertTracking")) == 0 :
               self.setProp("ExpertTracking",self.CosmicExpertTracking)
                                                               

    ## @brief Shortcut to the fieldOff option
    def MC09(self)     : return "MC09" == self.getProp( "DataType" )
    ## @brief Shortcut to the fieldOff option
    def fieldOff(self) : return "fieldOff" in self.getProp("SpecialData")
    ## @brief Shortcut to the veloOpen option
    def veloOpen(self) : return "veloOpen" in self.getProp("SpecialData")
    ## @brief Shortcut to the cosmics option
    def cosmics(self)  : return "cosmics"  in self.getProp("SpecialData")
    ## @brief Shortcut to the beamGas option
    def beamGas(self)  : return "beamGas"  in self.getProp("SpecialData")
    ## @brief Shortcut to the noDrifttimes option
    def noDrifttimes(self) : return "noDrifttimes" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the simplifiedGeometry option
    def simplifiedGeometry(self) : return "simplifiedGeometry" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the kalmanSmoother option
    def kalmanSmoother(self) : return "kalmanSmoother" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the noMaterialCorrections option
    def noMaterialCorrections(self) : return "noMaterialCorrections" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the timing option
    def timing(self) : return "timing" in self.getProp("ExpertTracking")
    ## @brief Shortcut to the upgrade option
    def upgrade(self): return "upgrade" in self.getProp("SpecialData")

    

    ## @brief Option to enable setting the special data options easily
    def setSpecialDataOption(self,option,value):
        optSet = option in self.getProp("SpecialData")
        if   True  == value and False == optSet : self.getProp("SpecialData").append(option)
        elif False == value and True  == optSet : self.getProp("SpecialData").remove(option)

    ## @brief Apply the configuration
    def __apply_configuration__(self):
        self.defineOptions()
        if self.getProp( "FilterBeforeFit" ) :
            from TrackSys import RecoTracking
            RecoTracking.RecoTracking()
            if self.upgrade():
                from RecoUpgrade import RecoTrackingUpgrade
                RecoTrackingUpgrade.RecoTrackingUpgrade()
        else :
            from TrackSys import RecoTrackingOld
            RecoTrackingOld.RecoTracking()

        if self.getProp( "WithMC" ):
            from TrackSys import PatChecking
            PatChecking.PatChecking()
            if self.upgrade():
                from RecoUpgrade import PatCheckingUpgrade
                PatCheckingUpgrade.PatCheckingUpgrade()

