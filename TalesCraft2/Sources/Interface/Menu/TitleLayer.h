#ifndef TitleLayer_h
#define TitleLayer_h

#include "Animation.h"

class TitleLayer : public CSLayer {
private:
	Animation* _animation;
public:
	TitleLayer();
private:
	~TitleLayer();
public:
	static inline TitleLayer* layer() {
		return autorelease(new TitleLayer());
	}
private:
	void onLink() override;
	void onUnlink() override;
	void onBackKey() override;
	void onTimeout() override;
	void onDraw(CSGraphics* graphics) override;
	void onTouchesEnded() override;
};

#endif /* TitleLayer_h */
