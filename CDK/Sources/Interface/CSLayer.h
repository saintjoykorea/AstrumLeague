//
//  CSLayer.h
//  CDK
//
//  Created by 김찬 on 12. 8. 3..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSLayer__
#define __CDK__CSLayer__

#include "CSLayerContainer.h"
#include "CSValue.h"
#include "CSString.h"
#include "CSHandler.h"
#include "CSTouch.h"
#include "CSKeyCode.h"
#include "CSGraphics.h"
#include "CSAnimation.h"

enum CSLayerState : byte  {
    CSLayerStateRemoved,
    CSLayerStateDetach,
    CSLayerStateAttach,
    CSLayerStateHidden,
    CSLayerStateFocus
};

enum CSLayerLayout : byte {
    CSLayerLayoutNone,
    CSLayerLayoutHorizontal,
    CSLayerLayoutVertical,
    CSLayerLayoutFlow
};

enum CSLayerKeyboardScroll : byte {
    CSLayerKeyboardScrollNone,
    CSLayerKeyboardScrollUp,
    CSLayerKeyboardScrollFit
};

enum CSLayerVisible {
    CSLayerVisibleCovered,
    CSLayerVisibleBackground,
    CSLayerVisibleForward
};

enum CSLayerTouchReturn {
    CSLayerTouchReturnNone,
    CSLayerTouchReturnCarry,
    CSLayerTouchReturnCatch
};

enum CSLayerTouchArea : byte {
	CSLayerTouchAreaFrame,
	CSLayerTouchAreaRange
};

enum CSLayerTransition {
    CSLayerTransitionLeft = 0x00000001,
    CSLayerTransitionRight = 0x00000002,
    CSLayerTransitionUp = 0x00000004,
    CSLayerTransitionDown = 0x00000008,
    CSLayerTransitionLeftBounce = 0x00000010,
    CSLayerTransitionRightBounce = 0x00000020,
    CSLayerTransitionUpBounce = 0x00000040,
    CSLayerTransitionDownBounce = 0x00000080,
    CSLayerTransitionHorizontalFlip = 0x00000100,
    CSLayerTransitionVerticalFlip = 0x00000200,
    CSLayerTransitionFade = 0x00000400,
    CSLayerTransitionHorizontalZoom = 0x00000800,
    CSLayerTransitionVerticalZoom = 0x000001000,
    CSLayerTransitionZoom = (0x00000800 | 0x00001000),
    CSLayerTransitionHorizontalFall = 0x00002000,
    CSLayerTransitionVerticalFall = 0x00004000,
    CSLayerTransitionFall = (0x00002000 | 0x00004000),
    CSLayerTransitionHorizontalPop = 0x0008000,
    CSLayerTransitionVerticalPop = 0x00010000,
    CSLayerTransitionPop = (0x00008000 | 0x00010000),
	CSLayerTransitionCustom = 0x80000000
};

extern const uint CSLayerTransitionMoveMask;

class CSLayer : public CSLayerContainer {
public:
    class DrawHandler : public CSHandler<CSLayer*, CSGraphics*> {
	public:
        void operator()(CSLayer* layer, CSGraphics* graphics);
    };
    DrawHandler OnDraw;
    DrawHandler OnDrawCover;
	CSHandler<CSLayer*> OnTimeout;
	CSHandler<CSLayer*, const CSArray<CSTouch>*, bool&> OnTouchesView;
	CSHandler<CSLayer*> OnTouchesBegan;
	CSHandler<CSLayer*> OnTouchesEnded;
	CSHandler<CSLayer*> OnTouchesMoved;
	CSHandler<CSLayer*> OnTouchesCancelled;
	CSHandler<CSLayer*, float> OnWheel;
	CSHandler<CSLayer*, int> OnKeyDown;
	CSHandler<CSLayer*, int> OnKeyUp;
	CSHandler<CSLayer*> OnStateChanged;
	CSHandler<CSLayer*> OnFrameChanged;
	CSHandler<CSLayer*> OnProjectionChanged;
	CSHandler<CSLayer*> OnLink;
	CSHandler<CSLayer*> OnUnlink;
	CSHandler<CSLayer*> OnPause;
	CSHandler<CSLayer*> OnResume;
	CSHandler<CSLayer*> OnBackKey;
	CSHandler<CSLayer*, int, void*, bool&> OnCustomEvent;
    
protected:
    int _key;
    CSLayerContainer* _parent;
    CSArray<CSLayer, 1, false>* _layers;
    CSArray<const CSTouch>* _touches;
    uint _transition;
    float _transitionDuration;
    float _transitionLatency;
    float _transitionAccelation;
    float _transitionProgress;
    float _transitionWeight;
    double _timeoutUpdated;
    float _timeoutInterval;
    uint _timeoutSequence;
    bool _timeoutFixedRate;
    bool _enabled;
    bool _covered;
    bool _touchInherit;
    bool _touchCarry;
    bool _touchIn;
    bool _touchOut;
    bool _touchMulti;
    bool _touchRefresh;
	CSLayerTouchArea _touchArea;
	CSLayerState _state;
    CSLayerLayout _layout;
    CSLayerKeyboardScroll _keyboardScroll;
    float _keyboardHeight;
    CSRect _frame;
    CSRect _prevFrame;
    CSRect _nextFrame;
    double _frameUpdated;
    float _frameInterval;
    float _spacing;
    float _contentWidth;
    float _contentHeight;
    uint _order;
    CSColor _color;
    const CSObject* _tag;
    const char* _tagname;
	const CSObject* _subtag;
	const char* _subtagname;
    CSArray<CSLayer>* _links;
public:
    CSLayer();
protected:
    virtual ~CSLayer();
public:
    static inline CSLayer* layer() {
        return autorelease(new CSLayer());
    }
protected:
    friend class CSView;
    
    virtual void submitLayout() override;
    virtual void submitFrame(const CSRect& frame, bool fromParent);
    virtual bool attach(CSLayerContainer* parent);
    virtual void detach(CSLayer* layer);
    
    virtual void onStateChanged();
    virtual void onTimeout();
    virtual void onDraw(CSGraphics* graphics);
    virtual void onDrawCover(CSGraphics* graphics);
    virtual void onTouchesView(const CSArray<CSTouch>* touches, bool& interrupt);
    virtual void onTouchesBegan();
    virtual void onTouchesMoved();
    virtual void onTouchesEnded();
    virtual void onTouchesCancelled();
	virtual void onWheel(float offset);
	virtual void onKeyDown(int keyCode);
	virtual void onKeyUp(int keyCode);
    virtual void onLink();
    virtual void onUnlink();
    virtual void onFrameChanged();
    virtual void onProjectionChanged();
    virtual void onPause();
    virtual void onResume();
    virtual void onBackKey();
    virtual void onCustomEvent(int op, void* param, bool& interrupt);
    virtual float keyboardScrollDegree(float bottom) const;
	virtual bool touchContains(const CSVector2& p) const;
private:
    void validateLinks();
    bool clearLinks();
    
    void notifyLink();
    void notifyUnlink();
    void notifyFrameChanged();
    void notifyProjectionChanged();
    void notifyTouchesView(const CSArray<CSTouch>* touches, bool& interrupt);
	void notifyPause();
    void notifyResume();
public:
    bool customEvent(int op, void* param);
    
    void commitTouchesBegan();
    bool commitTouchesMoved();
    void commitTouchesEnded();
    void commitTouchesCancelled();

    void beginTouch(const CSTouch* touch, bool event = true);
    void cancelTouches(bool event = true);
    void cancelChildrenTouches(bool event = true);
    
	virtual bool timeout(CSLayerVisible visible);
    
    inline uint timeoutSequence() const {
        return _timeoutSequence;
    }
    
    inline void resetTimeoutSequence() {
        _timeoutSequence = 0;
    }
    
    inline uint timeoutFixedRate() const {
        return _timeoutFixedRate;
    }
    
    inline void setTimeoutFixedRate(bool fixedRate) {
        _timeoutFixedRate = fixedRate;
    }
    
    inline bool touchRefresh() const {
        return _touchRefresh;
    }
    
    inline void setTouchRefresh(bool touchRefresh) {
        _touchRefresh = touchRefresh;
    }
	
	void clip(CSGraphics* graphics);

protected:
	void commitDraw(CSGraphics* graphics);
	void commitDrawCover(CSGraphics* graphics);
public:
	virtual void draw(CSGraphics* graphics);
    
    virtual CSLayerTouchReturn layersOnTouch(const CSTouch* touch, CSArray<CSLayer>* targetLayers, bool began);
    
    bool transitionForward(CSGraphics* graphics);
    bool transitionBackward(CSGraphics* graphics);
    
    void removeFromParent(bool transition);
    void addToParent();
    
    // CSLayerContainer
    inline const CSArray<CSLayer, 1, false>* layers() const {
        return _layers;
    }
    
    int layerCount() const;
    
    inline CSView* view() const override {
        return _parent ? _parent->view() : NULL;
    }
    
    inline void refresh() override {
        if (_parent) {
            _parent->refresh();
        }
    }
    
    void clearLayers(bool transition);
    virtual bool addLayer(CSLayer* layer);
    virtual bool insertLayer(uint index, CSLayer* layer);
    void clearLayers(int key, bool transition, bool inclusive, bool masking = false);
    CSArray<CSLayer, 1, false>* findLayers(int key, bool masking = false);
    void findLayers(int key, CSArray<CSLayer, 1, false>* outArray, bool masking = false);
    CSLayer* findLayer(int key, bool mask = false);
    CSLayer* findParent(int key, bool mask = false) override;
    virtual void convertToLocalSpace(CSVector2* point) const override;
    virtual void convertToParentSpace(CSVector2* point, const CSLayerContainer* parent) const override;
    virtual void convertToParentSpace(CSVector2* point, int key, bool masking = false) const override;
    virtual void convertToViewSpace(CSVector2* point) const override;
    
    //=====================
    
    inline bool enabled() const {
        return _enabled;
    }
    
    inline virtual void setEnabled(bool enabled) {
        if (!enabled) {
            cancelTouches();
            cancelChildrenTouches();
        }
        _enabled = enabled;
    }
    
    inline int key() const {
        return _key;
    }
    
    inline void setKey(int key) {
        _key = key;
    }
    
    inline CSLayerContainer* parent() const override {
        return _parent;
    }
    
    inline CSLayerState state() const {
        return _state;
    }
    inline const CSArray<const CSTouch>* touches() const {
        return _touches;
    }

    inline const CSTouch* touch() const {
        return _touches->lastObject();
    }

    inline bool isTouching() const {
        return _touches->count() > 0;
    }

    inline void setTouchInherit(bool touchInherit) {
        _touchInherit = touchInherit;
    }
    
    inline bool touchInherit() const {
        return _touchInherit;
    }
    
    inline void setTouchCarry(bool touchCarry) {
        _touchCarry = touchCarry;
    }
    
    inline bool touchCarry() const {
        return _touchCarry;
    }
    
    inline void setTouchIn(bool touchIn) {
        _touchIn = touchIn;
    }
    
    inline bool touchIn() const {
        return _touchIn;
    }
    
    inline void setTouchOut(bool touchOut) {
        _touchOut = touchOut;
    }
    
    inline bool touchOut() const {
        return _touchOut;
    }
    
    inline void setTouchMulti(bool multiTouch) {
        _touchMulti = multiTouch;
    }
    
    inline bool touchMulti() const {
        return _touchMulti;
    }

	inline void setTouchArea(CSLayerTouchArea touchArea) {
		_touchArea = touchArea;
	}

	inline CSLayerTouchArea touchArea() const {
		return _touchArea;
	}
    
    inline void setTransition(uint transition) {
        _transition = transition;
    }
    
    inline uint transition() const override {
        return _transition;
    }
    
    inline void setTransitionDuration(float transitionDuration) {
        _transitionDuration = transitionDuration;
    }
    
    inline float transitionDuration() const {
        return _transitionDuration;
    }
    
    inline void setTransitionAccelation(float transitionAccelation) {
        _transitionAccelation = transitionAccelation;
    }
    
    inline float transitionAccelation() const {
        return _transitionAccelation;
    }
    
    inline void setTransitionLatency(float transitionLatency) {
        _transitionLatency = transitionLatency;
    }
    
    inline float transitionLatency() const {
        return _transitionLatency;
    }
    
    inline float transitionProgress() const override {
        return _transitionProgress;
    }
    
    inline float transitionWeight() const {
        return _transitionWeight;
    }
    
    inline void setTransitionWeight(float transitionWeight) {
        _transitionWeight = transitionWeight;
    }
    
    inline void setTimeoutInterval(float timeoutInterval) {
        _timeoutInterval = timeoutInterval;
    }
    
    inline float timeoutInterval() const {
        return _timeoutInterval;
    }
    
    inline void setCovered(bool covered) {
        _covered = covered;
    }
    
    inline bool covered() const {
        return _covered;
    }
    
    inline void setOrder(uint order) {
        _order = order;
    }
    
    inline uint order() const {
        return _order;
    }
    
    void setFrame(const CSRect& frame);
    void setFrame(const CSRect& frame, float interval);
    inline const CSRect& frame() const {
        return _frame;
    }
    inline const CSRect& nextFrame() const {
        return _nextFrame;
    }
    inline const CSRect& prevFrame() const {
        return _prevFrame;
    }
    inline CSRect bounds() const {
        return CSRect(0, 0, _frame.size.width, _frame.size.height);
    }
    inline const CSSize& size() const {
        return _frame.size;
    }
    inline float width() const {
        return _frame.size.width;
    }
    inline float height() const {
        return _frame.size.height;
    }
    inline float center() const {
        return _frame.size.width * 0.5f;
    }
    inline float middle() const {
        return _frame.size.height * 0.5f;
    }
    inline CSVector2 centerMiddle() const {
        return CSVector2(_frame.size.width * 0.5f, _frame.size.height * 0.5f);
    }
    float contentWidth() const;
    float contentHeight() const;
    
    inline void setContentWidth(float contentWidth) {
        _contentWidth = contentWidth;
    }
    inline void setContentHeight(float contentHeight) {
        _contentHeight = contentHeight;
    }
    
    inline CSSize contentSize() const {
        return CSSize(contentWidth(), contentHeight());
    }
    
    inline void setSpacing(float spacing) {
        _spacing = spacing;
    }
    
    inline float spacing() const {
        return _spacing;
    }
    
    inline void setLayout(CSLayerLayout layout) {
        _layout = layout;
    }
    
    inline CSLayerLayout layout() const {
        return _layout;
    }
    
    inline void setKeyboardScroll(CSLayerKeyboardScroll keyboardScroll) {
        _keyboardScroll = keyboardScroll;
    }
    
    inline CSLayerKeyboardScroll keyboardScroll() const {
        return _keyboardScroll;
    }
    
	template <class T>
	struct TagCheck {
	};

	inline void clearTag() {
        release(_tag);
        _tagname = NULL;
    }
    
    template <class T>
    inline void setTag(T* tag) {
        if (_tag != tag) {
            _tagname = typeid(TagCheck<T>).name();
            release(_tag);
            _tag = retain(tag);
        }
    }
    
    template <class T>
    inline T* tag(bool assertType = true) const {
        bool checkTag = !_tagname || strcmp(typeid(TagCheck<T>).name(), _tagname) == 0;
        CSAssert(!assertType || checkTag, "invalid tag:%s, %s", typeid(TagCheck<T>).name(), _tagname);
        return checkTag ? (T*)_tag : NULL;
    }
    
    inline void setTagAsInt(int tag) {
        setTag(CSInteger::value(tag));
    }
    
    inline int tagAsInt() const {
        CSInteger* i = tag<CSInteger>();
        return i ? (int) * i : 0;
    }

	inline void clearSubtag() {
		release(_subtag);
		_subtagname = NULL;
	}

	template <class T>
	inline void setSubtag(T* subtag) {
		if (_subtag != subtag) {
			_subtagname = typeid(TagCheck<T>).name();
			release(_subtag);
			_subtag = retain(subtag);
		}
	}

	template <class T>
	inline T* subtag(bool assertType = true) const {
		bool checkTag = !_subtagname || strcmp(typeid(TagCheck<T>).name(), _subtagname) == 0;
		CSAssert(!assertType || checkTag, "invalid subtag:%s, %s", typeid(TagCheck<T>).name(), _subtagname);
		return checkTag ? (T*)_subtag : NULL;
	}

	inline void setSubtagAsInt(int subtag) {
		setSubtag(CSInteger::value(subtag));
	}

	inline int subtagAsInt() const {
		CSInteger* i = subtag<CSInteger>();
		return i ? (int)* i : 0;
	}
	
    inline void setColor(const CSColor& color) {
        if (_color != color) {
            _color = color;
            refresh();
        }
    }
    
    inline const CSColor& color() const {
        return _color;
    }
    
    void setOwner(CSLayer* owner);
};

#endif /* defined(__CDK__CSLayer__) */
