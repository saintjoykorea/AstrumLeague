//
//  Camera.h
//  TalesCraft2
//
//  Created by Kim Chan on 2016. 3. 9..
//  Copyright © 2016년 brgames. All rights reserved.
//

#ifndef Camera_h
#define Camera_h

#include "Terrain.h"

class Camera : public CSObject {
private:
    const Terrain* _terrain;
    float _fov;
    float _znear;
    float _zfar;
    float _angle;
    float _distance;
    float _maxZoom;
    byte _padding;
    bool _inverted;
    CSCamera _camera;
    float _zoom;
    float _pickRange;
	CSVector3 _up;
	CSVector3 _forward;
	CSVector3 _target;
    enum {
        StateNone,
        StateMoveScroll,
        StateStopScroll,
        StateMoveZoom,
        StateAuto
    } _state;
    struct {
        CSVector2 pos;
		float posTime;
        float zoom;
		float zoomTime;
    } _delta;
    bool _updated;
public:
    Camera(const Terrain* terrain, CSBuffer* buffer, const CSVector2& wp, bool inv);
private:
    ~Camera();
public:
    static inline Camera* camera(Terrain* terrain, CSBuffer* buffer, const CSVector2& wp, bool inv) {
        return autorelease(new Camera(terrain, buffer, wp, inv));
    }
    inline bool inverted() const {
        return _inverted;
    }
    inline float pickRange() const {
        return _pickRange;
    }
	inline const CSVector3& forward() const {
		return _forward;
	}
	inline const CSVector3& up() const {
		return _up;
	}
private:
	CSVector3 convertToGroundPoint(const CSVector2& wp);

    void applyImpl(const CSVector3& target);
    void apply();
    void update(const CSVector2& scroll, float zoom, float altitude);
    void updateMoveScroll(float delta);
    void updateStopScroll(float delta);
    void updateAuto(float delta);
public:
    void setScroll(const CSVector2& wp);
	void setScroll(const CSVector2& wp, float time);
    void moveScroll(CSVector2 diff);
    void stopScroll(bool interrupt);
    void setZoom(float zoom);
	void setZoom(float zoom, float time);
	void moveZoom(float diff);
    void stopZoom();
	bool update(float delta);
    
    const CSCamera& capture() const;
    void capture(CSCamera& capture) const;
};

#endif /* Camera_h */
