#ifndef DeckLayer_h
#define DeckLayer_h

#include "GatewayDelegate.h"

class DeckLayer : public CSLayer, public PVPGatewayDelegate {
private:
	CSScrollPane* _cardPane;
	float _singlePlayWaiting;
public:
	DeckLayer();
private:
	~DeckLayer() = default;
public:
	static inline DeckLayer* layer() {
		return autorelease(new DeckLayer());
	}
private:
	void onLink() override;
	void onUnlink() override;
	void onBackKey() override;
	void onTimeout() override;
	void onDraw(CSGraphics* graphics) override;

	void onDrawPrev(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedPrev(CSLayer* layer);

	void onDrawCard(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedCard(CSLayer* layer);
	
	void onDrawEquippedCard(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedEquippedCard(CSLayer* layer);

	void onDrawStart(CSLayer* layer, CSGraphics* graphics);
	void onTouchesEndedStart(CSLayer* layer);

	void onPVPConnect() override;
	void onPVPShutdown() override;
	void onPVPLogin() override;
	void onPVPPartyCreate() override;
	void onPVPMatchStart() override;

	void resetCards();
};

#endif /* DeckLayer_h */
