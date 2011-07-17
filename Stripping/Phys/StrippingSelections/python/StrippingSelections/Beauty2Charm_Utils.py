#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

from Gaudi.Configuration import *
from GaudiConfUtils.ConfigurableGenerators import FilterDesktop
from GaudiConfUtils.ConfigurableGenerators import CombineParticles
from PhysSelPython.Wrappers import Selection, MergedSelection
from Beauty2Charm_LoKiCuts import LoKiCuts

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def filterSelection(name,code,inputs):
    print 'filterSelection:',name,code
    return Selection(name+'Beauty2CharmFilter',
                     Algorithm=FilterDesktop(Code=code),
                     RequiredSelections=inputs)

def filterInputs(tag,inputs,config):
    '''Filter input particles.'''
    code = LoKiCuts(['TRCHI2DOF','PT','P','MIPCHI2DV'],config).code()
    return filterSelection(tag+'Inputs',code,inputs)

def topoInputsCuts(): # Don't need IP chi2 cut b/c is in 1st filter
    return "(TRCHI2DOF<3) & (PT > 500*MeV) & (P > 5000*MeV)"

def topoInputs(tag,inputs):
    '''Selects tracks that could have been used by the Topo.'''
    return filterSelection(tag+'TopoInputs',topoInputsCuts(),inputs)

def hasTopoChild(): return "AHASCHILD(ISBASIC & " + topoInputsCuts() + ")"

def bMassCut(decay,window):
    '''Returns ADAMASS functor given decay desc. and mass window.'''
    dec = decay[0]
    where = dec.find('B')
    if where < 0: print 'ERROR! No B in descriptor "%s"' % dec
    b = dec[where:where+2]
    if b is 'B0': # Also include B_s for neutral B's!
        b_s = b[0]+'_s'+b[-1]
        return "((ADAMASS('%s') < %s) | (ADAMASS('%s') < %s))" \
               % (b,window,b_s,window)
    else: return "(ADAMASS('%s') < %s)" % (b,window)

def makeTISTOSFilter(name,tistos):
    specs = {'Hlt2Topo.*Decision%'+tistos:0}
    from Configurables import TisTosParticleTagger
    tisTosFilter = TisTosParticleTagger(name+'TISTOSFilter')
    tisTosFilter.TisTosSpecs = specs
    tisTosFilter.ProjectTracksToCalo = False
    tisTosFilter.CaloClustForCharged = False
    tisTosFilter.CaloClustForNeutral = False
    tisTosFilter.TOSFrac = {4:0.0, 5:0.0}
    return tisTosFilter

def tisTosSelection(sel, tistos):
    '''Filters Selection sel to be tistos (TOS or TIS).'''
    tisTosFilter = makeTISTOSFilter(sel.name()+tistos+'Filter',tistos)
    return Selection(sel.name()+tistos+'Sel', Algorithm=tisTosFilter,
                     RequiredSelections=[sel])

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#

def makeB2X(name,decay,inputs,config):
    '''Makes all B -> X selections.'''
    comboCuts = LoKiCuts.combine([bMassCut(decay,config['MASS_WINDOW']),
                                  LoKiCuts(['AMAXDOCA'],config).code()])
    momCuts = LoKiCuts(['SUMPT','VCHI2DOF','BPVIPCHI2','BPVLTIME','BPVDIRA'],
                       config).code()    
    b2x = CombineParticles(DecayDescriptors=decay,CombinationCut=comboCuts,
                           MotherCut=momCuts)
    print 'makeB2X',name,'combo:',comboCuts,'mom:',momCuts
    return Selection(name,Algorithm=b2x,RequiredSelections=inputs)

def makeB2XMerged(name,decays,xtag,inputs,config):
    '''Merges B->X selections.  Returns the TIS and TOS merged selections.'''
    tosSels = []
    tisSels = []
    for tag, decay in decays.iteritems():
        sname = tag+xtag+'Beauty2Charm'
        sel = makeB2X(sname+'Sel',decay,inputs[tag],config)
        tosSels.append(tisTosSelection(sel,'TOS'))
        tisSels.append(tisTosSelection(sel,'TIS'))
    sname = name+xtag+'Beauty2Charm'
    tos = MergedSelection(sname+'TOS',RequiredSelections=tosSels)
    tis = MergedSelection(sname+'TIS',RequiredSelections=tisSels)
    return {'TOS':tos,'TIS':tis}

#\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\/\#
