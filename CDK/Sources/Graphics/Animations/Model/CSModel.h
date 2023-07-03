//
//  CSModel.h
//  TalesCraft2
//
//  Created by Kim Chan on 2020. 3. 9..
//  Copyright © 2020년 brgames. All rights reserved.
//

#ifndef CSModel_h
#define CSModel_h

#include "CSAnimationSubstance.h"

#include "CSMesh.h"

class CSBuffer;

class CSModel : public CSAnimationSubstance {
private:
    const CSArray<ushort>* _meshIndices;
    const CSArray<CSMeshMaterial>* _materials;
    float _progress;
public:
    CSModel(CSBuffer* buffer);
    CSModel(const CSModel* other, bool capture);
private:
    ~CSModel();
public:
    static inline CSModel* modelWithBuffer(CSBuffer* buffer) {
        return autorelease(new CSModel(buffer));
    }
    static inline CSModel* modelWithModel(const CSModel* other, bool capture) {
        return autorelease(new CSModel(other, capture));
    }
    
    inline void setMeshIndices(const CSArray<ushort>* meshIndices) {
        retain(_meshIndices, meshIndices);
    }
    inline const CSArray<ushort>* meshIndices() const {
        return _meshIndices;
    }
    inline void setMaterials(const CSArray<CSMeshMaterial>* materials) {
        retain(_materials, materials);
    }
    inline const CSArray<CSMeshMaterial>* materials() const {
        return _materials;
    }

    inline CSAnimationSubstanceType type() const override {
        return CSAnimationSubstanceTypeModel;
    }
    inline float duration(const CSCamera* camera, CSAnimationDuration type = CSAnimationDurationMax, float duration = 0.0f) const override {
        return 0.0f;
    }
	inline bool isFinite(bool finite) const override {
		return finite;
	}
    inline void rewind() override {
        _progress = 0;
    }
    CSAnimationState update(float delta, const CSCamera* camera, bool alive = true) override;
    void draw(CSGraphics* graphics, bool shadow) override;
    void preload(CSResourceDelegate* delegate = NULL) const override;
    
    inline CSAnimationSubstance* copy(bool capture) const override {
        return new CSModel(this, capture);
    }
};

#endif /* CSModel_h */
