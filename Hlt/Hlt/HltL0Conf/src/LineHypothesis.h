#ifndef LINEHYPOTHESIS_H 
#define LINEHYPOTHESIS_H 1

// Include files
#include "TfKernel/TStationHitManager.h"

/** @class LineHypothesis LineHypothesis.h
 *
 *  Helper class which gives a line hypothesis in x and
 *  a straight line in y 
 *  to be used for T-station decoding in the Tf framework
 *  
 *
 *  @author Johannes Albrecht
 *  @date   2008-01-18
 */
class LineHypothesis : virtual public Tf::IStationSelector {
public: 
  /// Standard constructor
  LineHypothesis( float my , float cy , float mx , float cx,
                  float dx, float dy ,
                  double zMin, double zMax ) :
	  m_y(my), m_cy(cy), m_x(mx), m_cx(cx), m_dx(dx), m_dy(dy),
    m_zMin(zMin), m_zMax(zMax)
  { }

  Tf::XYSearchWindow searchWindow(float zz) const
  {
    float z = zz;
    float x = m_x * z + m_cx;
    float y = m_y * z + m_cy;
    return Tf::XYSearchWindow(x - m_dx, x + m_dx, y - m_dy, y + m_dy);
  }

  void getValidity(double& zmin, double& zmax) const
  {
    zmin = m_zMin;
    zmax = m_zMax;
    return;
  }

private:
  float m_y, m_cy;
  float m_x, m_cx;
  float m_dx, m_dy;
  double m_zMin, m_zMax;
};
#endif // LINEHYPOTHESIS_H
