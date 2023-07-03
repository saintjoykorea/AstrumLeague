//
//  CSLayerTransition.h
//  CDK
//
//  Created by 김찬 on 12. 8. 10..
//  Copyright (c) 2012년 brgames. All rights reserved.
//

#ifndef __CDK__CSLayerTransition__
#define __CDK__CSLayerTransition__

#include "CSLayer.h"

static bool leftTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        graphics->translate(CSVector3((1.0f - progress) * layer->width(), 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool leftTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        graphics->translate(CSVector3(-progress * layer->width(), 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool rightTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        graphics->translate(CSVector3((progress - 1.0f) * layer->width(), 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool rightTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        graphics->translate(CSVector3(progress * layer->width(), 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool upTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        graphics->translate(CSVector3(0.0f, (1.0f - progress) * layer->height(), 0.0f));
        
        return true;
    }
    return false;
}

static bool upTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        graphics->translate(CSVector3(0.0f, -progress * layer->height(), 0.0f));
        
        return true;
    }
    return false;
}

static bool downTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        graphics->translate(CSVector3(0.0f, (progress - 1.0f) * layer->height(), 0.0f));
        
        return true;
    }
    return false;
}

static bool downTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        graphics->translate(CSVector3(0.0f, progress * layer->height(), 0.0f));
        
        return true;
    }
    return false;
}

static bool leftBounceTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        float width = layer->width();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * width;
        
        graphics->translate(CSVector3(width - d, 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool leftBounceTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        float width = layer->width();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * width;
        
        graphics->translate(CSVector3(-d, 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool rightBounceTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        float width = layer->width();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * width;
        
        graphics->translate(CSVector3(d - width, 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool rightBounceTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        float width = layer->width();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * width;
        
        graphics->translate(CSVector3(d, 0.0f, 0.0f));
        
        return true;
    }
    return false;
}

static bool upBounceTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        float height = layer->height();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * height;
        
        graphics->translate(CSVector3(0.0f, height - d, 0.0f));
        
        return true;
    }
    return false;
}

static bool upBounceTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        float height = layer->height();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * height;
        
        graphics->translate(CSVector3(0.0f, -d, 0.0f));
        
        return true;
    }
    return false;
}

static bool downBounceTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        float height = layer->height();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * height;
        
        graphics->translate(CSVector3(0.0f, d - height, 0.0f));
        
        return true;
    }
    return false;
}

static bool downBounceTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        float height = layer->height();
        float a = 2.0f * layer->transitionWeight();
        float d = (-a * progress * progress + (a + 1.0f) * progress) * height;
        
        graphics->translate(CSVector3(0.0f, d, 0.0f));
        
        return true;
    }
    return false;
}

static CSVector2 projectionToScreen(const CSMatrix& transform, const CSVector2& point) {
    float itmp3 = 1.0f / (point.x * transform[3] + point.y * transform[7] + transform[15]);
    float x = itmp3 * (point.x * transform[0] + point.y * transform[4] + transform[12]);
    float y = itmp3 * (point.x * transform[1] + point.y * transform[5] + transform[13]);
    return CSVector2(x, y);
}

static bool hflipTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    float angle = FloatPi * progress;
    float cx = layer->width() / 2;
    
    CSMatrix transform;
    transform = CSMatrix::translation(CSVector3(cx, 0.0f, -cx * CSMath::sin(angle)));
    transform = CSMatrix::rotationY(FloatPi - angle) * transform;
    transform = CSMatrix::translation(CSVector3(-cx, 0.0f, 0.0f)) * transform;
    
    CSMatrix projectionTransform = transform * graphics->world();       //TODO:FIX
    float tsx = projectionToScreen(projectionTransform, CSVector2::Zero).x;
    float tcx = projectionToScreen(projectionTransform, CSVector2(cx, 0)).x;
    if (tsx < tcx) {
        graphics->transform(transform);
        return true;
    }
    return false;
}

static bool hflipTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    float angle = FloatPi * progress;
    float cx = layer->width() / 2;
    
    CSMatrix transform;
    transform = CSMatrix::translation(CSVector3(cx, 0.0f, -cx * CSMath::sin(angle)));
    transform = CSMatrix::rotationY(-angle) * transform;
    transform = CSMatrix::translation(CSVector3(-cx, 0.0f, 0.0f)) * transform;
    
    CSMatrix projectionTransform = transform * graphics->world();       //TODO:FIX
    float tsx = projectionToScreen(projectionTransform, CSVector2::Zero).x;
    float tcx = projectionToScreen(projectionTransform, CSVector2(cx, 0)).x;
    if (tsx < tcx) {
        graphics->transform(transform);
        return true;
    }
    return false;
}

static bool vflipTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    float angle = FloatPi * progress;
    float cy = layer->height() / 2;
    
    CSMatrix transform;
    transform = CSMatrix::translation(CSVector3(0.0f, cy, -cy * CSMath::sin(angle)));
    transform = CSMatrix::rotationX(FloatPi - angle) * transform;
    transform = CSMatrix::translation(CSVector3(0.0f, -cy, 0.0f)) * transform;
    
    CSMatrix projectionTransform = transform * graphics->world();       //TODO:FIX
    float tsy = projectionToScreen(projectionTransform, CSVector2::Zero).y;
    float tcy = projectionToScreen(projectionTransform, CSVector2(0, cy)).y;
    if (tsy < tcy) {
        graphics->transform(transform);
        return true;
    }
    return false;
}

static bool vflipTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    float angle = FloatPi * progress;
    float cy = layer->width() / 2;
    
    CSMatrix transform;
    transform = CSMatrix::translation(CSVector3(0.0f, cy, -cy * CSMath::sin(angle)));
    transform = CSMatrix::rotationX(-angle) * transform;
    transform = CSMatrix::translation(CSVector3(0.0f, -cy, 0.0f)) * transform;
    
    CSMatrix projectionTransform = transform * graphics->world();       //TODO:FIX
    float tsy = projectionToScreen(projectionTransform, CSVector2::Zero).y;
    float tcy = projectionToScreen(projectionTransform, CSVector2(0, cy)).y;
    if (tsy < tcy) {
        graphics->transform(transform);
        return true;
    }
    return false;
}

static bool fadeTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        graphics->setColor(graphics->color() * CSColor(1.0f, 1.0f, 1.0f, progress));
        
        return true;
    }
    return false;
}

static bool fadeTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
		float a = 1.0f - progress;
		graphics->setColor(graphics->color() * CSColor(a, a, a, 1.0f));
        return true;
    }
    return false;
}

static bool hzoomTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(progress, 1.0f, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool vzoomTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f, progress, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool hzoomTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f + progress, 1.0f, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool vzoomTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress < 1.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f, 1.0f + progress, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool hpopTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        int bounce = CSMath::max((int)(layer->transitionDuration() / 0.1f), 1);
        
        float scale = 1.0f + 0.1f * layer->transitionWeight() * (1.0f - progress) * sinf((-0.5f + progress * bounce) * FloatPi);
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(scale, 1.0f, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool vpopTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        int bounce = CSMath::max((int)(layer->transitionDuration() / 0.1f), 1);
        
        float scale = 1.0f + 0.1f * layer->transitionWeight() * (1.0f - progress) * sinf((-0.5f + progress * bounce) * FloatPi);
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f, scale, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool hpopTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    return true;
}

static bool vpopTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    return true;
}

static bool hfallTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f + layer->transitionWeight() * (1.0f - progress), 1.0f, 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool vfallTransitionForward(CSLayer* layer, CSGraphics* graphics) {
    float progress = layer->transitionProgress();
    
    if (progress > 0.0f) {
        CSVector2 centerMiddle = layer->centerMiddle();
        
        graphics->translate(centerMiddle);
        graphics->scale(CSVector3(1.0f, 1.0f + layer->transitionWeight() * (1.0f - progress), 1.0f));
        graphics->translate(-centerMiddle);
        
        return true;
    }
    return false;
}

static bool hfallTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    return true;
}

static bool vfallTransitionBackward(CSLayer* layer, CSGraphics* graphics) {
    return true;
}

typedef bool (*TRANSITION_TRANSFORM)(CSLayer* layer, CSGraphics* graphics);

typedef struct _TransitionMethod {
    TRANSITION_TRANSFORM forward;
    TRANSITION_TRANSFORM backward;
} TransitionMethod;

static const TransitionMethod transitionMethods[] = {
    {   leftTransitionForward,  leftTransitionBackward  },
    {   rightTransitionForward, rightTransitionBackward },
    {   upTransitionForward,    upTransitionBackward    },
    {   downTransitionForward,  downTransitionBackward  },
    {   leftBounceTransitionForward,  leftBounceTransitionBackward  },
    {   rightBounceTransitionForward, rightBounceTransitionBackward },
    {   upBounceTransitionForward,    upBounceTransitionBackward    },
    {   downBounceTransitionForward,  downBounceTransitionBackward  },
    {   hflipTransitionForward,  hflipTransitionBackward  },
    {   vflipTransitionForward,  vflipTransitionBackward  },
    {   fadeTransitionForward,  fadeTransitionBackward  },
    {   hzoomTransitionForward, hzoomTransitionBackward },
    {   vzoomTransitionForward, vzoomTransitionBackward },
    {   hfallTransitionForward, hfallTransitionBackward },
    {   vfallTransitionForward, vfallTransitionBackward },
    {   hpopTransitionForward, hpopTransitionBackward   },
    {   vpopTransitionForward, vpopTransitionBackward   },
};

#define LAYER_TRANSITION_COUNT  (sizeof(transitionMethods) / sizeof(TransitionMethod))

#endif
