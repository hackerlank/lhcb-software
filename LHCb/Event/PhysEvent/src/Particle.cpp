// $Id: Particle.cpp,v 1.1 2002-03-27 20:05:30 gcorti Exp $
// Include files 

// STD and STL
#include <iostream> // from Gaudi
#include "GaudiKernel/StreamBuffer.h" 

// local
#include "Event/Particle.h"

//-----------------------------------------------------------------------------
// Implementation file for class : Particle
//                                 derived from class KeyedObjeect
//
// 19/03/2002 : Gloria Corti
//-----------------------------------------------------------------------------


//=============================================================================
// charge in units of positron charge
//=============================================================================
double Particle::charge() const {

  // Derive charge from particleID
  int myid = particleID().id();

  // gamma
  if( myid == 1 ) {
    return 0.0;
  }
  // e-, mu-, pi-, K-, p~
  if( (myid == 11) || (myid == 13) || (myid == -211) || 
      (myid == -321) || (myid == -2212) ) {
    return -1.0;
  }
  // e+, mu+, pi+, K+, p
  if( (myid == -11) || (myid == -13) || (myid == 211) || 
      (myid == 321) || (myid == 2212) ) {
    return 1.0;
  }
  // pi0, Kl, Ks
  if( (myid == 111) || (myid == 130) || (myid == 310) ) {
    return 0.0;
  }
  // J/Psi and B0
  if( (myid == 443) || (abs(myid) == 511) ) {
    return 0.0;
  }
  return 0.0;
};

//=============================================================================
// Set covariance matrix relative to momentum (4x4) and propagate to 
// (sx, sy, P)
//=============================================================================
void Particle::setMomentumErr(const HepSymMatrix& value)
{
  m_momentumErr = value;
  HepMatrix trMomToSlopes(3,3,0.0);
  double px = m_momentum.px();
  double py = m_momentum.py();
  double pz = m_momentum.pz();
  double p  = m_momentum.vect().mag();
  // Transpose of Transformation matrix of P(sx,sy,P) = f{ S(px,py,pz) }
  trMomToSlopes(1,1) = 1./pz;          ///< dSx/dPx
  trMomToSlopes(1,2) = 0.;             ///< dSx/dPy
  trMomToSlopes(1,3) = -px/(pz*pz);    ///< dSx/dPz
  trMomToSlopes(2,1) = 0.;             ///< dSy/dPx
  trMomToSlopes(2,2) = 1./pz;          ///< dSy/dPy
  trMomToSlopes(2,3) = -py/(pz*pz);    ///< dSy/dPz
  trMomToSlopes(3,1) = px/p;           ///< dP/dPx
  trMomToSlopes(3,2) = py/p;           ///< dP/dPy
  trMomToSlopes(3,3) = pz/p;           ///< dP/dPz  
   
  // Now obtain the new covariance matrix C(P) = transpose(T) x C(S) x T
  m_slopesMomErr = m_momentumErr.sub(1,3).similarity(trMomToSlopes);
}

//=============================================================================
// Set matrix with correlation errors between momemtum and pointOnTrack
// (momentum x point) and propagate to slopesMomCorr
//=============================================================================
void Particle::setPosMomCorr(const HepMatrix& value)
{
  m_posMomCorr = value;
}

//=============================================================================
// Set error matrix on (slopeX, slopeY, P) and propagate to momentumErr
//=============================================================================
void Particle::setSlopesMomErr(const HepSymMatrix& value)
{
  m_slopesMomErr = value;
  HepMatrix trSlopesToMom(4,3,0.0);
  double p = m_momentum.vect().mag();
  double pz = fabs(m_momentum.pz());
  double sx = slopeX();
  double sy = slopeY();
  double a = pz/p;
  // Transpose of Transformation matrix of P(px,py,pz) = f{ S(sx,sy,P) }
  trSlopesToMom(1,1) = a*pz*(1+sy*sy);   ///< dPx/dSx
  trSlopesToMom(1,2) = -a*pz*(sx*sx);    ///< dPx/dSy
  trSlopesToMom(1,3) = a*sx;             ///< dPx/dP
  trSlopesToMom(2,1) = -a*pz*(sy*sy);    ///< dPy/dSx
  trSlopesToMom(2,2) = a*pz*(1+sx*sx);   ///< dPy/dSy
  trSlopesToMom(2,3) = a*sy;             ///< dPy/dP
  trSlopesToMom(3,1) = -a*pz*sx;         ///< dPz/dSx
  trSlopesToMom(3,2) = a*sy;             ///< dPz/dSy
  trSlopesToMom(3,3) = a;                ///< dPz/dP  
  trSlopesToMom(4,1) = 0.0;              ///< dE/dSx
  trSlopesToMom(4,2) = 0.0;              ///< dE/dSy
  trSlopesToMom(4,3) = p/m_momentum.e(); ///< dE/dP
   
  // Now obtain the new covariance matrix C(P) = transpose(T) x C(S) x T
  m_momentumErr = m_slopesMomErr.similarity(trSlopesToMom);
 
}

//=============================================================================
// Set correlation matrix with errors between (slopeX, slopeY, P) 
// and pointOnTrack and propagate to posMomCorr
//=============================================================================
void Particle::setPosSlopesCorr(const HepMatrix& value)
{
  m_posSlopesCorr = value; 
}
