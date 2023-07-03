#define AccountImpl

#include "DeckLayer.h"

#include "PVPGateway.h"

#include "WaitingLayer.h"
#include "FadeMessageLayer.h"
#include "MessageLayer.h"

#include "GameLayer.h"

static constexpr float SinglePlayWaiting = 10;

DeckLayer::DeckLayer() {
	setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
	setTransition(CSLayerTransitionFade);
	setCovered(true);

	CSLayer* prevLayer = CSLayer::layer();
	prevLayer->setFrame(CSRect(0, 0, 150, 70));
	prevLayer->OnDraw.add(this, &DeckLayer::onDrawPrev);
	prevLayer->OnTouchesEnded.add(this, &DeckLayer::onTouchesEndedPrev);
	addLayer(prevLayer);

	_cardPane = CSScrollPane::scrollPane();
	_cardPane->setFrame(CSRect(25, 130, ProjectionWidth - 50, ProjectionHeight - 570));
	_cardPane->scroll.setBarColor(CSColor::TranslucentWhite);
	_cardPane->scroll.setBarThickness(5);
	addLayer(_cardPane);

	resetCards();
	
	for (int i = 0; i < 2; i++) {
		for (int j = 0; j < 4; j++) {
			CSButton* cardLayer = CSButton::button();
			cardLayer->setTagAsInt(i * 4 + j);
			cardLayer->setFrame(CSRect(ProjectionBaseX + 145 + j * 110, ProjectionHeight - 365 + i * 110, 100, 100));
			cardLayer->OnDraw.add(this, &DeckLayer::onDrawEquippedCard);
			cardLayer->OnTouchesEnded.add(this, &DeckLayer::onTouchesEndedEquippedCard);
			addLayer(cardLayer);
		}
	}

	CSButton* startButton = CSButton::button();
	startButton->setFrame(CSRect(ProjectionBaseX + 186, ProjectionHeight - 118, 348, 98));
	startButton->OnDraw.add(this, &DeckLayer::onDrawStart);
	startButton->OnTouchesEnded.add(this, &DeckLayer::onTouchesEndedStart);
	addLayer(startButton);
}

void DeckLayer::onLink() {
	PVPGateway::sharedGateway()->addDelegate(this);

}

void DeckLayer::onUnlink() {
	PVPGateway::sharedGateway()->removeDelegate(this);
}

void DeckLayer::onBackKey() {
	removeFromParent(true);
}

void DeckLayer::onTimeout() {
	const Party* party = Account::sharedAccount()->party();

	if (party && party->state == PartyStateReady && _singlePlayWaiting > 0) {
		_singlePlayWaiting -= timeoutInterval();

		if (_singlePlayWaiting <= 0) {
			_singlePlayWaiting = 0;

			PVPGateway::sharedGateway()->requestPartyExit();

			static const MapIndex mapIndices[] = {
				MapIndex(0, 0, 1, 0),
				MapIndex(0, 1, 1, 0)
			};

			GameLayer* gameLayer = GameLayer::layerForOfflinePlay(GameOfflineModeTest, mapIndices[randInt(0, countof(mapIndices))], Account::sharedAccount(), Player::playerForTraining(), 1);

			if (gameLayer) {
				view()->clearLayers(false);
				view()->addLayer(gameLayer);
			}
			else {
				FadeMessageLayer::show(view(), Asset::sharedAsset()->string(MessageSetError, MessageErrorDataFail));
			}
		}
	}
}

void DeckLayer::onDraw(CSGraphics* graphics) {
	graphics->setColor(CSColor(60, 110, 160, 255));
	graphics->drawRect(bounds(), true);
	graphics->setColor(CSColor(15, 30, 50, 255));
	graphics->drawRect(CSRect(0, 0, ProjectionWidth, 60), true);

	graphics->setColor(CSColor::White);
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_0), CSRect(20, 90, ProjectionWidth - 40, ProjectionHeight - 515));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameControlBorder), CSRect(ProjectionBaseX + 125, ProjectionHeight - 405, 470, 265));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_1), CSRect(ProjectionBaseX + 210, 90, 305, 35));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_1), CSRect(ProjectionBaseX + 210, ProjectionHeight - 405, 305, 35));

	graphics->setStrokeWidth(2);
	graphics->setFont(Asset::sharedAsset()->boldSmallFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameCardsCollected), CSVector2(center(), 103), CSAlignCenterMiddle);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameBattleDeck), CSVector2(center(), ProjectionHeight - 392), CSAlignCenterMiddle);
}

void DeckLayer::onDrawPrev(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGamePrevButton), CSVector3::Zero);
}

void DeckLayer::onTouchesEndedPrev(CSLayer* layer) {
	removeFromParent(true);
}

void DeckLayer::onDrawCard(CSLayer* layer, CSGraphics* graphics) {
	const Card* card = layer->tag<const Card>();

	const SkinData* skin = card->skinData();

	bool equipped = Account::sharedAccount()->cards()->containsObjectIdenticalTo(card);

	if (equipped) graphics->setColor(CSColor::Gray);

	if (skin) {
		graphics->drawImage(Asset::sharedAsset()->image(skin->smallIcon), layer->centerMiddle(), CSAlignCenterMiddle);
	}
	else {
		graphics->drawRect(layer->bounds().adjustRect(-5), false);

		graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_0), layer->centerMiddle(), CSAlignCenterMiddle);
	}
}

void DeckLayer::onTouchesEndedCard(CSLayer* layer) {
	const Card* card = layer->tag<const Card>();

	Account* account = Account::sharedAccount();

	int index = account->cards()->indexOfObjectIdenticalTo(card);

	if (index >= 0) account->cards()->removeObjectAtIndex(index);
	else if (account->cards()->count() < CardSlots) account->cards()->addObject(const_cast<Card*>(card));
	else return;

	refresh();

	PVPGateway::sharedGateway()->requestUpdateProfile();
}

void DeckLayer::onDrawEquippedCard(CSLayer* layer, CSGraphics* graphics) {
	int tag = layer->tagAsInt();

	const Account* account = Account::sharedAccount();

	if (tag < account->cards()->count()) {
		const Card* card = account->cards()->objectAtIndex(tag);
		const SkinData* skin = card->skinData();
		if (skin) {
			graphics->drawImage(Asset::sharedAsset()->image(skin->smallIcon), layer->centerMiddle(), CSAlignCenterMiddle);
		}
		else {
			graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_0), layer->centerMiddle(), CSAlignCenterMiddle);
		}
	}
	else {
		graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameCardBorder_0), layer->centerMiddle(), CSAlignCenterMiddle);
	}
}

void DeckLayer::onTouchesEndedEquippedCard(CSLayer* layer) {
	int tag = layer->tagAsInt();

	Account* account = Account::sharedAccount();

	if (tag < account->cards()->count()) {
		account->cards()->removeObjectAtIndex(tag);

		refresh();

		PVPGateway::sharedGateway()->requestUpdateProfile();
	}
}

void DeckLayer::onDrawStart(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameStartButton), layer->centerMiddle(), CSAlignCenterMiddle);

	graphics->setStrokeWidth(2);
	graphics->setFont(Asset::sharedAsset()->boldExtraLargeFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameStartButton), CSVector2(layer->center(), layer->middle() - 5), CSAlignCenterMiddle);
}

void DeckLayer::onTouchesEndedStart(CSLayer* layer) {
	//======================================================================
	//TODO
	/*
	static const MapIndex mapIndices[] = {
		MapIndex(0, 0, 1, 0),
		MapIndex(0, 1, 1, 0)
	};

	GameLayer* gameLayer = GameLayer::layerForOfflinePlay(GameOfflineModeTest, mapIndices[randInt(0, countof(mapIndices))], Account::sharedAccount(), Player::playerForTraining(), 1);

	if (gameLayer) {
		view()->clearLayers(false);
		view()->addLayer(gameLayer);
	}
	else {
		FadeMessageLayer::show(view(), Asset::sharedAsset()->string(MessageSetError, MessageErrorDataFail));
	}
	*/
	//======================================================================
	WaitingLayer::show(this);

	if (PVPGateway::sharedGateway()->isConnected()) {
		PVPGateway::sharedGateway()->requestPartyCreate(GameOnlineModeTest, 1, 1, 2, NULL);
	}
	else {
		PVPGateway::sharedGateway()->requestConnect();
	}
}

void DeckLayer::onPVPConnect() {
	PVPGateway::sharedGateway()->requestLogin(false);
}

void DeckLayer::onPVPShutdown() {
	WaitingLayer::hide(this);

	FadeMessageLayer::show(view(), Asset::sharedAsset()->string(MessageSetError, MessageErrorNetworkFail));
}

void DeckLayer::onPVPLogin() {
	PVPGateway::sharedGateway()->requestPartyCreate(GameOnlineModeTest, 1, 1, 2, NULL);
}

void DeckLayer::onPVPPartyCreate() {
	PVPGateway::sharedGateway()->requestPartyStart(true);

	_singlePlayWaiting = SinglePlayWaiting;
}

void DeckLayer::onPVPMatchStart() {
	GameLayer* gameLayer = GameLayer::layerForOnlinePlay(GameOnlineModeTest, false);

	if (gameLayer) {
		view()->clearLayers(false);
		view()->addLayer(gameLayer);
	}
	else {
		WaitingLayer::hide(this);

		FadeMessageLayer::show(view(), Asset::sharedAsset()->string(MessageSetError, MessageErrorDataFail));
	}
}

void DeckLayer::resetCards() {
	_cardPane->clearLayers(false);

	float bx = (_cardPane->width() - ((int)(_cardPane->width() / 110) * 110)) * 0.5f;

	CSVector2 pos(bx, 0);
	foreach(const Card*, card, Account::sharedAccount()->allCards()) {
		CSButton* cardLayer = CSButton::button();
		cardLayer->setTag(card);
		cardLayer->setFrame(CSRect(pos.x + 5, pos.y + 5, 100, 100));
		cardLayer->OnDraw.add(this, &DeckLayer::onDrawCard);
		cardLayer->OnTouchesEnded.add(this, &DeckLayer::onTouchesEndedCard);
		_cardPane->addLayer(cardLayer);

		pos.x += 110;
		if (pos.x + 110 > _cardPane->width()) {
			pos.x = bx;
			pos.y += 110;
		}
	}
}
