// $Id: SelectB0JPsiKs.h,v 1.1 2002-03-27 20:54:03 gcorti Exp $
#ifndef USER_SELECTB0JPSIKS_H 
#define USER_SELECTB0JPSIKS_H 1

// Include files
// from STL
#include <string>

// from Gaudi
#include "GaudiKernel/Algorithm.h"

// Forward declarations
class IAssociator;
class IPhysSelTool;
class IParticlePropertySvc;
class IHistogram1D;
class IAddFourMom;
class IPhysDesktop;
class IMassVertexFitter;
class IVertexFitter;
class IGeomDispCalculator;
class IParticleFilter;
class IParticleStuffer;

/** @class SelectB0JPsiKs SelectB0JPsiKs.h User/SelectB0JPsiKs.h
 *  
 *  
 *  @author Claudia Pereira Nunes
 *  @date   18/03/2002
 */
class SelectB0JPsiKs : public Algorithm {
public:
  /// Standard constructor
  SelectB0JPsiKs( const std::string& name, ISvcLocator* pSvcLocator );

  virtual ~SelectB0JPsiKs( ); ///< Destructor

  virtual StatusCode initialize();    ///< Algorithm initialization
  virtual StatusCode execute   ();    ///< Algorithm execution
  virtual StatusCode finalize  ();    ///< Algorithm finalization

protected:

private:
  std::string m_pNameSearch;  ///< MCParticle name to search for
  long        m_pIDSearch;    ///< MCParticle ID to search for

  std::vector<std::string>  m_daugName; ///< Name of daughters to be found
  std::vector<long>         m_daugID;   ///< GeantID of daughters to be found  

  int         m_nEvents;     ///< N events processed

  

  double		m_b0masswinloo;         // Mass window for the two Mu Combination
  double		m_b0zwin;         // Z vertex window for the two Mu Combination
  double                m_chi2UncVtxCut;
    
  double                m_b0masswinti;
    
  double                m_impParamB0Cut;
  double                m_impParamKsJPsiCut;  

  bool			m_SelB0;
  
  int		m_B0Count;
  double                m_B0Mass; 

  
  IAssociator*     m_pAsct;    ///< Reference to associator used  
  IPhysSelTool*    m_pSelTool;  ///< Reference to tool for Phys Selection 
                                ///< details
  IParticlePropertySvc* m_ppSvc;   ///< Reference to particle property service
  	
  IPhysDesktop* m_pDesktop;        ///< Reference to desktop tool
  IMassVertexFitter* m_pLagFit; ///< Reference to Mass Constrained Vertex Fitter
  IVertexFitter* m_pVertexFit;    ///< Reference to unconstrained
  IGeomDispCalculator* m_pGeomDispCalc;  ///< Reference to geometrical calc. 
  IParticleFilter* m_pFilter;  ///< Reference to ParticleFilter. 
  IParticleStuffer* m_pStuffer;  ///< Reference to ParticleStuffer. 

  
  bool                  m_produceHistogram;    // flag for histo production

  // Histograms ( used if m_produceHistogram = 1 (true) )
  IHistogram1D*         m_hChi2UncFit;
  IHistogram1D*         m_hSum4p;
  IHistogram1D*         m_hB0Mass;
  IHistogram1D*         m_hImpParamB0;
  IHistogram1D*         m_hImpParamKsJPsi;
 



  // Retrieve SdtHep ID for JPsi or Ks
  long   m_JPsiID;
  long   m_KsID;
  long   m_B0ID;

};
#endif // USER_SELECTB0JPSIKS_H
