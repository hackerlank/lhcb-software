/***************************************************************************** 
 * Project: RooFit                                                           * 
 *                                                                           * 
 * This code was autogenerated by RooClassFactory                            * 
 *****************************************************************************/ 

// Your description goes here... 

#include "Riostream.h" 

#include "myExpo.h" 
#include "RooAbsReal.h" 
#include "RooAbsCategory.h" 
//#include <math.h> 
#include "TMath.h" 

ClassImp(myExpo) 

myExpo::myExpo(const char *name, const char *title, 
               RooAbsReal& _x,
               RooAbsReal& _gamma,
               RooAbsReal& _threshold
               ) :
RooAbsPdf(name,title), 
  x("x","x",this,_x),
  gamma("gamman","gamma",this,_gamma),
  threshold("threshold","threshold",this,_threshold){} 


 myExpo::myExpo(const myExpo& other, const char* name) :  
   RooAbsPdf(other,name), 
   x("x",this,other.x),
   gamma("gamma",this,other.gamma),
   threshold("threshold",this,other.threshold){} 



Double_t myExpo::evaluate() const {
  if( x < threshold)return 0;
  double t = gamma*(x-threshold);
  return exp(t);
}




