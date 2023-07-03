#ifndef LobbyLayer_h
#define LobbyLayer_h

#include "Animation.h"

class LobbyLayer : public CSLayer {
private:
	Animation* _animation;
	Animation* _battleAnimation;
public:
	LobbyLayer();
private:
	~LobbyLayer();
public:
	static inline LobbyLayer* layer() {
		return autorelease(new LobbyLayer());
	}
private:
	void onLink() override;
	void onUnlink() override;
	void onBackKey() override;
	void onTimeout() override;
	void onDraw(CSGraphics* graphics) override;
	
	void onDrawOption(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedOption(CSLayer* layer);
	void onDrawMail(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedMail(CSLayer* layer);

	void onDrawCommander(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedCommander(CSLayer* layer);
	void onDrawUnit(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedUnit(CSLayer* layer);
	void onDrawCommunity(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedCommunity(CSLayer* layer);
	void onDrawShop(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedShop(CSLayer* layer);

	void onDrawBattle(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedBattle(CSLayer* layer);
};

#endif /* LobbyLayer_h */
