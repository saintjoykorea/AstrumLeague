//
//  Spawn+value.cpp
//  TalesCraft2
//
//  Created by Kim Chan on 2017. 4. 5..
//  Copyright © 2017년 brgames. All rights reserved.
//
#include "Spawn.h"

AbilityValue<byte> Spawn::readAbilityByte(const byte*& cursor) {
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
        default:
            return AbilityValue<byte>(0, AbilityNone);
    }
}

AbilityValue<ushort> Spawn::readAbilityShort(const byte*& cursor) {
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
        default:
            return AbilityValue<ushort>(0, AbilityNone);
    }
}

AbilityValue<float> Spawn::readAbilityFloat(const byte*& cursor) {
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
        default:
            return AbilityValue<float>(0, AbilityNone);
    }
}

AbilityValue<fixed> Spawn::readAbilityFixed(const byte*& cursor) {
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
        default:
            return AbilityValue<fixed>(fixed::Zero, AbilityNone);
    }
}
