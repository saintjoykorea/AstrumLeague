//
//  CSResource.h
//  TalesCraft2
//
//  Created by Kim Chan on 2019. 12. 9..
//  Copyright © 2019년 brgames. All rights reserved.
//

#ifndef __CDK__CSResource__
#define __CDK__CSResource__

#include "CSDelegate.h"

enum CSResourceType : byte {
    CSResourceTypeImage,
    CSResourceTypeMesh,
    CSResourceTypeString
};

class CSResource : public CSObject {
protected:
    inline virtual ~CSResource() {
        
    }
public:
    virtual CSResourceType resourceType() const = 0;
};

typedef CSDelegate<const CSResource*, CSResourceType, const ushort*, int> CSResourceDelegate;

typedef CSDelegate0<const CSResource*, CSResourceType, const ushort*, int> CSResourceDelegate0;
template <typename Param0>
using CSResourceDelegate1 = CSDelegate1<Param0, const CSResource*, CSResourceType, const ushort*, int>;
template <typename Param0, typename Param1>
using CSResourceDelegate2 = CSDelegate2<Param0, Param1, const CSResource*, CSResourceType, const ushort*, int>;
template <typename Param0, typename Param1, typename Param2>
using CSResourceDelegate3 = CSDelegate3<Param0, Param1, Param2, const CSResource*, CSResourceType, const ushort*, int>;

#endif /* __CDK__CSResource__ */
