//
//  CSView.h
//  CDK
//
//  Created by 김찬 on 12. 8. 1..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSView__
#define __CDK__CSView__

#include "CSDictionary.h"

#include "CSGraphics.h"

#include "CSLayer.h"

#include "CSTask.h"

#ifdef CDK_IMPL
typedef struct _CSPlatformTouch {
    uint64 key;
	CSTouchButton button;
    float x;
    float y;
} CSPlatformTouch;
#endif

struct CSEdgeInsets {
    float top;
    float left;
    float bottom;
    float right;
};

class CSView : public CSLayerContainer {
private:
    void* _handle;
    CSTask* _task;
    CSArray<CSLayer, 1, false>* _layers;
    CSDictionary<uint, CSTouch>* _touches;
    CSDictionary<uint, CSArray<CSLayer> >* _touchLayers;
    
    struct {
        float fov;
        float width;
        float height;
        float znear;
        float zfar;
    } _projection;
    
    float _keyboardHeight;
    
#ifdef CS_DISPLAY_COUNTER
    double _lastFrameElapsed;
    int _lastFrameCount;
    int _frameCount;
#endif
    float _touchSensitivity;
    float _popupDarkness;
    float _popupBlur;
    bool _enabled;
    bool _refresh;
#ifdef CS_DISPLAY_LAYER_FRAME
    bool _displayLayerFrame;
public:
    inline bool displayLayerFrame() const {
        return _displayLayerFrame;
    }
#endif
public:
    CSView(void* handle);
private:
    ~CSView();
public:
    inline void* handle() const {
        return _handle;
    }
    void setProjection(float fov, float width, float height, float znear, float zfar);
    float projectionFov() const;
    float projectionWidth() const;
    float projectionHeight() const;
    float projectionZNear() const;
    float projectionZFar() const;
public:
#ifdef CDK_IMPL
    void touchesBegan(const CSPlatformTouch* touches, uint count);
    void touchesMoved(const CSPlatformTouch* touches, uint count);
    void touchesEnded(const CSPlatformTouch* touches, uint count);
    void touchesCancelled(const CSPlatformTouch* touches, uint count);
    bool beginTouch(CSLayer* layer, const CSTouch* touch);
    void cancelTouches(CSLayer* layer);
	void wheel(float offset);
	void keyDown(int keyCode);
	void keyUp(int keyCode);
    void timeout();
    void draw(CSGraphics* graphics);
#endif
    CSRect frame() const;
    CSRect bounds() const;
    CSEdgeInsets edgeInsets() const;
    
    inline CSView* view() const override {
        return const_cast<CSView*>(this);
    }
    
    inline CSLayerContainer* parent() const override {
        return NULL;
    }
    
    inline const CSArray<CSLayer, 1, false>* layers() const {
        return _layers;
    }
    
    int layerCount() const;
    
    inline void refresh() override {
        _refresh = true;
    }
#ifdef CDK_IMPL
    void notifyPause();
    void notifyResume();
#endif
    void clearLayers(bool transition);
    bool addLayer(CSLayer* layer);
    bool insertLayer(uint index, CSLayer* layer);
    void clearLayers(int key, bool transition, bool inclusive, bool masking = false);
    const CSArray<CSLayer, 1, false>* findLayers(int key, bool masking = false);
    void findLayers(int key, CSArray<CSLayer, 1, false>* outArray, bool masking = false);
    CSLayer* findLayer(int key, bool masking = false);
    CSLayer* findParent(int key, bool masking = false) override;
    
    void insertLayerAsPopup(uint index, CSLayer* layer, float darkness, float blur);
    void addLayerAsPopup(CSLayer* layer, float darkness, float blur);
    
    inline void setPopupDarkness(float darkness) {
        _popupDarkness = darkness;
    }
    inline float popupDarkness() const {
        return _popupDarkness;
    }
    inline void setPopupBlur(float blur) {
        _popupBlur = blur;
    }
    inline float popupBlur() const {
        return _popupBlur;
    }
    inline void insertLayerAsPopup(uint index, CSLayer* layer) {
        insertLayerAsPopup(index, layer, _popupDarkness, _popupBlur);
    }
    inline void addLayerAsPopup(CSLayer* layer) {
        addLayerAsPopup(layer, _popupDarkness, _popupBlur);
    }
    
    CSRect convertToUIFrame(CSRect frame) const;
    
    void convertToLocalSpace(CSVector2* point) const override;
    inline void convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const override {
        //do nothing
    }
    inline void convertToParentSpace(CSVector2* point, int key, bool mask = false) const override {
        //do nothing
    }
    inline void convertToViewSpace(CSVector2* point) const override {
        //do nothing
    }
    inline void submitLayout() override {
        //do nothing
    }

    inline uint transition() const override {
        return 0;
    }
    
    inline float transitionProgress() const override {
        return 1.0f;
    }
    
    inline bool enabled() const {
        return _enabled;
    }
    void setEnabled(bool enabled);
    
    inline float touchSensitivity() const {
        return _touchSensitivity;
    }
    
    inline void setTouchSensitivity(float touchSensitivity) {
        _touchSensitivity = touchSensitivity;
    }
#ifdef CDK_IMPL
    void backKey();

    void setKeyboardHeight(float height);
    
#endif
    inline float keyboardHeight() const {
        return _keyboardHeight;
    }
    
    bool customEvent(int op, void* param);
    
    bool screenshot(const char* path) const;
};

#endif
