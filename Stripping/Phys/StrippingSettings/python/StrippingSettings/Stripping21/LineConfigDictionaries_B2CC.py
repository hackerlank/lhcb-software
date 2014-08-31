################################################################################
##                          S T R I P P I N G  2 1                            ##
##                                                                            ##
##  Configuration for B2CC WG                                                 ##
##  Contact person: Carlos Vazquez Sierra (carlos.vazquez@cern.ch)            ##
################################################################################

from GaudiKernel.SystemOfUnits import *

######################################################################
## StrippingBetaSBu2JpsiKDetachedLine (FullDST)
## StrippingBetaSBd2JpsiKstarDetachedLine (FullDST)
## StrippingBetaSJpsi2MuMuLine (MicroDST)
## StrippingBetaSLambdab2JpsiLambdaUnbiasedLine (MicroDST)
## StrippingBetaSLambdab2JpsippiDetachedLine (MicroDST)
## StrippingBetaSBu2JpsiKPrescaledLine (MDST.DST)
## StrippingBetaSBs2JpsiPhiPrescaledLine (MDST.DST)
## StrippingBetaSBs2JpsiPhiDetachedLine (MDST.DST)
## StrippingBetaSBd2JpsiKstarPrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiKsPrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiKsDetachedLine (MDST.DST)
## StrippingBetaSBd2JpsiKsLDDetachedLine (MDST.DST)
## StrippingBetaSBs2JpsiKstarWideLine (MDST.DST)
## StrippingBetaSBd2JpsiPi0PrescaledLine (MDST.DST)
## StrippingBetaSBd2JpsiPi0DetachedLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2JpsiXforBeta_s.py
## Authors: Greig Cowan, Juan Palacios, Francesca Dordei, Carlos Vazquez Sierra
## -------------------------------------------------------------------
## Stripping reports (Revision 177153):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.3810|       381|       |  37.419|
## |_StrippingSequenceStreamTest_B2JpsiXforBeta_s_               |  0.3810|       381|       |  37.411|
## |!StrippingBetaSJpsi2MuMuLine                                 |  0.0480|        48|  1.021|   0.129|
## |!StrippingBetaSBd2JpsiKsLDDetachedLine                       |  0.0040|         4|  1.000|   9.100|
## |!StrippingBetaSBu2JpsiKDetachedLine                          |  0.0410|        41|  1.122|   3.001|
## |!StrippingBetaSBu2JpsiKPrescaledLine                         |  0.0240|        24|  1.625|   0.319|
## |!StrippingBetaSBs2JpsiPhiPrescaledLine                       |  0.0480|        48|  1.354|   3.952|
## |!StrippingBetaSBs2JpsiPhiDetachedLine                        |  0.0430|        43|  2.116|   2.496|
## |!StrippingBetaSBs2JpsiKstarWideLine                          |  0.0220|        22|  1.455|   0.771|
## |!StrippingBetaSBd2JpsiKstarPrescaledLine                     |  0.0510|        51|  1.961|   1.032|
## |!StrippingBetaSBd2JpsiKstarDetachedLine                      |  0.0520|        52|  1.692|   2.598|
## |!StrippingBetaSBd2JpsiKsPrescaledLine                        |  0.0120|        12|  1.000|   0.622|
## |!StrippingBetaSBd2JpsiKsDetachedLine                         |  0.0020|         2|  1.000|   0.131|
## |!StrippingBetaSLambdab2JpsippiDetachedLine                   |  0.0290|        29|  1.517|   0.321|
## |!StrippingBetaSLambdab2JpsiLambdaUnbiasedLine                |  0.0430|        43|  1.070|   0.173|
## |!StrippingBetaSBd2JpsiPi0PrescaledLine                       |  0.0490|        49|  1.184|   1.189|
## |!StrippingBetaSBd2JpsiPi0DetachedLine                        |  0.0420|        42|  1.190|   2.014|
##
######################################################################

BetaS = {
    'BUILDERTYPE' : 'B2JpsiXforBeta_sConf',
    'CONFIG'      : { 'TRCHI2DOF'            :       5
                    , 'BPVLTIME'             :       0.2
                    , 'JpsiMassWindow'       :       80
                    , 'DaughterPT'           :       1000
                    , 'VCHI2PDOF'            :       10
                    , 'Jpsi2MuMuPrescale'    :       0.075
                    , 'Bd2JpsiKstarPrescale' :       0.29
                    , 'Bd2JpsiKsPrescale'    :       1.0
                    , 'Bs2JpsiPhiPrescale'   :       0.62
                    , 'Bs2JpsiPi0Prescale'   :       0.9
                    , 'Bu2JpsiKPrescale'     :       0.2
                    },
    'STREAMS'     : { 'Leptonic' : ['StrippingBetaSJpsi2MuMuLine',
                                    'StrippingBetaSBu2JpsiKPrescaledLine',
                                    'StrippingBetaSBs2JpsiPhiPrescaledLine',
                                    'StrippingBetaSBs2JpsiPhiDetachedLine',
                                    'StrippingBetaSBd2JpsiKstarPrescaledLine',
                                    'StrippingBetaSBd2JpsiKsPrescaledLine',
                                    'StrippingBetaSBd2JpsiKsDetachedLine',
                                    'StrippingBetaSBd2JpsiKsLDDetachedLine',
                                    'StrippingBetaSBs2JpsiKstarWideLine',
                                    'StrippingBetaSLambdab2JpsiLambdaUnbiasedLine',
                                    'StrippingBetaSBd2JpsiPi0PrescaledLine',
                                    'StrippingBetaSBd2JpsiPi0DetachedLine',
                                    'StrippingBetaSLambdab2JpsippiDetachedLine'],
                      'Dimuon'  :  ['StrippingBetaSBu2JpsiKDetachedLine',
                                    'StrippingBetaSBd2JpsiKstarDetachedLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBs2EtacPhi_Etac2KKKKB2CharmoniumX_6HLine (MicroDST)
## StrippingBs2EtacPhi_Etac2KKPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingBs2EtacPhi_Etac2PiPiPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingBs2ChicPhi_Chic2KKKKB2CharmoniumX_6HLine (MicroDST)
## StrippingBs2ChicPhi_Chic2KKPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingBs2ChicPhi_Chic2PiPiPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingB2EtacKst_Etac2KKKKB2CharmoniumX_6HLine (MicroDST)
## StrippingB2EtacKst_Etac2KKPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingB2EtacKst_Etac2PiPiPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingB2ChicKst_Chic2KKKKB2CharmoniumX_6HLine (MicroDST)
## StrippingB2ChicKst_Chic2KKPiPiB2CharmoniumX_6HLine (MicroDST)
## StrippingB2ChicKst_Chic2PiPiPiPiB2CharmoniumX_6HLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2CharmoniumX_6H.py
## Authors: Adam Morris
## -------------------------------------------------------------------
## Stripping reports (Revision 176954):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.1250|       125|       |  22.655|
## |_StrippingSequenceStreamTest_B2CharmoniumX_6H_               |  0.1250|       125|       |  22.648|
## |!StrippingBs2EtacPhi_Etac2KKKKB2CharmoniumX_6HLine           |  0.0030|         3|  1.000|   8.623|
## |!StrippingBs2EtacPhi_Etac2KKPiPiB2CharmoniumX_6HLine         |  0.0090|         9|  2.222|   0.806|
## |!StrippingBs2EtacPhi_Etac2PiPiPiPiB2CharmoniumX_6HLine       |  0.0170|        17|  1.882|   0.376|
## |!StrippingBs2ChicPhi_Chic2KKKKB2CharmoniumX_6HLine           |  0.0020|         2|  1.500|   0.110|
## |!StrippingBs2ChicPhi_Chic2KKPiPiB2CharmoniumX_6HLine         |  0.0270|        27|  2.704|   0.395|
## |!StrippingBs2ChicPhi_Chic2PiPiPiPiB2CharmoniumX_6HLine       |  0.0370|        37|  2.324|   0.404|
## |!StrippingB2EtacKst_Etac2KKKKB2CharmoniumX_6HLine            |  0.0020|         2|  1.000|   1.952|
## |!StrippingB2EtacKst_Etac2KKPiPiB2CharmoniumX_6HLine          |  0.0150|        15|  1.667|   0.204|
## |!StrippingB2EtacKst_Etac2PiPiPiPiB2CharmoniumX_6HLine        |  0.0220|        22|  1.864|   0.228|
## |!StrippingB2ChicKst_Chic2KKKKB2CharmoniumX_6HLine            |  0.0010|         1|  2.000|   0.088|
## |!StrippingB2ChicKst_Chic2KKPiPiB2CharmoniumX_6HLine          |  0.0310|        31|  2.290|   0.214|
## |!StrippingB2ChicKst_Chic2PiPiPiPiB2CharmoniumX_6HLine        |  0.0350|        35|  2.171|   0.240|
##
######################################################################

B2CharmoniumX_6H = {
    'BUILDERTYPE' : 'StrippingB2CharmoniumX_6HConf',
    'CONFIG'      : { 'Prescale'             : 1.
                    , 'Postscale'            : 1.
                    , 'TRCHI2'               : 3.
                    , 'DOCA'                 : 0.1
                    # K parameters:
                    , 'K_PT'                 : 250.
                    , 'K_IPCHI2'             : 4.
                    # pi parameters:
                    , 'pi_PT'                : 250.
                    , 'pi_IPCHI2'            : 4.
                    # 'max' daughters:
                    , 'PT_maxetacdaughter'   : 1400.
                    , 'PT_maxchicdaughter'   : 1300.
                    , 'IPCHI2_maxdaughter'   : 9.
                    # phi parameters:
                    , 'phi_DIRA'             : 0.95
                    , 'phi_VDZ'              : 0.
                    , 'phi_PT'               : 1000.
                    , 'phi_IPCHI2'           : 6.
                    , 'phi_VCHI2'            : 7.
                    , 'phi_window'           : 30.
                    # Kst parameters:
                    , 'Kst_DIRA'             : 0.93
                    , 'Kst_VDZ'              : 0.
                    , 'Kst_PT'               : 1000.
                    , 'Kst_IPCHI2'           : 4.
                    , 'Kst_VCHI2'            : 12.
                    , 'Kst_window'           : 100.
                    # etac parameters:
                    , 'etac_DIRA'            : 0.9
                    , 'etac_VDZ'             : 0.
                    , 'etac_IPCHI2'          : 4.
                    , 'etac_VCHI2'           : 9.
                    , 'etac_sumPT'           : 4000.
                    , 'etac_window'          : 90.
                    # chic/etac parameters:
                    , 'chic_DIRA'            : 0.9
                    , 'chic_VDZ'             : 0.
                    , 'chic_IPCHI2'          : 4.
                    , 'chic_VCHI2'           : 9.
                    , 'chic_sumPT'           : 3600.
                    , 'chic_window'          : 60.
                    # Bs parameters:
                    , 'Bs_VDZ'               : 0.
                    , 'Bs_DIRA'              : 0.9
                    , 'Bs_VCHI2'             : 25.
                    , 'Bs_window'            : 500.
                    },
    'STREAMS'     : { 'Bhadron' : ['StrippingBs2EtacPhi_Etac2KKKKB2CharmoniumX_6HLine',
                                   'StrippingBs2EtacPhi_Etac2KKPiPiB2CharmoniumX_6HLine',
                                   'StrippingBs2EtacPhi_Etac2PiPiPiPiB2CharmoniumX_6HLine',
                                   'StrippingBs2ChicPhi_Chic2KKKKB2CharmoniumX_6HLine',
                                   'StrippingBs2ChicPhi_Chic2KKPiPiB2CharmoniumX_6HLine',
                                   'StrippingBs2ChicPhi_Chic2PiPiPiPiB2CharmoniumX_6HLine',
                                   'StrippingB2EtacKst_Etac2KKKKB2CharmoniumX_6HLine',
                                   'StrippingB2EtacKst_Etac2KKPiPiB2CharmoniumX_6HLine',
                                   'StrippingB2EtacKst_Etac2PiPiPiPiB2CharmoniumX_6HLine',
                                   'StrippingB2ChicKst_Chic2KKKKB2CharmoniumX_6HLine',
                                   'StrippingB2ChicKst_Chic2KKPiPiB2CharmoniumX_6HLine',
                                   'StrippingB2ChicKst_Chic2PiPiPiPiB2CharmoniumX_6HLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine (MicroDST)
## StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2Psi2SX.py
## Authors: Fred Blanc, Neus Lopez March (left LHCb!)
## -------------------------------------------------------------------
## Stripping reports (Revision 176850):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0300|        30|       |  18.212|
## |_StrippingSequenceStreamTest_B2Psi2SX_                       |  0.0300|        30|       |  18.206|
## |!StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine                 |  0.0060|         6|  1.000|   0.874|
## |!StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine                 |  0.0010|         1|  1.000|   7.671|
## |!StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine                   |  0.0210|        21|  2.000|   0.475|
## |!StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine               |  0.0080|         8|  1.625|   0.119|
## |!StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine                  |  0.0030|         3|  1.000|   0.068|
##
######################################################################

BetaSPsi2S = {
    'BUILDERTYPE' : 'B2Psi2SXConf',
    'CONFIG'      : { 'PionsTRCHI2DOF'       : 5
                    , 'Psi2SPiMIPCHI2DV'     : 9         # before 4
                    , 'Psi2SAM23down'        : 400
                    , 'Psi2SAM23up'          : 800
                    , 'Psi2SAPT'             : 500
                    , 'Psi2SADAMASS'         : 30
                    , 'Psi2SVFASPF'          : 16
                    , 'PIDpi'                : 10        # new 10
                    , 'ChKTRCHI2DOF'         : 5
                    , 'ChKPID'               : 0         # before -2
                    , 'PhiWin'               : 20
                    , 'PhiPT'                : 1000      # before 500, now 1000
                    , 'PhiVFASPF'            : 25        # new change it was 16 before
                    , 'PhiMAXTRCHI2DOF'      : 5
                    , 'PhiMINTRCHI2DOF'      : -2
                    , 'KstMassDown'          : 826
                    , 'KstMassUp'            : 966
                    , 'KstAPT'               : 1500      # before 500
                    , 'KstVFASPF'            : 16
                    , 'KstTRCHI2DOF'         : 4
                    , 'KstPIDK'              : 0         # before -2
                    , 'KstPIDpi'             : 10        # new 10
                    , 'KsVFASPF'             : 20
                    , 'KsBPVDLS'             : 5
                    , 'incl_LinePrescale'    : 0.1       # now 0.1, before 0.5 & 0.2
                    , 'incl_LinePostscale'   : 1
                    , 'BPVLTIME_detached'    : 0.15
                    , 'BsMassCutDownPre'     : 5000      # before 5000
                    , 'BsMassCutUpPre'       : 5650      # before 5650
                    , 'BsMassCutDownPost'    : 5150      # before 5100
                    , 'BsMassCutUpPost'      : 5550      # before 5550
                    , 'BsVCHI2PDOF'          : 20        # new change it was 10 before
                    , 'sig_LinePrescale'     : 1
                    , 'sig_LinePostscale'    : 1
                    , 'ChKPT'                : 500
                    , 'K_LinePrescale'       : 1         # 0.5
                    , 'K_LinePostscale'      : 1
                    , 'Kstar_LinePrescale'   : 1         # before 0.5
                    , 'Kstar_LinePostscale'  : 1
                    , 'MINTREEPT2'           : 1000
                    , 'BKsVCHI2PDOF'         : 10
                    , 'Ks_LinePrescale'      : 1
                    , 'Ks_LinePostscale'     : 1
                    },
    'STREAMS'     : { 'Leptonic': ['StrippingBetaSPsi2S_InclPsi2SToJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bs2Psi2SPhiJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bu2Psi2SKJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bd2Psi2SKstarJpsiPiPiLine',
                                   'StrippingBetaSPsi2S_Bd2Psi2SKsJpsiPiPiLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine (MicroDST)
## StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine (MicroDST)
## StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine (MicroDST)
## -------------------------------------------------------------------
## Lines defined in StrippingB2Psi2SXMuMu.py
## Authors: Neus Lopez March (left LHCb!)
## -------------------------------------------------------------------
## Stripping reports (Revision 176850):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.1100|       110|       |  18.789|
## |_StrippingSequenceStreamTest_B2Psi2SMuMu_                    |  0.1100|       110|       |  18.782|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine                 |  0.0020|         2|  1.000|   0.929|
## |!StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine         |  0.0610|        61|  1.000|   7.690|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine        |  0.0020|         2|  1.500|   0.076|
## |!StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine         |  0.0020|         2|  1.000|   0.072|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine           |  0.0350|        35|  1.286|   0.477|
## |!StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine          |  0.0170|        17|  2.118|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine       |  0.0100|        10|  1.900|   0.187|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine      |  0.0090|         9|  1.889|   0.054|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine          |  0.0070|         7|  1.000|   0.077|
## |!StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine         |  0.0150|        15|  1.133|   0.058|
##
######################################################################

BetaSPsi2SMuMu = {
    'BUILDERTYPE' : 'Bs2Psi2SPhiMuMuConf',
    'CONFIG'      : { 'muPID'                       : 0.
                    , 'Psi2SMassWin'                : 60.
                    , 'Psi2SADOCACHI2CUT'           : 30.
                    , 'Psi2SVFASPF'                 : 16
                    , 'ChKTRCHI2DOF'                : 5
                    , 'ChKPID'                      : 0          # before -2
                    , 'PhiWin'                      : 20
                    , 'PhiPT'                       : 1000       # before 500
                    , 'PhiVFASPF'                   : 25         # new change, it was 16 before
                    , 'PhiMAXTRCHI2DOF'             : 5
                    , 'PhiMINTRCHI2DOF'             : -2
                    , 'KstMassDown'                 : 826
                    , 'KstMassUp'                   : 966
                    , 'KstAPT'                      : 1200       # before 500
                    , 'KstVFASPF'                   : 16
                    , 'KstTRCHI2DOF'                : 4
                    , 'KstPIDK'                     : 0          # before -2
                    , 'KstPIDpi'                    : 10         # new
                    , 'KsVFASPF'                    : 20
                    , 'KsBPVDLS'                    : 5
                    , 'incl_LinePrescale'           : 0.1        # before 0.1
                    , 'incl_LinePostscale'          : 1
                    , 'incl_LineTisTosHlt1'         : { "Hlt1DiMuonHighMassDecision%TOS"      : 0 }
                    , 'incl_LineTisTosHlt2'         : { "Hlt2DiMuonPsi2SDecision%TOS"         : 0 }
                    , 'BPVLTIME_detached'           : 0.2        # before 0.15
                    , 'incl_DetachedLinePrescale'   : 1          # before 0.5
                    , 'incl_DetachedLinePostscale'  : 1
                    , 'incl_DetachedLineTisTosHlt1' : { "Hlt1DiMuonHighMassDecision%TOS"      : 0
                                                      , "Hlt1TrackMuonDecision%TOS"           : 0
                                                      , "Hlt1TrackAllL0Decision%TOS"          : 0 
                                                      }
                    , 'incl_DetachedLineTisTosHlt2' : { "Hlt2DiMuonDetachedHeavyDecision%TOS" : 0
                                                      , "Hlt2DiMuonDetachedPsi2SDecision%TOS" : 0
                                                      }
                    , 'incl_DetachedCut'            : "BPVDLS > 3"
                    , 'BsMassCutDownPre'            : 5000       # before 5000
                    , 'BsMassCutUpPre'              : 5650       # before 5650
                    , 'BsMassCutDownPost'           : 5150       # before 5100
                    , 'BsMassCutUpPost'             : 5550       # before 5550
                    , 'BsVCHI2PDOF'                 : 20         # new change, it was 10 before
                    , 'sig_PrescaledLinePrescale'   : 0.5        # before 1.0
                    , 'sig_PrescaledLinePostscale'  : 1
                    , 'sig_DetachedLinePrescale'    : 1
                    , 'sig_DetachedLinePostscale'   : 1
                    , 'BPVLTIME_unbiased'           : None
                    , 'MINTREEPT'                   : 1000       # before 1000
                    , 'MINTREEPT2'                  : 1500       # before 1000
                    , 'ChKPT'                       : 500
                    , 'K_PrescaledLinePrescale'     : 0.1        # before 0.3 & 0.2
                    , 'K_PrescaledLinePostscale'    : 1
                    , 'K_DetachedLinePrescale'      : 1          # before 0.1
                    , 'K_DetachedLinePostscale'     : 1
                    , 'KstarPT'                     : 2
                    , 'Kstar_PrescaledLinePrescale' : 0.07       # before 0.1
                    , 'Kstar_PrescaledLinePostscale': 1
                    , 'Kstar_DetachedLinePrescale'  : 1          # before 1
                    , 'Kstar_DetachedLinePostscale' : 1
                    , 'BKsVCHI2PDOF'                : 10
                    , 'Ks_PrescaledLinePrescale'    : 1
                    , 'Ks_PrescaledLinePostscale'   : 1
                    , 'Ks_DetachedLinePrescale'     : 1
                    , 'Ks_DetachedLinePostscale'    : 1
                    },
    'STREAMS'     : { 'Leptonic': ['StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuLine',
                                   'StrippingBetaSPsi2SMuMu_InclPsi2SToMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bs2Psi2SPhiMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bu2Psi2SKMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKstarMuMuPrescaledLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuDetachedLine',
                                   'StrippingBetaSPsi2SMuMu_Bd2Psi2SKsMuMuPrescaledLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBetaSBs2JpsieePhiDetachedLine (MDST.DST)
## StrippingBetaSBs2JpsieePhiFromTracksLine (MDST.DST)
## StrippingBetaSBs2JpsieePhiLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingBs2JpsieePhi.py
## Authors: Artur Ukleja, Jibo He, Konrad Klimaszewski
## Last changes made by Konrad Klimaszewski
## -------------------------------------------------------------------
## Stripping reports (Revision 176907):
##
## StrippingReport (2012)                                        INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0530|        53|       |  37.926|
## |_StrippingSequenceStreamTest_Bs2JpsieePhi_                   |  0.0530|        53|       |  37.919|
## |!StrippingBetaSBs2JpsieePhiDetachedLine                      |  0.0430|        43|  1.721|  23.977|
## |!StrippingBetaSBs2JpsieePhiFromTracksLine                    |  0.0290|        29|  1.897|   4.405|
## |!StrippingBetaSBs2JpsieePhiLine                              |  0.0180|        18|  2.000|   0.481|
##
######################################################################

BetaSBs2JpsieePhi = {
    'BUILDERTYPE' : 'Bs2JpsieePhiConf',
    'CONFIG'      : { 'ElectronPTLoose'            :   500.    # MeV
                    , 'ElectronPIDLoose'           :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOFLoose' :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOFLoose'    :    15.    # adimensional
                    , 'JpsiMassMinLoose'           :  2500.    # MeV
                    , 'JpsiMassMaxLoose'           :  3300.    # MeV
                    , 'KaonTrackCHI2pDOFLoose'     :     5.    # adimensional
                    , 'PhiPTLoose'                 :  1000.    # MeV
                    , 'PhiVertexCHI2pDOFLoose'     :    15.    # adimensional
                    , 'PhiMassMinLoose'            :   990.    # MeV
                    , 'PhiMassMaxLoose'            :  1050.    # MeV
                    , 'BsVertexCHI2pDOFLoose'      :    10.    # adimensional
                    , 'BsMassMinLoose'             :  4600.    # MeV
                    , 'BsMassMaxLoose'             :  6000.    # MeV
                    , 'LifetimeCut'                : " & (BPVLTIME()>0.3*ps)"
                    , 'PrescaleLoose'              :     1.    # adimensional
                    , 'ElectronPT'                 :   500.    # MeV
                    , 'ElectronPID'                :     0.    # adimensional
                    , 'ElectronTrackCHI2pDOF'      :     5.    # adimensional
                    , 'JpsiVertexCHI2pDOF'         :    15.    # adimensional
                    , 'JpsiMassMin'                :  2500.    # MeV
                    , 'JpsiMassMax'                :  3300.    # MeV
                    , 'KaonTrackCHI2pDOF'          :     5.    # adimensional
                    , 'KaonPID'                    :    -2.    # adimensional
                    , 'PhiPT'                      :  1000.    # MeV
                    , 'PhiVertexCHI2pDOF'          :    15.    # adimensional
                    , 'PhiMassMin'                 :   990.    # MeV
                    , 'PhiMassMax'                 :  1050.    # MeV
                    , 'BsVertexCHI2pDOF'           :    10.    # adimensional
                    , 'BsMassMin'                  :  4600.    # MeV
                    , 'BsMassMax'                  :  6000.    # MeV
                    , 'BsDIRA'                     :     0.99  # adimensional
                    , 'Prescale'                   :     0.1   # adimensional
                    },
    'STREAMS'     : { 'Radiative': ['StrippingBetaSBs2JpsieePhiDetachedLine',
                                    'StrippingBetaSBs2JpsieePhiFromTracksLine',
                                    'StrippingBetaSBs2JpsieePhiLine'] },
    'WGs'         : [ 'B2CC' ]
    }

######################################################################
## StrippingBs2EtacPhiBDTLine (MDST.DST)
## -------------------------------------------------------------------
## Lines defined in StrippingBs2EtacPhiBDT.py
## Authors: Morgan Martin, Jibo He
## -------------------------------------------------------------------
## Stripping reports (Revision 176954):
##
## StrippingReport (2012)                                         INFO Event 100000, Good event 100000
## |                                              *Decision name*|*Rate,%*|*Accepted*| *Mult*|*ms/evt*|
## |_StrippingGlobal_                                            |  0.0120|        12|       |  17.928|
## |_StrippingSequenceStreamTest_Bs2EtacPhi_BDT_                 |  0.0120|        12|       |  17.920|
## |!StrippingBs2EtacPhiBDTLine                                  |  0.0120|        12|  2.333|   8.622|
##
######################################################################

Bs2EtacPhiBDT = {
    'BUILDERTYPE' : 'Bs2EtacPhiBDTConf',
    'CONFIG'      : { 'HLTCuts'              : "(HLT_PASS_RE('Hlt2Topo.*Decision') | HLT_PASS_RE('Hlt2IncPhi.*Decision'))"
                    , 'KaonCuts'             : "(PROBNNk > 0.13) & (PT > 300*MeV) & (TRGHOSTPROB<0.4)"
                    , 'PionCuts'             : "(PROBNNpi > 0.2) & (PT > 250*MeV) & (TRGHOSTPROB<0.4)"
                    , 'EtacComAMCuts'        : "(AM<3.25*GeV)"
                    , 'EtacComN4Cuts'        : """
                                               (in_range(2.75*GeV, AM, 3.25*GeV))
                                               & ( (ACHILD(PT,1)+ACHILD(PT,2)+ACHILD(PT,3)+ACHILD(PT,4) ) > 2.5 *GeV)
                                               & ( (ACHILD(MIPCHI2DV(), 1) + ACHILD(MIPCHI2DV(), 2) + ACHILD(MIPCHI2DV(), 3) + ACHILD(MIPCHI2DV(), 4))>30)
                                               """
                    , 'EtacMomN4Cuts'        : "(VFASPF(VCHI2/VDOF) < 9.) & (in_range(2.8*GeV, MM, 3.2*GeV)) & (MIPCHI2DV(PRIMARY) > 2.)"
                    , 'PhiCuts'              : """
                                               (MAXTREE(ABSID=='K+',TRGHOSTPROB) < 0.4)
                                               & (MM<1.05*GeV)
                                               & (PT> 800*MeV)
                                               & (MIPCHI2DV(PRIMARY) > 2.)
                                               & (VFASPF(VCHI2) < 9.)
                                               """ 
                    , 'BsComCuts'              : "(ADAMASS('B_s0') < 500 *MeV)"
                    , 'BsMomCuts'              : "(VFASPF(VCHI2/VDOF) < 25.) & (BPVDIRA> 0.99) & (BPVIPCHI2()<25) & (BPVDLS>0)"
                    , 'Bs2EtacPhiMVACut'       :  "0."
                    , 'Bs2EtacPhiXmlFile'      :  "$TMVAWEIGHTSROOT/data/Bs2EtacPhi_BDT_v1r0.xml"
                    , 'Prescale'               : 1.
                    },
    'STREAMS'     : { 'Bhadron' : 'StrippingBs2EtacPhiBDTLine' },
    'WGs'         : [ 'B2CC' ]
    }

