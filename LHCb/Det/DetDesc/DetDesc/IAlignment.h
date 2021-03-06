#ifndef DETDESC_IALIGNMENT_H
#define DETDESC_IALIGNMENT_H 1

// Base class
#include "DetDesc/IConditionInfo.h"

///---------------------------------------------------------------------------
/** @class IAlignment IAlignment.h Det/DetDesc/IAlignment.h

    An abstract interface to get the alignment information of
    a detector element.

    @author Andrea Valassi 
    @date December 2001
*///--------------------------------------------------------------------------

class IAlignment : virtual public IConditionInfo
{

};

#endif // DETDESC_IALIGNMENT_H




