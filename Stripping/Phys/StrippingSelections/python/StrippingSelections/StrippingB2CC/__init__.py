"""
Module importing stripping selection line builder modules
for B2CC WG.
"""

# last update: 7th May 2015
_selections = ['StrippingB2JpsiXforBeta_s',
               'StrippingB2Psi2SX',
               'StrippingB2Psi2SXMuMu',
               'StrippingB2CharmoniumX_6H',
               'StrippingBs2EtacPhiBDT',
               'StrippingBd2JpsieeKS',
               'StrippingBs2JpsieePhi',
               'StrippingB2JpsiPi0']

for _sel in _selections :
    try :
        __import__( '%s.%s'  % ( __name__, _sel ) )
    except Exception, x:
        print '[WARNING] Submodule %s.%s raises the excetpion "%s" and will be skipped !' % ( __name__,_sel,x )

from sys import modules as _modules
_this = _modules[__name__]

_strippingKeys = filter ( lambda x : x[:9]=='Stripping',
                          locals().keys())

_strippingModules = [getattr(_this, _k) for _k in _strippingKeys]

