
//----------------------------------------------------------------------------
/** @file DeRichRadiator.h
 *
 *  Header file for detector description class : DeRichRadiator
 *
 *  CVS Log :-
 *  $Id: DeRichRadiator.h,v 1.27 2009-11-11 17:27:29 papanest Exp $
 *
 *  @author Antonis Papanestis a.papanestis@rl.ac.uk
 *  @date   2004-06-18
 */
//----------------------------------------------------------------------------

#ifndef RICHDET_DERICHRADIATOR_H
#define RICHDET_DERICHRADIATOR_H 1

// STL
#include <vector>

// Math typedefs
#include "GaudiKernel/Point3DTypes.h"
#include "GaudiKernel/GaudiException.h"

// DetDesc
#include "DetDesc/ISolid.h"
#include "DetDesc/TabulatedProperty.h"
#include "DetDesc/Material.h"
#include "DetDesc/IGeometryInfo.h"

// Kernel
#include "Kernel/RichRadiatorType.h"
#include "Kernel/RichDetectorType.h"
#include "Kernel/RichRadIntersection.h"

// RichDet
#include "RichDet/DeRichBase.h"
#include "RichDet/Rich1DTabProperty.h"
#include "RichDet/DeRichLocations.h"

//----------------------------------------------------------------------------
/** @class DeRichRadiator DeRichRadiator.h
 *
 * DeRichRadiator provides information about the Rich radiators.
 *
 * This is the common base class of the Rich Single/Multi Solid Radiator
 * detector classes.
 *
 * @author Antonis Papanestis a.papanestis@rl.ac.uk
 */
//----------------------------------------------------------------------------

class DeRichRadiator: public DeRichBase
{

public:

  /// Type for a vector of pointers to DeRichRadiators
  typedef std::vector<DeRichRadiator*> Vector;

public:

  /// Default constructor
  DeRichRadiator( const std::string & name = "" );

  /// Destructor
  virtual ~DeRichRadiator();

  /**
   * This is where most of the geometry is read and variables initialised
   * @return Status of initialisation
   * @retval StatusCode::FAILURE Initialisation failed, program should
   * terminate
   */
  virtual StatusCode initialize();

  /**
   * Retrieves the id of this radiator
   * @return The id of this radiator (Aerogel, C4F10, CF4)
   */
  inline Rich::RadiatorType radiatorID() const
  {
    return m_radiatorID;
  }

  /**
   * Retrieves the rich detector of this radiator
   * @return the rich detector of this radiator (Rich1/2)
   */
  inline Rich::DetectorType rich() const
  {
    return m_rich;
  }

  /**
   * Retrieves the refractive index of the radiator with the option to use HLT
   * conditions for pressure and temperature (first checking if it is valid)
   * @param hlt Return the HLT refractive index if true
   * @return A pointer to the refractive index  interpolated function of the radiator
   * @retval NULL No interpolation function
   */
  inline const Rich::TabulatedProperty1D* refIndex( bool hlt = false ) const
  {
    return ( hlt ? hltRefIndex() : checkRefIndex() );
  }

  /**
   * Retrieves The refractive index of the radiator for use by the HLT
   * @return A pointer to the HLT refractive index interpolated function of the radiator
   * @retval NULL No interpolation function
   */
  inline const Rich::TabulatedProperty1D* hltRefIndex() const
  {
    return ( m_hltRefIndex ? m_hltRefIndex : generateHltRefIndex() );
  }

  /**
   * Retrieves The refractive index of the radiator
   * @return A pointer to the refractive index of the radiator
   */
  inline const TabulatedProperty* refIndexTabProp() const
  {
    return m_refIndexTabProp;
  }

  /**
   * Retrieves the Rayleigh properties of the radiator
   * @return A pointer to the Rayleigh interpolated function
   * @retval NULL No interpolation function
   */
  inline const Rich::TabulatedProperty1D* rayleigh() const
  {
    return m_rayleigh;
  }

  /**
   * Retrieves the Rayleigh properties of the radiator
   * @return A pointer to the Rayleigh tab property
   * @retval NULL No Rayleigh tab property
   */
  inline const TabulatedProperty* rayleighTabProp() const
  {
    return m_rayleighTabProp;
  }

  /**
   * Retrieves the absorption properties of the radiator
   * @return A pointer to the absorption interpolated function
   * @retval NULL No interpolation function
   */
  inline const Rich::TabulatedProperty1D* absorption() const
  {
    return m_absorption;
  }

  /**
   * Retrieves the absorption properties of the radiator
   * @return A pointer to the absorption tab property
   * @retval NULL No tab property
   */
  inline const TabulatedProperty* absorptionTabProp() const
  {
    return m_absorptionTabProp;
  }

  /**
   * Finds the next intersection point with radiator.
   * @return Status of intersection
   * @retval StatusCode::FAILURE No intersection
   */
  virtual StatusCode nextIntersectionPoint( const Gaudi::XYZPoint& pGlobal,
                                            const Gaudi::XYZVector& vGlobal,
                                            Gaudi::XYZPoint& returnPoint ) const = 0;

  /**
   * Finds the entry and exit points of the radiator. For boolean solids
   * this is the first and last intersection point.
   * @return Status of intersection
   * @retval StatusCode::FAILURE if there is no intersection
   */
  virtual StatusCode intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                         const Gaudi::XYZVector& vGlobal,
                                         Gaudi::XYZPoint& entryPoint,
                                         Gaudi::XYZPoint& exitPoint ) const = 0;

  /**
   * Finds the intersection points with radiator. For boolean solids there
   * can be more than two intersection points
   *
   * @return The number of intersection points.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersectionPoints( const Gaudi::XYZPoint& pGlobal,
                                           const Gaudi::XYZVector& vGlobal,
                                           std::vector<Gaudi::XYZPoint>& points ) const = 0;


  /**
   * Finds the intersections (entry/exit) with radiator. For boolean solids there
   * can be more than one intersections
   *
   * @return The number of intersections.
   * @retval Zero if there is no intersction.
   */
  virtual unsigned int intersections( const Gaudi::XYZPoint& pGlobal,
                                      const Gaudi::XYZVector& vGlobal,
                                      std::vector<RichRadIntersection>& intersections ) const = 0;

  /** Returns the refractive index at the given photon energy for this radiator
   *  @param energy The photon energy
   *  @param hlt Return the HLT refractive index if true
   *  @return The refractive index at that energy
   */
  virtual double refractiveIndex( const double energy, bool hlt = false ) const = 0;

protected:

  /// Initialise Tab Property Interpolators
  StatusCode initTabPropInterpolators();

  /**
   * Generates and returns the refractive index of the radiator for use by the HLT
   * @return A pointer to the HLT refractive index interpolated function of the radiator
   * @retval NULL No interpolation function
   */
  virtual const Rich::TabulatedProperty1D* generateHltRefIndex() const;

  /**
   * Checks if the refractive index is valid. If not it throws an exception
   * @return A pointer to the refractive index  interpolated function of the radiator
   * @retval NULL No interpolation function
   */
  inline const Rich::TabulatedProperty1D* checkRefIndex() const
  {
    if ( m_refIndex )
      return m_refIndex;
    else
      throw GaudiException( "Invalid refractive index", "DeRichRadiator", StatusCode::FAILURE );

  }


protected:

  Rich::RadiatorType m_radiatorID;  ///< The radiator id (Aerogel, CF4, C4F10)
  Rich::DetectorType m_rich;        ///< The Rich detector of this radiator

  /// pointer to the refractive index of the material
  Rich::TabulatedProperty1D* m_refIndex;

  /// pointer to the refractive index of the material used by the HLT
  mutable Rich::TabulatedProperty1D* m_hltRefIndex;

  /// pointer to the Tabulated property refractive index
  const TabulatedProperty* m_refIndexTabProp;

  /// pointer to the Tabulated property refractive index
  mutable TabulatedProperty* m_hltRefIndexTabProp;

  /// pointer to the Tabulated property Cherenkov Ref Index
  const TabulatedProperty* m_chkvRefIndexTabProp;

  /// pointer to the Rayleigh scattering properties
  Rich::TabulatedProperty1D* m_rayleigh;

  /// pointer to the Tabulated property Rayleigh
  const TabulatedProperty* m_rayleighTabProp;

  /// pointer to the absorption tabulated function
  Rich::TabulatedProperty1D* m_absorption;

  /// pointer to the Tabulated property for the absoption
  const TabulatedProperty* m_absorptionTabProp;

  /// First update flag
  bool m_firstUpdate;

};

#endif    // RICHDET_DERICHRADIATOR_H
