//
//  Prop.h
//  TalesCraft2
//
//  Created by 김찬 on 2015. 10. 26..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifndef Prop_h
#define Prop_h

#include "Object.h"

class Prop : public Object {
private:
    struct {
        Animation* animation;
        float remaining;
		bool relocated;
    } _motion;
    CSQuaternion _rotation;
    float _translation;
    float _scale;
    bool _environment;

    Prop(const AnimationIndex& index, float translation, float scale, const CSQuaternion& rotation, float radius, bool environment);
    ~Prop();
public:
    static Prop* prop(const AnimationIndex& index, float translation, float scale, const CSQuaternion& rotation, float radius, bool environment);
public:
    void locate(const FPoint& point) override;
    ObjectDisplay display() const override;
    bool update(bool running) override;
    void draw(CSGraphics* graphics, ObjectLayer layer) override;
};

#endif /* Prop_h */
