//
//  CSMember.h
//  CDK
//
//  Created by Kim Chan on 2016. 9. 13..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef __CDK__CSMember__
#define __CDK__CSMember__

#include "CSMacro.h"

#include "CSObject.h"

template <typename Member, int csobject = derived<CSObject, Member>::value>
class CSMemberImpl {
};

template <typename Member>
class CSMemberImpl<Member, 1> {
public:
    static inline void retain(Member*& dest, Member* src) {
        CSObject::retain(dest, src);
    }
    
    static inline void retain(Member* member) {
        CSObject::retain(member);
    }
    
    static inline void release(Member* member) {
        CSObject::release(member);
    }
    
    static inline uint hash(Member* member) {
        return member->hash();
    }
    
    static inline bool isEqual(Member* a, Member* b) {
        return a->isEqual(b);
    }
};

template <typename Member>
class CSMemberImpl<Member, 0> {
public:
    static inline void retain(Member*& dest, Member* src) {
        dest = src;
    }
    
    static inline void retain(Member* member) {
    }
    
    static inline void release(Member* member) {
    }
    
    static inline uint hash(Member* member) {
        return (uint)*member;
    }
    
    static inline bool isEqual(Member* a, Member* b) {
        return a == b;
    }
};

class CSMember {
public:
    template <typename Member>
    static inline void release(Member* member) {
        CSMemberImpl<Member>::release(member);
    }
    
    template <typename Member>
    static inline void release(Member member) {
    }
    
    template <typename Member>
    static inline void retain(Member* member) {
        CSMemberImpl<Member>::retain(member);
    }
    
    template <typename Member>
    static inline void retain(Member member) {
    }
    
    template <class Member>
    static inline void retain(Member*& dest, Member* src) {
        CSMemberImpl<Member>::retain(dest, src);
    }
    
    template <typename Member>
    static inline void retain(Member& dest, const Member& src) {
        dest = src;
    }
    
    template <typename Member>
    static inline uint hash(Member* member) {
        return CSMemberImpl<Member>::hash(member);
    }
    
    template <typename Member>
    static inline uint hash(Member member) {
        return (uint)member;
    }
    
    template <typename Member>
    static inline bool isEqual(Member* a, Member* b) {
        return CSMemberImpl<Member>::isEqual(a, b);
    }
    
    template <typename Member>
    static inline bool isEqual(Member a, Member b) {
        return a == b;
    }
};


#endif /* __CDK__CSMember__ */
