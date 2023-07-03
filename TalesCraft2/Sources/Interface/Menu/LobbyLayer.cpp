#include "LobbyLayer.h"

#include "Asset.h"
#include "Sound.h"

#include "MessageLayer.h"
#include "DeckLayer.h"

LobbyLayer::LobbyLayer() {
	setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
	setTransition(CSLayerTransitionFade);

	_animation = new Animation(AnimationSetGame, AnimationGameLobby);
	_battleAnimation = new Animation(AnimationSetGame, AnimationGameBattleButton);

	CSButton* optionButton = CSButton::button();
	optionButton->setFrame(CSRect(ProjectionWidth - 70, 0, 70, 70));
	optionButton->OnDraw.add(this, &LobbyLayer::onDrawOption);
	optionButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedOption);
	addLayer(optionButton);

	CSButton* mailButton = CSButton::button();
	mailButton->setFrame(CSRect(ProjectionWidth - 140, 0, 70, 70));
	mailButton->OnDraw.add(this, &LobbyLayer::onDrawMail);
	mailButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedMail);
	addLayer(mailButton);

	CSButton* commanderButton = CSButton::button();
	commanderButton->setFrame(CSRect(ProjectionBaseX + 30, ProjectionHeight - 175, 110, 110));
	commanderButton->OnDraw.add(this, &LobbyLayer::onDrawCommander);
	commanderButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedCommander);
	addLayer(commanderButton);

	CSButton* unitButton = CSButton::button();
	unitButton->setFrame(CSRect(ProjectionBaseX + 150, ProjectionHeight - 175, 110, 110));
	unitButton->OnDraw.add(this, &LobbyLayer::onDrawUnit);
	unitButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedUnit);
	addLayer(unitButton);

	CSButton* communityButton = CSButton::button();
	communityButton->setFrame(CSRect(ProjectionBaseX + 460, ProjectionHeight - 175, 110, 110));
	communityButton->OnDraw.add(this, &LobbyLayer::onDrawCommunity);
	communityButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedCommunity);
	addLayer(communityButton);

	CSButton* shopButton = CSButton::button();
	shopButton->setFrame(CSRect(ProjectionBaseX + 580, ProjectionHeight - 175, 110, 110));
	shopButton->OnDraw.add(this, &LobbyLayer::onDrawShop);
	shopButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedShop);
	addLayer(shopButton);


	CSButton* battleButton = CSButton::button();
	battleButton->setFrame(CSRect(ProjectionWidth / 2 - 80, ProjectionHeight - 200, 160, 160));
	battleButton->OnDraw.add(this, &LobbyLayer::onDrawBattle);
	battleButton->OnTouchesEnded.add(this, &LobbyLayer::onTouchesEndedBattle);
	addLayer(battleButton);
}

LobbyLayer::~LobbyLayer() {
	_animation->release();
	_battleAnimation->release();
}

void LobbyLayer::onLink() {
	Sound::play(SoundControlLobbyBgm, SoundBgmLobby, SoundPlayBgm);
}

void LobbyLayer::onUnlink() {
	Sound::stop(SoundControlLobbyBgm);
}

void LobbyLayer::onBackKey() {
	MessageLayer* popupLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonFinishApplication), NULL, MessageLayerTypeConfirmCancel);
	popupLayer->setOwner(this);
	popupLayer->OnConfirm.add([](CSLayer* layer, bool yes) {
		if (yes) {
			CSApplication::sharedApplication()->finish();
		}
	});
	view()->addLayerAsPopup(popupLayer);
}

void LobbyLayer::onTimeout() {
	_animation->update(timeoutInterval());
	_battleAnimation->update(timeoutInterval());

	refresh();
}

void LobbyLayer::onDraw(CSGraphics* graphics) {
	_animation->draw(graphics, centerMiddle());

	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyBorder), CSRect(center(), 0, center(), 70));
	graphics->scale(CSVector3(-1, 1, 1));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyBorder), CSRect(-center(), 0, center(), 70));
	graphics->scale(CSVector3(-1, -1, 1));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyBorder), CSRect(center(), -ProjectionHeight, center(), 60));
	graphics->scale(CSVector3(-1, 1, 1));
	graphics->drawStretchImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyBorder), CSRect(-center(), -ProjectionHeight, center(), 60));
	graphics->scale(CSVector3(-1, -1, 1));

	const Account* account = Account::sharedAccount();

	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyProfile), CSVector2(80, 80), CSAlignCenterMiddle);

	graphics->setFont(Asset::sharedAsset()->boldSmallFont);
	graphics->drawString(account->name, CSVector2(155, 50), CSAlignMiddle);

	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyExpGage_0), CSRect(155, 75, 140, 5));
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyExpGage_1), CSRect(155, 75, 140 * 0.5f, 5));

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->setStrokeWidth(1);
	graphics->drawString("50 / 100", CSVector2(155, 90), CSAlignMiddle);
	graphics->setStrokeWidth(0);

	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyLvBorder), CSVector2(center(), 100), CSAlignCenterMiddle);
	
	graphics->setFont(Asset::sharedAsset()->boldMediumFont);
	graphics->drawDigit(account->level, false, CSVector2(center(), 110), CSAlignCenterMiddle);

	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyAssetBorder_0), CSVector2(width() - 10, 80), CSAlignRight);
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyAssetBorder_1), CSVector2(width() - 10, 125), CSAlignRight);
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyAssetBorder_2), CSVector2(width() - 10, 170), CSAlignRight);
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyAssetBorder_3), CSVector2(width() - 10, 215), CSAlignRight);

	graphics->setFont(Asset::sharedAsset()->boldSmallFont);
	graphics->drawString("0", CSVector2(width() - 25, 98), CSAlignRightMiddle);
	graphics->drawString("0", CSVector2(width() - 25, 143), CSAlignRightMiddle);
	graphics->drawString("0", CSVector2(width() - 25, 188), CSAlignRightMiddle);
	graphics->drawString("0", CSVector2(width() - 25, 233), CSAlignRightMiddle);
}

void LobbyLayer::onDrawOption(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyOptionButton), layer->centerMiddle(), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedOption(CSLayer* layer) {
	
}

void LobbyLayer::onDrawMail(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyMailButton), layer->centerMiddle(), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedMail(CSLayer* layer) {
	
}

void LobbyLayer::onDrawCommander(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyCommanderButton), layer->centerMiddle(), CSAlignCenterMiddle);

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameCommander), CSVector2(layer->center(), 130), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedCommander(CSLayer* layer) {

}

void LobbyLayer::onDrawUnit(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyUnitButton), layer->centerMiddle(), CSAlignCenterMiddle);

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameUnit), CSVector2(layer->center(), 130), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedUnit(CSLayer* layer) {

}

void LobbyLayer::onDrawCommunity(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyCommunityButton), layer->centerMiddle(), CSAlignCenterMiddle);

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameCommunity), CSVector2(layer->center(), 130), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedCommunity(CSLayer* layer) {

}

void LobbyLayer::onDrawShop(CSLayer* layer, CSGraphics* graphics) {
	graphics->drawImage(Asset::sharedAsset()->image(ImageSetGame, ImageGameLobbyShopButton), layer->centerMiddle(), CSAlignCenterMiddle);

	graphics->setFont(Asset::sharedAsset()->boldExtraSmallFont);
	graphics->drawString(Asset::sharedAsset()->string(MessageSetGame, MessageGameShop), CSVector2(layer->center(), 130), CSAlignCenterMiddle);
}

void LobbyLayer::onTouchesEndedShop(CSLayer* layer) {

}

void LobbyLayer::onDrawBattle(CSLayer* layer, CSGraphics* graphics) {
	_battleAnimation->draw(graphics, layer->centerMiddle());
}

void LobbyLayer::onTouchesEndedBattle(CSLayer* layer) {
	view()->addLayer(DeckLayer::layer());
}
