//
//  AssetIndex.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 8. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef AssetIndex_h
#define AssetIndex_h

#include "Application.h"

struct AssetIndex2 {
    ushort indices[2];
    
    static const AssetIndex2 None;
    
    inline AssetIndex2() {
    }
    
    AssetIndex2(const byte*& cursor);
    AssetIndex2(CSBuffer* buffer);
    AssetIndex2(int index0, int index1);
    AssetIndex2(int raw);
    
    inline bool operator ==(const AssetIndex2& other) const {
        return other.indices[0] == indices[0] && other.indices[1] == indices[1];
    }
    
    inline bool operator !=(const AssetIndex2& other) const {
        return !(*this == other);
    }
    
    inline operator int() const {
        return indices[0] << 16 | indices[1];
    }

    inline operator int64() const {
        return operator int();
    }

    inline operator uint() const {
        return operator int();
    }
        
    inline operator bool() const {
        return *this != None;
    }

    inline bool operator !() const {
        return *this == None;
    }
    
    void writeTo(CSBuffer* buffer) const;
    
    template <typename T>
    static CSArray<AssetIndex2>* indicesFromArray(const CSArray<T, 2>* arr) {
        CSArray<AssetIndex2>* indices = CSArray<AssetIndex2>::array();
        if (arr) {
            for (int i = 0; i < arr->count(); i++) {
                const CSArray<T>* arr2 = arr->objectAtIndex(i);
                if (arr2) {
                    for (int j = 0; j < arr2->count(); j++) {
                        indices->addObject(AssetIndex2(i, j));
                    }
                }
            }
        }
        return indices;
    }
};

struct AssetIndex3 {
    ushort indices[3];
    
    static const AssetIndex3 None;
    
    inline AssetIndex3() {
    }
    
    AssetIndex3(const byte*& cursor);
    AssetIndex3(CSBuffer* buffer);
    AssetIndex3(int index0, int index1, int index2);
    AssetIndex3(const AssetIndex2& group, int index2);
    AssetIndex3(int64 raw);
    
    inline AssetIndex2 group() const {
        return AssetIndex2(indices[0], indices[1]);
    }
    
    inline bool operator ==(const AssetIndex3& other) const {
        return other.indices[0] == indices[0] && other.indices[1] == indices[1] && other.indices[2] == indices[2];
    }
    
    inline bool operator !=(const AssetIndex3& other) const {
        return !(*this == other);
    }
    
    inline operator int64() const {
        return ((int64)indices[0] << 32) | ((int64)indices[1] << 16) | indices[2];
    }

    inline operator uint() const {
        return operator int64();
    }
    
    inline operator bool() const {
        return *this != None;
    }

    inline bool operator !() const {
        return *this == None;
    }
    
    void writeTo(CSBuffer* buffer) const;
    
    template <typename T>
    static CSArray<AssetIndex3>* indicesFromArray(const CSArray<T, 3>* arr) {
        CSArray<AssetIndex3>* indices = CSArray<AssetIndex3>::array();
        if (arr) {
            for (int i = 0; i < arr->count(); i++) {
                const CSArray<T>* arr2 = arr->objectAtIndex(i);
                if (arr2) {
                    for (int j = 0; j < arr2->count(); j++) {
                        const CSArray<T>* arr3 = arr->objectAtIndex(i);
                        if (arr3) {
                            for (int k = 0; k < arr3->count(); k++) {
                                indices->addObject(AssetIndex3(i, j, k));
                            }
                        }
                    }
                }
            }
        }
        return indices;
    }
};

struct AssetIndex4 {
    ushort indices[4];
    
    static const AssetIndex4 None;
    
    inline AssetIndex4() {
    }
    
    AssetIndex4(const byte*& cursor);
    AssetIndex4(CSBuffer* buffer);
    AssetIndex4(int index0, int index1, int index2, int index3);
    AssetIndex4(const AssetIndex2& group, int index2, int index3);
    AssetIndex4(const AssetIndex3& group, int index3);
    AssetIndex4(int64 raw);
    
    inline AssetIndex3 group() const {
        return AssetIndex3(indices[0], indices[1], indices[2]);
    }
    
    inline bool operator ==(const AssetIndex4& other) const {
        return other.indices[0] == indices[0] && other.indices[1] == indices[1] && other.indices[2] == indices[2] && other.indices[3] == indices[3];
    }
    
    inline bool operator !=(const AssetIndex4& other) const {
        return !(*this == other);
    }
    
    inline operator int64() const {
        return ((int64)indices[0] << 48) | ((int64)indices[1] << 32) | ((int64)indices[2] << 16) | indices[3];
    }

    inline operator uint() const {
        return operator int64();
    }
    
    inline operator bool() const {
        return *this != None;
    }

    inline bool operator !() const {
        return *this == None;
    }
    
    void writeTo(CSBuffer* buffer) const;
    
    template <typename T>
    static CSArray<AssetIndex4>* indicesFromArray(const CSArray<T, 4>* arr) {
        CSArray<AssetIndex4>* indices = CSArray<AssetIndex4>::array();
        if (arr) {
            for (int i = 0; i < arr->count(); i++) {
                const CSArray<T>* arr2 = arr->objectAtIndex(i);
                if (arr2) {
                    for (int j = 0; j < arr2->count(); j++) {
                        const CSArray<T>* arr3 = arr->objectAtIndex(i);
                        if (arr3) {
                            for (int k = 0; k < arr3->count(); k++) {
                                const CSArray<T>* arr4 = arr->objectAtIndex(i);
                                if (arr4) {
                                    for (int p = 0; p < arr4->count(); p++) {
                                        indices->addObject(AssetIndex4(i, j, k, p));
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }
        return indices;
    }
};
typedef AssetIndex2 CommandIndex;
typedef AssetIndex2 UnitIndex;
typedef AssetIndex3 SkinIndex;
typedef AssetIndex2 BuffIndex;
typedef AssetIndex2 RuneIndex;
typedef AssetIndex2 ThemeIndex;
typedef AssetIndex2 EmoticonIndex;
typedef AssetIndex2 AnimationGroupIndex;
typedef AssetIndex3 AnimationIndex;
typedef AssetIndex2 ImageGroupIndex;
typedef AssetIndex3 ImageIndex;
typedef AssetIndex2 MeshIndex;
typedef AssetIndex2 MessageIndex;
typedef AssetIndex2 SoundGroupIndex;
typedef AssetIndex3 SoundIndex;
typedef AssetIndex4 MapIndex;

#endif /* AssetIndex_h */
