//
//  AnnounceData.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 12. 2..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef AnnounceData_h
#define AnnounceData_h

#include "AssetIndex.h"

enum AnnounceIndex {
	AnnounceNone = -1,
	AnnounceGameStart,
	AnnounceNotEnoughMineral,
	AnnounceNotEnoughGas,
	AnnounceNotEnoughMana,
	AnnounceRunOutMineral,
	AnnounceRunOutGas,
	AnnounceBuildUnabled,
	AnnounceBuildStopped,
	AnnounceBuildComplete,
	AnnounceTrainingComplete,
	AnnounceSpellDetected,
	AnnouncePlayerBaseAttacked,
	AnnouncePlayerAttacked,
	AnnounceAllianceBaseAttacked,
	AnnounceAllianceAttacked
};

struct AnnounceData {
	const CSLocaleString* name;
	const CSLocaleString* description;
	const CSDictionary<CSString, SoundGroupIndex>* voices;

	AnnounceData(CSBuffer* buffer);
	~AnnounceData();

	inline AnnounceData(const AnnounceData& other) {
		CSAssert(false, "invalid operation");
	}
	inline AnnounceData& operator=(const AnnounceData&) {
		CSAssert(false, "invalid operation");
		return *this;
	}
	SoundIndex voice(AnnounceIndex index) const;
};


#endif /* AnnounceData_h */
