//
//  SoundButton.h
//  TalesCraft2
//
//  Created by ChangSun on 2016. 10. 19..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef SoundButton_h
#define SoundButton_h

#include "AssetIndex.h"

class SoundButton : public CSButton {
private:
    SoundIndex _index;
public:
    SoundButton(const SoundIndex& index);
private:
    inline virtual ~SoundButton() {}
public:
    static inline SoundButton* button(const SoundIndex& index) {
        return autorelease(new SoundButton(index));
    }
protected:
    virtual void onTouchesEnded() override;
};



#endif /* SoundButton_h */
