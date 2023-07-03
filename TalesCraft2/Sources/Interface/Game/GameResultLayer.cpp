#include "GameResultLayer.h"

#include "LobbyLayer.h"

GameResultLayer::GameResultLayer(GameResult result) {
	setFrame(CSRect(0, 0, ProjectionWidth, ProjectionHeight));
	if (result == GameResultVictory) _animation = new Animation(AnimationSetGame, AnimationGameWin);
	else _animation = new Animation(AnimationSetGame, AnimationGameLose);
}

GameResultLayer::~GameResultLayer() {
	_animation->release();
}

void GameResultLayer::onTimeout() {
	_animation->update(timeoutInterval());
	refresh();
}

void GameResultLayer::onDraw(CSGraphics* graphics) {
	_animation->draw(graphics, centerMiddle());
}

void GameResultLayer::onTouchesEnded() {
	if (!_animation->remaining(CSAnimationDurationMin)) {
		view()->clearLayers(false);
		view()->addLayer(LobbyLayer::layer());
	}
}
