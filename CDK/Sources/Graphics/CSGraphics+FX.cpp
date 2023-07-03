//
//  CSGraphics+FX.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2018. 1. 26..
//  Copyright © 2018년 brgames. All rights reserved.
//
#define CDK_IMPL

#include "CSGraphics.h"

#include "CSGraphicsImpl.h"

void CSGraphics::shockwave(const CSVector3& center, float range, float thickness, float progress) {
	commit();

	CSGraphicsImpl::sharedImpl()->shockwaveShader()->draw(this, center, range, thickness, progress, _dirtyOffscreen);

	_dirtyOffscreen = false;
}

void CSGraphics::lens(const CSVector3& center, float range, bool convex) {
	commit();

	CSGraphicsImpl::sharedImpl()->lensShader()->draw(this, center, range, convex, _dirtyOffscreen);

	_dirtyOffscreen = false;
}

void CSGraphics::swirl(const CSVector3& center, float range, float angle) {
	commit();

	CSGraphicsImpl::sharedImpl()->swirlShader()->draw(this, center, range, angle, _dirtyOffscreen);

	_dirtyOffscreen = false;
}

void CSGraphics::pixellate(const CSZRect& rect, float fraction) {
	commit();

	CSGraphicsImpl::sharedImpl()->pixellateShader()->draw(this, rect, fraction);

	_dirtyOffscreen = true;
}

void CSGraphics::kuwahara(const CSZRect& rect, int radius) {
	commit();

	CSGraphicsImpl::sharedImpl()->kuwaharaShader()->draw(this, rect, radius);

	_dirtyOffscreen = true;
}

void CSGraphics::blur(const CSZRect& rect, float weight) {
	commit();

	CSGraphicsImpl::sharedImpl()->blurShader()->draw(this, rect, weight);

	_dirtyOffscreen = true;
}

void CSGraphics::blur(const CSZRect& rect, const CSVector2& dir) {
	commit();

	CSGraphicsImpl::sharedImpl()->blurShader()->draw(this, rect, dir);

	_dirtyOffscreen = true;
}

void CSGraphics::blur(const CSZRect& rect, const CSVector3& center, float range) {
	commit();

	CSGraphicsImpl::sharedImpl()->blurShader()->draw(this, rect, center, range);

	_dirtyOffscreen = true;
}

