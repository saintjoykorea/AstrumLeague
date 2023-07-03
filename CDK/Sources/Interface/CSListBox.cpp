//
//  CSListBox.cpp
//  CDK
//
//  Created by 김찬 on 12. 8. 20..
//  Copyright (c) 2012년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSListBox.h"

CSListBox::CSListBox() {
    setLayout(CSLayerLayoutVertical);
    _autoFocus = true;
}

bool CSListBox::addLayer(CSLayer* layer) {
    if (CSScrollPane::addLayer(layer)) {
        layer->OnTouchesBegan.add(this, &CSListBox::onTouchesBeganItem);
        return true;
    }
    return false;
}

bool CSListBox::insertLayer(uint index, CSLayer* layer) {
    if (CSScrollPane::insertLayer(index, layer)) {
        layer->OnTouchesBegan.add(this, &CSListBox::onTouchesBeganItem);
        return true;
    }
    return false;
}

void CSListBox::detach(CSLayer* layer) {
    if (_selectedItem == layer) {
        onSelect(NULL);
        _selectedItem = NULL;
        
        layer->OnTouchesBegan.remove(this);
    }
}

void CSListBox::changeSelectedItem(CSLayer* selectedItem) {
    _selectionChanged = _selectedItem != selectedItem;
    
    if (_selectedItem != selectedItem) {
        if (_selectedItem) {
            _selectedItem->cancelTouches();
        }
        onSelect(selectedItem);
        _selectedItem = selectedItem;
        
        if (_autoFocus) {
            focusLayer(_selectedItem);
        }
    }
    else {
        onSelect(selectedItem);
    }
    refresh();
}

void CSListBox::setSelectedItem(CSLayer* selectedItem) {
    if (selectedItem == NULL || _layers->containsObject(selectedItem)) {
        changeSelectedItem(selectedItem);
    }
}

int CSListBox::selectedIndex() const {
    return _selectedItem ? _layers->indexOfObject(_selectedItem) : CSNotFound;
}

void CSListBox::setSelectedIndex(int selectedIndex) {
    CSLayer* selectedItem = selectedIndex < 0 ? NULL : _layers->objectAtIndex(selectedIndex);
    
    changeSelectedItem(selectedItem);
}

void CSListBox::submitLayout() {
    CSLayer::submitLayout();
    
    if (_autoFocus && _selectedItem) {
        focusLayer(_selectedItem);
    }
}

void CSListBox::onTouchesMoved() {
    CSScrollPane::onTouchesMoved();
    
    if (_focusScroll && scroll.isScrolling()) {
        setSelectedItem(NULL);
    }
}

void CSListBox::onTouchesBeganItem(CSLayer* layer) {
    CSListBox* listBox = static_cast<CSListBox*>(layer->parent());
    if (!listBox->scroll.isScrolling()) {
        listBox->changeSelectedItem(layer);
    }
}

void CSListBox::onSelect(CSLayer* next) {
    OnSelect(this, next);
}

