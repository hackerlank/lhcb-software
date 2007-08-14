// $Id: ITsaStubLinker.h,v 1.1.1.1 2007-08-14 13:42:02 jonrob Exp $
#ifndef _ITsaStubLinker_H
#define _ITsaStubLinker_H

#include "GaudiKernel/IAlgTool.h"

static const InterfaceID IID_ITsaStubLinker( "ITsaStubLinker", 0, 0 );

#include <vector>

namespace Tf
{
  namespace Tsa
  {

    class SeedStub;
    class SeedTrack;

    /** @class ITsaStubLinker ITsaStubLinker.h TsaKernel/ITsaStubLinker.h
     *
     *  Interface to Tsa Stub Linker
     *
     *  @author M.Needham
     *  @date   12/11/2006
     */

    class ITsaStubLinker: virtual public IAlgTool {
    public:

      /// Retrieve interface ID
      static const InterfaceID& interfaceID() { return IID_ITsaStubLinker ; }

      virtual StatusCode execute(  std::vector<SeedStub*> stubs[], std::vector<SeedTrack*>& seeds  ) = 0;

    };

  }
}

#endif
