//
//  Destination.h
//  TalesCraft2
//
//  Created by 김찬 on 14. 5. 29..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef Destination_h
#define Destination_h

#include "Object.h"

class Destination : public Object {
private:
	const Force* _force;
public:
    Destination(const Force* force, float radius = 0);
private:
    ~Destination();
public:
    static Destination* destination(const Force* force, float radius = 0);
    
	ObjectDisplay display() const override;

    inline bool update(bool running) override {
        Object::update(running);
        return retainCount() > 1;
    }
    inline void draw(CSGraphics* graphics, ObjectLayer layer) override {
    
    }
};

#endif /* Destination_h */
