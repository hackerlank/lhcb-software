// Include files

// local
#include "LbGenXicc/GenXicc.h"

//-----------------------------------------------------------------------------
// Implementation file for class : GenXicc
//
// 2011-04-10 
//-----------------------------------------------------------------------------

Counter   GenXicc::s_counter   ;
Upcom     GenXicc::s_upcom     ;
Vegcross  GenXicc::s_vegcross  ;
Confine   GenXicc::s_confine   ;
Funtrans  GenXicc::s_funtrans  ;
Loggrade  GenXicc::s_loggrade  ; 
Mixevnt   GenXicc::s_mixevnt   ;  
Outpdf    GenXicc::s_outpdf    ;   
Subopen   GenXicc::s_subopen   ;  
Usertran  GenXicc::s_usertran  ; 
Vegasinf  GenXicc::s_vegasinf  ; 
Vegasbin  GenXicc::s_vegasbin  ; 
Valmatrix GenXicc::s_valmatrix ; 
Mtypeofxi GenXicc::s_mtypeofxi ; 
Wbstate   GenXicc::s_wbstate   ; 

	 
//SetParameter Fortran function for Xicc by F. Zhang 10-04-11
extern "C" {
#ifdef WIN32
  void __stdcall SETXICCDEFAULTPARAMETERS( ) ;
#else
  void setxiccdefaultparameters_( ) ;
#endif
}

extern "C" {
#ifdef WIN32
  void __stdcall SETXICCCONSISTENTPARAMETERS( ) ;
#else
  void setxiccconsistentparameters_( ) ;
#endif
}

void GenXicc::SetXiccDefaultParameters( ) {
#ifdef WIN32
  SETXICCDEFAULTPARAMETERS( );
#else
  setxiccdefaultparameters_( );
#endif
}

void GenXicc::SetXiccConsistentParameters( ) {
#ifdef WIN32
  SETXICCCONSISTENTPARAMETERS( );
#else
  setxiccconsistentparameters_( );
#endif
}

//EvntInit Fortran function for Xicc by F. Zhang 10-04-11
extern "C" {
#ifdef WIN32
  void __stdcall EVNTINIT( ) ;
#else
  void evntinit_( ) ;
#endif
}

void GenXicc::EvntInit( ) {
#ifdef WIN32
  EVNTINIT( );
#else
  evntinit_( );
#endif
}
