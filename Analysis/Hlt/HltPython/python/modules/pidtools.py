# =============================================================================
""" @namespace pidtools.py
@brief Translation between pid as integer and as string, and some shortcut pid functions
@author Diego Martinez diego.martinez.santos@cern.ch
@date 2007-11-28
"""
# =============================================================================

#---------------------------------------------------
ParticleTranslator={
    'phi':333,333:'phi',
    'e-':11,11:'e-',
    'e+':-11,-11: 'e+',
    'neutrino e':12,12:'neutrino e',
    'antineutrino e':-12,-12:'antineutrino e',
    'mu-':13,13:'mu-',
    'mu+':-13,-13:'mu+',
    'neutrino mu':14,14:'neutrino mu',
    'antineutrino mu':-14,-14:'antineutrino mu',
    'tau-':15,15:'tau-',
    'tau+':-15,-15:'tau+',
    'neutrino tau':16,16:'neutrino tau',
    'antineutrino tau':-16,-16:'antineutrino tau',
    'g':21,21:'g',
    'photon':22,22:'photon',
    'Z':23,23:'Z',
    'W+':24,24:'W+',
    'W-':-24,-24:'W-',
    'pi0':111,111:'pi0',
    'anti pi0':-111,-111:'anti pi 0',
    'pi+':211,211:'pi+',
    'pi-':-211,-211:'pi-',
    'rho0(770)':113,113:'rho0(770)',
    'anti rho0(770)':-113,-113:'anti rho0(770)',
    'rho+(770)':213,213:'rho+(770)',
    'rho-(770)':-213,-213:'rho-(770)',
    'eta':221,221:'eta',
    'anti eta':-221,-221:'anti eta',
    'eta"(958)':331,331:'eta"(958)',
    'ant eta"(958)':331,331:'anti eta"(958)',
    'w(782)':223,223:'w(782)',
    'anti w(782)':-223,-223:'anti w(782)',
    'phi(1020)':333,333:'phi(1020)',
    'anti phi(1020)':-333,-333:'anti phi(1020)',
    'K0l':130,130:'k0l',
    'anti K0l':-130,-130:'anti K0l',
    'K0s':310,310:'K0s',
    'anti K0s':-310,-310:'anti K0s',
    'K0':311,311:'K0',
    'anti K0':-311,-311:'anti K0',
    'K+':321,321:'K+',
    'K-':-321,-321:'K-',
    'K*0(892)':313,313:'K*0(892)',
    'antiK*0(892)':-313,-313:'antiK*0(892)',
    'K*+(892)':323,323:'K*+(892)',
    'K*-(892)':-323,-323:'K*-(892)',
    'D+':411,411:'D+',
    'D-':-411,-411:'D-',
    'D0':421,421:'D0',
    'anti D0':-421,-421:'anti D0',
    'Ds+':431,431:'Ds+',
    'Ds-':-431,-431:'Ds-',
    'B0':511,511:'B0',
    'anti B0':-511,-511:'anti B0',
    'B+':521,521:'B+',
    'B-':-521,-521:'B-',
    'B0s':531,531:'B0s',
    'anti B0s':-531,-531:'anti B0s',
    'eta c(1S)':441,441:'eta c(1S)',
    'J/Psi(1S)':443,443:'J/Psi(1S)',
    'anti eta c(1S)':-441,-441:'anti eta c(1S)',
    'anti J/Psi(1S)':-443,-443:'anti J/Psi(1S)',
    'eta b(1S)':551,551:'eta b(1S)',
    'anti eta b(1S)':-551,-551:'anti eta b(1S)',
    'Ypsilon(1S)':553,553:'Ypsilon(1S)',
    'anti Ypsilon(1S)':-553,-553:'anti Ypsilon(1S)',
    'p':2212,2212:'p',
    'anti p':-2212,-2212:'anti p',
    'n':2112,2112:'n',
    'anti n':-2112,-2112:'anti n',
    'Delta++':2224,2224:'Delta++',
    'Delta--':-2224,-2224:'Delta--',
    'Delta+':2214,2214:'Delta+',
    'Delta-':1114,1114:'Delta-',
    'anti Delta+':-2214,-2214:'anti Delta+',
    'anti Delta-':-1114,-1114:'anti Delta-',
    'lambda':3122,3122:'lambda',
    'anti lambda': -3122,-3122:'anti lambda',
    'sygma+':3222,3222:'sygma+',
    'anti sygma+':-3222,-3222:'anti sygma+',
    'sygma0':3212,3212:'sygma0',
    'anti sygma0':-3212,-3212:'anti sygma0',
    'sygma-':3112,3112:'sygma-',
    'anti sygma-':-3112,-3112:'anti sygma-',
    'OMG-':3334,3334:'OMG-',
    'OMG+':-3334,-3334:'OMG+',
    '=0':3322,3322:'=0',
    'anti =0':-3322,-3322:'anti =0',
    '=-':3312,3312:'=-',
    '=+':-3312,-3312:'=+',
    'BC+':541,541:'BC+',
    'BC-':-541,-541:'BC-',
    'K1(1270)0':10313,10313:'K1(1270)0',
    'anti K1(1270)0':-10313,-10313:'anti K1(1270)0',
    'K1(1270)+':10323,10323:'K1(1270)+',
    'K1(1270)-':-10323,-10323:'K1(1270)-',
    'sygma*-':3114,3114:'sygma*-',
    'anti sygma*-':-3114,-3114:'anti sygma*-',
    'sygma*+':3224,3224:'sygma*+',
    'anti sygma*+':-3224,-3224:'anti sygma*+',
    '=+ c':4232,4232:'=+ c',
    '=- c':-4232,-4232:'=- c',
    'b1(1235)0':10113,10113:'b1(1235)0',
    'anti b1(1235)0':-10113,-10113:'anti b1(1235)0',
    'sygma*0':3214,3214:'sigma*0',
    'anti sygma*0':-3214,-3214:'anti sygma*0',
    'D*(2007)0':423,423:'D*(2007)0',
    'anti D*(2007)0':-423,-423:'anti D*(2007)0',
    'Linker Error':0.5,0.5:'Linker Error'
    
    
    }

#---------------------------------------------------
def PTranslate(n):
    if (ParticleTranslator.has_key(n) == False): return str(n)
    return ParticleTranslator[n]
 
#---------------------------------------------------
def pid (a) :
    """ Returns pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return a.particleID().pid()

#---------------------------------------------------
def absPid (a) :
    """ Returns absolute value of pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return abs(pid(a))

#---------------------------------------------------
def pidName (a) :
    """ Returns pid of a particle or mcparticle.
    Author: Hugo Ruiz, hugo.ruiz@cern.ch"""
    return  PTranslate(a.particleID().pid())

