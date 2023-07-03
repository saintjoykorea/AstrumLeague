#ifndef UnitBaseGame_h
#define UnitBaseGame_h

#include "UnitBase.h"

#include "Force.h"

class Unit;
class Spawn;

class UnitBaseGame : public UnitBase {
protected:
	Force* _force;
protected:
	inline UnitBaseGame(Force* force, const UnitIndex& index, int level, int skin) : UnitBase(index, level, skin), _force(force) {

	}
	inline virtual ~UnitBaseGame() {

	}
public:
	inline Force* force() {
		return _force;
	}
	inline const Force* force() const override {
		return _force;
	}

	inline virtual Unit* asUnit() {
		return NULL;
	}
	inline virtual Spawn* asSpawn() {
		return NULL;
	}
	inline const Unit* asUnit() const {
		return const_cast<UnitBaseGame*>(this)->asUnit();
	}
	inline const Spawn* asSpawn() const {
		return const_cast<UnitBaseGame*>(this)->asSpawn();
	}
	inline virtual fixed attack(Unit* target, fixed rate, fixed time) {
		return fixed::Zero;
	}
	inline const ImageIndex& smallIcon() const {
		const SkinData* skin = skinData();
		return skin ? skin->smallIcon : ImageIndex::None;
	}
};

#endif