#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import DataOnDemand, Selection, MergedSelection, VoidEventSelection
from Beauty2Charm_LoKiCuts import LoKiCuts
from Configurables import SubstitutePID
from Configurables import ConeJetProxyFilter

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

class ProtoLine(object):
    '''Stores selections, prescales, etc. that will become lines.'''
    def __init__(self,sels,pre=1.0):
        self.selections = sels
        self.pre = pre

    def prescale(self,line,name,config):
        if not name in config['Prescales'].keys(): return self.pre
        else: return config['Prescales'][name]

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def dataOnDemand(loc): return DataOnDemand(Location="Phys/%s/Particles"%loc)

def filterSelection(name,code,inputs,preambulo=None):
    alg = FilterDesktop(Code=code)
    if preambulo is not None : alg.Preambulo = preambulo
    return Selection(name+'Beauty2CharmFilter',
                     Algorithm=alg,
                     RequiredSelections=inputs)

def filterInputs(tag,inputs,config):
    '''Filter input particles.'''
    code = LoKiCuts(['TRCHI2DOF','PT','P','MIPCHI2DV','MM','CHILDCL1',
                     'CHILDCL2','PIDK','PIDp','BPVVDRHO','BPVVDZ','DOCAMAX','TRGHP'],
                    config).code()
    return filterSelection(tag+'Inputs',code,inputs)

def filterPi0s(which,inputs,config):
    cuts = ['PT','P']
    if which is 'Resolved':
        cuts.append('CHILDCL1')
        cuts.append('CHILDCL2')
    code = LoKiCuts.combine(["ABSID==111",LoKiCuts(cuts,config).code()])
    return filterSelection('Pi0'+which+'Inputs',code,inputs)

def topoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "HASTRACK & (TRCHI2DOF<2.5) & (PT > 500*MeV) & (P > 5000*MeV)"

def topoInputsLooseCuts():
    return "HASTRACK & (TRCHI2DOF<2.5) & (PT > 250*MeV) & (P > 2500*MeV)"


def topoKSInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(PT > 500*MeV) & (P > 5000*MeV) & (BPVVDCHI2 > 1000)"

def topoInputs(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'TopoInputs',topoInputsCuts(),inputs)

def topoInputsLoose(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'TopoInputsLoose',topoInputsLooseCuts(),inputs)


def hasTopoChild():
    return "AHASCHILD((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +"))"

def hasTopoChildren():
    return "NINTREE((ISBASIC & "+topoInputsCuts()+")|((ABSID=='KS0') & "\
           +topoKSInputsCuts() +")) > 1"

def has1TrackChild():
    return "INTREE(HASTRACK & (P>10000*MeV) & (PT>1700*MeV) & (TRCHI2DOF<2.5) "\
           "& (MIPCHI2DV(PRIMARY)>16) & (MIPDV(PRIMARY)>0.1*mm))"

def makeTOSFilter(name,specs):
    from Configurables import TisTosParticleTagger
    tosFilter = TisTosParticleTagger(name+'TOSFilter')
    tosFilter.TisTosSpecs = specs
    tosFilter.ProjectTracksToCalo = False
    tosFilter.CaloClustForCharged = False
    tosFilter.CaloClustForNeutral = False
    tosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tosFilter

def tosSelection(sel,specs):
    '''Filters Selection sel to be TOS.'''
    tosFilter = makeTOSFilter(sel.name(),specs)
    return Selection(sel.name()+'TOS', Algorithm=tosFilter,
                     RequiredSelections=[sel])
            
def makeTISTOSFilter(name):
    specs = {'Hlt2Topo.*Decision%TOS':0,
             'Hlt2Topo.*Decision%TIS':0,
             'Hlt2IncPhi.*Decision%TOS':0,'Hlt2IncPhi.*Decision%TIS':0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel):
    '''Filters Selection sel to be TOS OR TIS.'''
    tisTosFilter = makeTISTOSFilter(sel.name())
    tsel = Selection(sel.name()+'TISTOS', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])
    return tsel

def filterPID(name,input,config,level=1):
    cuts = ["(NINGENERATION(('p+'==ABSID) & (PIDp < %s),%d) == 0)" \
            % (config['P']['PIDp_MIN'],level),            
            "(NINGENERATION(('K+'==ABSID) & (PIDK < %s), %d) == 0)" \
            % (config['K']['PIDK_MIN'],level),
            "(NINGENERATION(('pi+'==ABSID) & (PIDK > %s), %d) == 0)" \
            % (config['PI']['PIDK_MAX'],level)]
    return filterSelection(name,LoKiCuts.combine(cuts),input)

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeB2X(name,decay,inputs,config,useIP=True,resVert=True):
    '''Makes all B -> X selections.'''
    from copy import deepcopy
    config = deepcopy(config)
    if name.find('B02') >= 0 or name.find('B2D0PiNoIP') >= 0:
        config['AM_MAX'] = '6000*MeV'
    if name.find('B02DPiD2HHHBeauty2Charm') >=0 or name.find('B02DKD2HHHBeauty2Charm') >=0 :
        config['AM_MAX'] = '7000*MeV'
    if name.find('B02DPiD2Pi0HHHResolvedBeauty2Charm') >=0 or name.find('B02DKD2Pi0HHHResolvedBeauty2Charm') >=0 :
        config['AM_MAX'] = '7000*MeV'
    if name.find('StrippingB02DsstarPiDsstar2DGammaD2HHHBeauty2CharmLine') >=0 or name.find('StrippingB02DsstarKDsstar2DGammaD2HHHBeauty2CharmLine') >=0 :
        config['AM_MAX'] = '7000*MeV'
    comboCuts = LoKiCuts(['SUMPT','AM'],config).code()
    flightCuts = ['BPVLTIME','DZ1','DZ2']
    if useIP: flightCuts += ['BPVIPCHI2','BPVDIRA']
    momCuts = [LoKiCuts(['VCHI2DOF', 'BPVCORRM'],config).code(),has1TrackChild(),
               hasTopoChildren(),LoKiCuts(flightCuts,config).code()]
    momCuts = LoKiCuts.combine(momCuts)
    b2x = CombineParticles(DecayDescriptors=decay,CombinationCut=comboCuts,
                           MotherCut=momCuts, 
                           StopAtMaxCombinations = True, 
                           MaxCombinations = 10000000)

    presels = []
    for i in inputs : 
        _code = "(CONTAINS('%s')<2000)" % i.outputLocation()
        presels += [ VoidEventSelection(name + "CombCut" + i.name(),
                                  Code=_code, 
                                  RequiredSelection = i
                                  ) ] 

    return Selection(name,Algorithm=b2x,RequiredSelections=inputs + presels )

def makeB2XSels(decays,xtag,inputs,config,useIP=True,resVert=True):
    '''Returns a list of the Hb->X selections.'''
    sels = []
    for tag, decay in decays.iteritems():
        sname = tag+xtag+'Beauty2Charm'
        sel = makeB2X(sname,decay,inputs[tag],config,useIP,resVert)
        sel = tisTosSelection(sel)
        sel = filterSelection(sname+'B2CBBDT',
                              "FILTER('BBDecTreeTool/B2CBBDT')",[sel])
        from Configurables import BBDecTreeTool as BBDT
        bbdt = BBDT(sel.name()+'.B2CBBDT')
        bbdt.Threshold = config['B2CBBDT_MIN']
        bbdt.ParamFile = 'Beauty2Charm_BDTParams_v1r0.txt'
        bbdt.ParticleDictTool='LoKi::Hybrid::DictOfFunctors/VarHandler'
        from Configurables import LoKi__Hybrid__DictOfFunctors as VarHandler
        varHandler = VarHandler( bbdt.name() + '.VarHandler' )
        heavyIDs = "( (5 == ((ABSID / 1000) % 10)) " \
                   + "| (4 == ((ABSID / 1000) % 10))" \
                   + "| (5 == ((ABSID / 100) % 10))" \
                   + "| (4 == ((ABSID / 100) % 10)) )"
        hvChi2DOFTot = "SUMTREE(VFASPF(VCHI2),%s,0.0) / SUMTREE(VFASPF(VDOF),%s,0.0)" % (heavyIDs, heavyIDs)
        varHandler.Variables = { "FDCHI2"     :  "BPVVDCHI2",
                                 "PT"         :  "PT",
                                 "HVCHI2DOFTOT" : hvChi2DOFTot }
        sels.append(sel)
        
    return sels
 
def makeBc2BX(name,decay,inputs,config):
    from copy import deepcopy
    config = deepcopy(config)
    comboCuts = LoKiCuts(['SUMPT','AM'],config).code()
    flightCuts = ['BPVLTIME','DZ1']
    flightCuts += ['BPVIPCHI2','BPVDIRA']
    momCuts = [LoKiCuts(['VCHI2DOF'],config).code(),
               "INTREE( HASTRACK & ISBASIC & (P>5000*MeV) & (PT > 500*MeV) & (MIPCHI2DV(PRIMARY) > 10) & (MIPDV(PRIMARY) > 0.06*mm))",
               LoKiCuts(flightCuts,config).code()]
    momCuts = LoKiCuts.combine(momCuts)
    bc2bx = CombineParticles(DecayDescriptors=decay,
                             CombinationCut=comboCuts,
                             MotherCut=momCuts)

    return Selection(name,Algorithm=bc2bx,RequiredSelections=inputs)    

def makeBc2BXSels(decays,xtag,inputs,config):
    sels = []
    for tag, decay in decays.iteritems():
        sname = tag+xtag+'Beauty2Charm'
        sel = makeBc2BX(sname,decay,inputs[tag],config)
        sels.append(sel)
    return sels


def makeB2DstarX(sel,uppions,config):
    # change B to something that doesn't fly
    sub = SubstitutePID('SubPID'+sel.name(),
                        Code="DECTREE('Beauty -> Charm ...')")
    sub.MaxChi2PerDoF = -666
    sub.Substitutions = {'Beauty -> Charm ...' : 'J/psi(1S)'}
    subsel = Selection(sel.name()+'DstarUPSel',Algorithm=sub,
                       RequiredSelections=[sel])
    filter = "INTREE(ID=='J/psi(1S)')"
    subsel = filterSelection(sel.name()+'DstarUPFilter',filter,[subsel])
    # Delta M cut on D* (preambulo is below)
    #dmcut = '((ISD1 & ((MD1PI-MD1) < 180)) | (ISD2 & ((MD2PI-MD2) < 180)))'
    #dmcut = '((MD1PI-MD1) < 180) | ((MD2PI-MD2) < 180)'
    dmcut = "((((IDD1==421)|(IDD1==411)) & ((MD1PI-MD1) < 180)) | (((IDD2==421)|(IDD2==411)) & ((MD2PI-MD2) < 180)))" 
    # add UP track
    combConfig = {'AMAXDOCA_MAX' : '0.5*mm'}
    comboCuts = [LoKiCuts(['AMAXDOCA'],combConfig).code(),dmcut] 
    comboCuts = LoKiCuts.combine(comboCuts)
    momCuts = LoKiCuts(['VCHI2DOF','BPVVDCHI2','BPVDIRA'],config).code()
    #did = "((ABSID=='D+')|(ABSID=='D0'))"
    preambulo = ['PXPI = ACHILD(PX,2)',
                 'PYPI = ACHILD(PY,2)',
                 'PZPI = ACHILD(PZ,2)',
                 'EPI  = ACHILD(E,2)',
                 'IDD1 = ACHILD(CHILD(ABSID,1),1)',
                 #'ISD1 = ACHILD(CHILD(%s,1),1)' % did,
                 'PXD1 = ACHILD(CHILD(PX,1),1)',
                 'PYD1 = ACHILD(CHILD(PY,1),1)',
                 'PZD1 = ACHILD(CHILD(PZ,1),1)',
                 'ED1  = ACHILD(CHILD(E,1),1)',
                 'MD1  = ACHILD(CHILD(M,1),1)',
                 'MD1PI = sqrt((EPI+ED1)**2 - (PXPI+PXD1)**2 - (PYPI+PYD1)**2 - (PZPI+PZD1)**2)',
                 #'ISD2 = ACHILD(CHILD(%s,2),1)' % did,
                 'IDD2 = ACHILD(CHILD(ABSID,2),1)',
                 'PXD2 = ACHILD(CHILD(PX,2),1)',
                 'PYD2 = ACHILD(CHILD(PY,2),1)',
                 'PZD2 = ACHILD(CHILD(PZ,2),1)',
                 'ED2  = ACHILD(CHILD(E,2),1)',
                 'MD2  = ACHILD(CHILD(M,2),1)',
                 'MD2PI = sqrt((EPI+ED2)**2 - (PXPI+PXD2)**2 - (PYPI+PYD2)**2 - (PZPI+PZD2)**2)',
                 ]                 
    cp = CombineParticles(CombinationCut=comboCuts,MotherCut=momCuts,
                          Preambulo=preambulo,
                          DecayDescriptor='[B+ -> J/psi(1S) pi+]cc')
    return Selection('DstarUP'+sel.name(),Algorithm=cp,
                     RequiredSelections=[uppions]+[subsel])
                                                 
#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
