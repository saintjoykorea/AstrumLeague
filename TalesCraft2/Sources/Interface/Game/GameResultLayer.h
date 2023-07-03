#ifndef GameResultLayer_h
#define GameResultLayer_h

#include "Asset.h"

class GameResultLayer : public CSLayer {
private:
	Animation* _animation;
public:
	GameResultLayer(GameResult result);
private:
	~GameResultLayer();
public:
	static inline GameResultLayer* layer(GameResult result) {
		return autorelease(new GameResultLayer(result));
	}
private:
	void onTimeout() override;
	void onDraw(CSGraphics* graphics) override;
	void onTouchesEnded() override;
};

#endif /* GameResultLayer_h */
