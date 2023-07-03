//
//  CSMacro.h
//  CDK
//
//  Created by 김찬 on 2015. 10. 22..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef __CDK__CSMacro__
#define __CDK__CSMacro__

template <typename B, typename D>
struct derived_host {
    operator const B*() const;
    operator D*();
};

template <typename B, typename D>
struct derived {
    template <typename T>
    static int sfinae(D*, T);
    static char sfinae(const B*, int);
    
    static const bool value = sizeof(sfinae(derived_host<B, D>(), int())) == sizeof(int);
};

#define countof(arr)    (sizeof(arr) / sizeof(arr[0]))

template <class T> struct non_deduced { using type = T; };
template <class T> using non_deduced_t = typename non_deduced<T>::type;

#endif
