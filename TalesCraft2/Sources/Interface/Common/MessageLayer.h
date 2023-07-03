//
//  MessageLayer.h
//  TalesCraft2
//
//  Created by Jae Hong Kim on 2014. 11. 18..
//  Copyright (c) 2014년 brgames. All rights reserved.
//

#ifndef MessageLayer_h
#define MessageLayer_h

#include "Application.h"

enum MessageLayerType {
    MessageLayerTypeConfirm,
    MessageLayerTypeConfirmCancel,
    MessageLayerTypeDenyCancel,
    MessageLayerTypeRemoveCancel,
    MessageLayerTypeSaveCancel,
    MessageLayerTypeMoveCancel,
    MessageLayerTypeReconnectCancel
};

class MessageLayer : public CSLayer {
public:
	CSHandler<CSLayer*, bool> OnConfirm;
private:
    const CSString* _message;
    const CSString* _caption;
    MessageLayerType _type;
	bool _shortHeight;
	bool _shouldConfirm;
public:
    MessageLayer(const CSString* message, const CSString* caption = NULL, MessageLayerType type = MessageLayerTypeConfirm);
private:
    ~MessageLayer();
public:
    static inline MessageLayer* layer(const CSString *message, const CSString* caption = NULL, MessageLayerType type = MessageLayerTypeConfirm) {
        return autorelease(new MessageLayer(message, caption, type));
    }
	inline bool shouldConfirm() const {
		return _shouldConfirm;
	}
	inline void setShouldConfirm(bool shouldConfirm) {
		_shouldConfirm = shouldConfirm;
	}
private:
    void onBackKey() override;
    void onDraw(CSGraphics* graphics) override;
	void onTouchesCancelled() override;
    
    void onDrawDeny(CSLayer* layer, CSGraphics* graphics);
    void onDrawRemove(CSLayer* layer, CSGraphics* graphics);
    void onDrawSave(CSLayer* layer, CSGraphics* graphics);
    void onDrawMove(CSLayer* layer, CSGraphics* graphics);
    void onDrawReconnect(CSLayer* layer, CSGraphics* graphics);
    
	void onDrawConfirm(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedConfirm(CSLayer* layer);
    void onDrawCancel(CSLayer* layer, CSGraphics* graphics);
    void onTouchesEndedCancel(CSLayer* layer);
    
    void onDrawClose(CSLayer* layer, CSGraphics* graphics);    
};

#endif /* MessageLayer_h */
