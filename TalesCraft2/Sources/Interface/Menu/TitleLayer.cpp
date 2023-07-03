#include "TitleLayer.h"

#include "Asset.h"

#include "MessageLayer.h"
#include "LobbyLayer.h"

static constexpr float TitleDuration = 3.0f;

TitleLayer::TitleLayer() {
	setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));

	_animation = new Animation(AnimationSetCommon, AnimationCommonTitle);
}

TitleLayer::~TitleLayer() {
	_animation->release();
}

void TitleLayer::onLink() {
	Sound::play(SoundControlTitleBgm, SoundBgmTitle, SoundPlayBgm);
}

void TitleLayer::onUnlink() {
	Sound::stop(SoundControlTitleBgm);
}

void TitleLayer::onBackKey() {
	MessageLayer* popupLayer = MessageLayer::layer(Asset::sharedAsset()->string(MessageSetCommon, MessageCommonFinishApplication), NULL, MessageLayerTypeConfirmCancel);
	popupLayer->setOwner(this);
	popupLayer->OnConfirm.add([](CSLayer* layer, bool yes) {
		if (yes) {
			CSApplication::sharedApplication()->finish();
		}
	});
	view()->addLayerAsPopup(popupLayer);
}

void TitleLayer::onTimeout() {
	_animation->update(timeoutInterval());

	refresh();
}

void TitleLayer::onDraw(CSGraphics* graphics) {
	_animation->draw(graphics, centerMiddle());
}

void TitleLayer::onTouchesEnded() {
	if (timeoutSequence() * timeoutInterval() >= TitleDuration) {
		view()->clearLayers(false);
		view()->addLayer(LobbyLayer::layer());
	}
}
