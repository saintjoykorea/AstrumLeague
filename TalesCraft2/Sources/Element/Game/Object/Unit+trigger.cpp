//
//  Unit+value.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 5..
//  Copyright © 2017년 brgames. All rights reserved.
//
#define GameImpl

#include "Unit.h"

#include "Map.h"

AbilityValue<byte> Unit::readAbilityByte(const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<byte>(readByte(cursor), AbilityNone);
        case 1:
            return AbilityValue<byte>(0, readShort(cursor));
        case 2:
			{
				int v = readByte(cursor);
				return AbilityValue<byte>(v, readShort(cursor));
			}
		case 3:
			return AbilityValue<byte>(_revision, AbilityNone);
		case 4:
			return AbilityValue<byte>(_revision, readShort(cursor));
		case 5:
			return AbilityValue<byte>(_damage, AbilityNone);
		case 6:
			return AbilityValue<byte>(_damage, readShort(cursor));
		case 7:
			return AbilityValue<byte>(_damageCount, AbilityNone);
		case 8:
			return AbilityValue<byte>(_damageCount, readShort(cursor));
        default:
            return AbilityValue<byte>(0, AbilityNone);
    }
}

AbilityValue<ushort> Unit::readAbilityShort(const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<ushort>(readShort(cursor), AbilityNone);
        case 1:
            return AbilityValue<ushort>(0, readShort(cursor));
        case 2:
			{
				int v = readShort(cursor);
				return AbilityValue<ushort>(v, readShort(cursor));
			}
		case 3:
			return AbilityValue<ushort>(_revision, AbilityNone);
		case 4:
			return AbilityValue<ushort>(_revision, readShort(cursor));
		case 5:
			return AbilityValue<ushort>(_damage, AbilityNone);
		case 6:
			return AbilityValue<ushort>(_damage, readShort(cursor));
		case 7:
			return AbilityValue<ushort>(_damageCount, AbilityNone);
		case 8:
			return AbilityValue<ushort>(_damageCount, readShort(cursor));
        default:
            return AbilityValue<ushort>(0, AbilityNone);
    }
}

AbilityValue<float> Unit::readAbilityFloat(const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<float>(readFloat(cursor), AbilityNone);
        case 1:
            return AbilityValue<float>(0, readShort(cursor));
        case 2:
			{
				float v = readFloat(cursor);
				return AbilityValue<float>(v, readShort(cursor));
			}
        case 3:
            return AbilityValue<float>(_revision, AbilityNone);
        case 4:
            return AbilityValue<float>(_revision, readShort(cursor));
        case 5:
            return AbilityValue<float>(_damage, AbilityNone);
        case 6:
            return AbilityValue<float>(_damage, readShort(cursor));
		case 7:
			return AbilityValue<float>(_damageCount, AbilityNone);
		case 8:
			return AbilityValue<float>(_damageCount, readShort(cursor));
        default:
            return AbilityValue<float>(0, AbilityNone);
    }
}

AbilityValue<fixed> Unit::readAbilityFixed(const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<fixed>(readFixed(cursor), AbilityNone);
        case 1:
            return AbilityValue<fixed>(fixed::Zero, readShort(cursor));
        case 2:
			{
				fixed v = readFixed(cursor);
				return AbilityValue<fixed>(v, readShort(cursor));
			}
        case 3:
            return AbilityValue<fixed>(_revision, AbilityNone);
        case 4:
            return AbilityValue<fixed>(_revision, readShort(cursor));
        case 5:
            return AbilityValue<fixed>(_damage, AbilityNone);
        case 6:
            return AbilityValue<fixed>(_damage, readShort(cursor));
		case 7:
			return AbilityValue<fixed>((fixed)_damageCount, AbilityNone);
		case 8:
			return AbilityValue<fixed>((fixed)_damageCount, readShort(cursor));
		default:
            return AbilityValue<fixed>(fixed::Zero, AbilityNone);
    }
}

AbilityValue<byte> Unit::readAbilityByte(const Buff* buff, const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<byte>(readByte(cursor), AbilityNone);
        case 1:
            return AbilityValue<byte>(0, readShort(cursor));
        case 2:
			{
				int v = readByte(cursor);
				return AbilityValue<byte>(v, readShort(cursor));
			}
		case 3:
			return AbilityValue<byte>(buff->revision, AbilityNone);
		case 4:
			return AbilityValue<byte>(buff->revision, readShort(cursor));
		case 5:
			return AbilityValue<byte>(buff->damage, AbilityNone);
		case 6:
			return AbilityValue<byte>(buff->damage, readShort(cursor));
		case 7:
			return AbilityValue<byte>(buff->damageCount, AbilityNone);
		case 8:
			return AbilityValue<byte>(buff->damageCount, readShort(cursor));
		case 9:
			return AbilityValue<byte>(buff->stack, AbilityNone);
		case 10:
			return AbilityValue<byte>(buff->stack, readShort(cursor));
        default:
            return AbilityValue<byte>(0, AbilityNone);
    }
}

AbilityValue<ushort> Unit::readAbilityShort(const Buff* buff, const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<ushort>(readShort(cursor), AbilityNone);
        case 1:
            return AbilityValue<ushort>(0, readShort(cursor));
        case 2:
			{
				int v = readShort(cursor);
				return AbilityValue<ushort>(v, readShort(cursor));
			}
		case 3:
			return AbilityValue<ushort>(buff->revision, AbilityNone);
		case 4:
			return AbilityValue<ushort>(buff->revision, readShort(cursor));
		case 5:
			return AbilityValue<ushort>(buff->damage, AbilityNone);
		case 6:
			return AbilityValue<ushort>(buff->damage, readShort(cursor));
		case 7:
			return AbilityValue<ushort>(buff->damageCount, AbilityNone);
		case 8:
			return AbilityValue<ushort>(buff->damageCount, readShort(cursor));
		case 9:
			return AbilityValue<ushort>(buff->stack, AbilityNone);
		case 10:
			return AbilityValue<ushort>(buff->stack, readShort(cursor));
        default:
            return AbilityValue<ushort>(0, AbilityNone);
    }
}

AbilityValue<float> Unit::readAbilityFloat(const Buff* buff, const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<float>(readFloat(cursor), AbilityNone);
        case 1:
            return AbilityValue<float>(0, readShort(cursor));
        case 2:
			{
				float v = readFloat(cursor);
				return AbilityValue<float>(v, readShort(cursor));
			}
        case 3:
            return AbilityValue<float>(buff->revision, AbilityNone);
        case 4:
            return AbilityValue<float>(buff->revision, readShort(cursor));
        case 5:
            return AbilityValue<float>(buff->damage, AbilityNone);
        case 6:
            return AbilityValue<float>(buff->damage, readShort(cursor));
		case 7:
			return AbilityValue<float>(buff->damageCount, AbilityNone);
		case 8:
			return AbilityValue<float>(buff->damageCount, readShort(cursor));
		case 9:
			return AbilityValue<float>(buff->stack, AbilityNone);
		case 10:
			return AbilityValue<float>(buff->stack, readShort(cursor));
        default:
            return AbilityValue<float>(0, AbilityNone);
    }
}

AbilityValue<fixed> Unit::readAbilityFixed(const Buff* buff, const byte*& cursor) {
    switch (readByte(cursor)) {
        case 0:
            return AbilityValue<fixed>(readFixed(cursor), AbilityNone);
        case 1:
            return AbilityValue<fixed>(fixed::Zero, readShort(cursor));
        case 2:
			{
				fixed v = readFixed(cursor);
				return AbilityValue<fixed>(v, readShort(cursor));
			}
        case 3:
            return AbilityValue<fixed>(buff->revision, AbilityNone);
        case 4:
            return AbilityValue<fixed>(buff->revision, readShort(cursor));
        case 5:
            return AbilityValue<fixed>(buff->damage, AbilityNone);
        case 6:
            return AbilityValue<fixed>(buff->damage, readShort(cursor));
		case 7:
			return AbilityValue<fixed>((fixed)buff->damageCount, AbilityNone);
		case 8:
			return AbilityValue<fixed>((fixed)buff->damageCount, readShort(cursor));
		case 9:
			return AbilityValue<fixed>(buff->stack, AbilityNone);
		case 10:
			return AbilityValue<fixed>(buff->stack, readShort(cursor));
        default:
            return AbilityValue<fixed>(fixed::Zero, AbilityNone);
    }
}

UnitIndex Unit::readRandomUnitIndex(const byte*& cursor) {
	int count = readLength(cursor);
	CSAssert(count >= 1, "invalid data");
	const byte* base = cursor;
	cursor += count * sizeof(UnitIndex);
	if (count > 1) {
		int random = Map::sharedMap()->random()->nextInt(0, count - 1);

		for (int i = 0; i < count; i++) {
			const byte* current = base + ((random + i) % count) * sizeof(UnitIndex);
			UnitIndex index(current);
			if (Asset::sharedAsset()->openCards->containsObject(index)) {
				return index;
			}
		}
	}
	return UnitIndex(base);
}