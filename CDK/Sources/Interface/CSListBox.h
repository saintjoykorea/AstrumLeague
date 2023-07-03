//
//  CSListBox.h
//  CDK
//
//  Created by 김찬 on 12. 8. 20..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSListBox__
#define __CDK__CSListBox__

#include "CSScrollPane.h"

class CSListBox;

class CSListBox : public CSScrollPane {
public:
	CSHandler<CSListBox*, CSLayer*> OnSelect;
protected:
    CSLayer* _selectedItem;
    bool _selectionChanged;
    bool _autoFocus;
    
public:
    CSListBox();
protected:
    inline virtual ~CSListBox() {
    
    }
public:
    static inline CSListBox* listBox() {
        return autorelease(new CSListBox());
    }
protected:
    virtual void onSelect(CSLayer* next);
    
public:
    inline CSLayer* selectedItem() const {
        return _selectedItem;
    }
    
    void setSelectedItem(CSLayer* selectedItem);
    int selectedIndex() const;
    void setSelectedIndex(int selectedIndex);
    inline bool selectionChanged() const {
        return _selectionChanged;
    }
    
    inline bool autoFocus() const {
        return _autoFocus;
    }
    
    inline void setAutoFocus(bool autoFocus) {
        _autoFocus = autoFocus;
    }
    
    virtual bool addLayer(CSLayer* layer) override;
    virtual bool insertLayer(uint index, CSLayer* layer) override;
    
protected:
    virtual void detach(CSLayer* layer) override;
    virtual void submitLayout() override;
    virtual void onTouchesMoved() override;
private:
    void onTouchesBeganItem(CSLayer* layer);
    void changeSelectedItem(CSLayer* selectedItem);
};

#endif /* defined(__CDK__CSListBox__) */
