// $Id:$
//====================================================================
//  AIDA Detector description implementation for LCD
//--------------------------------------------------------------------
//
//  Author     : M.Frank
//
//====================================================================

#include "DetDesc/detector/DetFactoryHelper.h"
#include "DetDesc/detector/CompactDetectors.h"

using namespace std;
using namespace DetDesc;
using namespace DetDesc::Geometry;

namespace DetDesc { namespace Geometry {

  template <> Ref_t DetElementFactory<DiskTracker>::create(LCDD& lcdd, const xml_h& e, SensitiveDetector& sens)  {
    xml_det_t  x_det     = e;
    Material   air       = lcdd.air();
    string     det_name  = x_det.nameStr();
    string     det_type  = x_det.typeStr();
    bool       reflect   = x_det.reflect();
    DetElement sdet (lcdd,det_name,det_type,x_det.id());
    Volume     motherVol = lcdd.pickMotherVolume(sdet);
    int l_num = 0;

    for(xml_coll_t i(x_det,_X(layer)); i; ++i, ++l_num)  {
      xml_comp_t x_layer = i;
      string l_nam = det_name+_toString(l_num,"_layer%d");
      double  zmin = x_layer.inner_z();
      double  rmin = x_layer.inner_r();
      double  rmax = x_layer.outer_r();
      double  z    = zmin, layerWidth = 0.;
      Tube    l_tub(lcdd,l_nam,rmin,rmax,2*z,2*M_PI);
      Volume  l_vol(lcdd,l_nam+"_volume",l_tub,air);
      int     s_num = 0;

      for(xml_coll_t j(x_layer,_X(slice)); j; ++j)  {
	double thickness = xml_comp_t(j).thickness();
	layerWidth += thickness;
      }
      l_vol.setVisAttributes(lcdd,x_layer.visStr());
      for(xml_coll_t j(x_layer,_X(slice)); j; ++j, ++s_num)  {
	xml_comp_t x_slice = j;
	double thick = x_slice.thickness();
	Material mat = lcdd.material(x_slice.materialStr());
	string s_nam = l_nam+_toString(s_num,"_slice%d");
	Tube   s_tub(lcdd,s_nam,rmin,rmax,2*M_PI);
	Volume s_vol(lcdd,s_nam+"_volume", s_tub, mat);

	if ( x_slice.isSensitive() ) s_vol.setSensitiveDetector(sens);
	s_vol.setAttributes(lcdd,x_slice.regionStr(),x_slice.limitsStr(),x_slice.visStr());

	PlacedVolume spv = l_vol.placeVolume(s_vol,Position(0,0,z-zmin-layerWidth/2+thick/2));
	spv.addPhysVolID(_X(layer),l_num);
	spv.addPhysVolID(_X(slice),s_num);
      }

      PlacedVolume lpv = motherVol.placeVolume(l_vol,Position(0,0,zmin+layerWidth/2.));
      lpv.addPhysVolID(_X(system),sdet.id());
      lpv.addPhysVolID(_X(barrel),1);
      DetElement layer(lcdd,l_nam,det_type+"/Layer",l_num);
      sdet.add(layer.addPlacement(lpv));
      if ( reflect )  {
	PlacedVolume lpvR = motherVol.placeVolume(l_vol,Position(0,0,-zmin-layerWidth/2),ReflectRot());
	lpvR.addPhysVolID(_X(system),sdet.id());
	lpvR.addPhysVolID(_X(barrel),2);
	DetElement layerR(lcdd,l_nam+"_reflect",det_type+"/Layer",l_num);
	sdet.add(layerR.addPlacement(lpvR));
      }
    }
    sdet.setCombineHits(x_det.attr<bool>(_A(combineHits)),sens);
    return sdet;
  }
}}

DECLARE_NAMED_DETELEMENT_FACTORY(DetDesc,DiskTracker);
