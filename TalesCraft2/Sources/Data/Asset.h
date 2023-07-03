//
//  Asset.h
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//

#ifndef Asset_h
#define Asset_h

#include "AssetDefinition.h"

#include "LocaleData.h"
#include "AbilityData.h"
#include "CommandData.h"
#include "DamageTypeData.h"
#include "UnitData.h"
#include "ResourceData.h"
#include "BuffData.h"
#include "RuneData.h"
#include "PingData.h"
#include "AnnounceData.h"
#include "ThemeData.h"
#include "EmoticonData.h"

#include "TerrainData.h"

#include "MapThumb.h"

#include "Animation.h"

enum AssetPoolType : byte {
    AssetPoolNone,
    AssetPoolImage,
    AssetPoolMesh,
    AssetPoolMap,
    AssetPoolImageFromFile,
    AssetPoolImageFromURL
};

struct AssetPoolKey {
    AssetPoolType type;
    
    union _context {
        AssetIndex4 index;
        struct {
            const CSString* path;
            uint format;
        } externImage;
        
        inline _context() {}
    } context;
    
    uint hash;
    
    AssetPoolKey();
    AssetPoolKey(AssetPoolType type, const AssetIndex4& index);
    AssetPoolKey(AssetPoolType type, const CSString* imagePath, uint imageFormat);
    AssetPoolKey(const AssetPoolKey& other);
    ~AssetPoolKey();
    
    AssetPoolKey& operator=(const AssetPoolKey& other);
    
    bool operator ==(const AssetPoolKey& other) const;
    inline bool operator !=(const AssetPoolKey& other) const {
        return !(*this == other);
    }
    inline operator uint() const {
        return hash;
    }
};

struct Asset {
public:
	struct {
		ushort maxSupply;
        ushort maxCardLevel;
        ushort maxRuneLevel;
        ushort resourceChargeMax;
        fixed resourceChargeTime;
	} gamePreference;
    const CSArray<AbilityData>* abilities;
    const CSArray<AbilityTargetData>* abilityTargets;
    ushort abilityRefs[AbilityCount];
    const CSArray<CommandData, 2>* commands;
    const CSArray<DamageTypeData>* attackTypes;
    const CSArray<DamageTypeData>* armorTypes;
    const CSArray<fixed, 2>* damages;
    const CSArray<UnitData, 2>* units;
    const ResourceData* resources[2];
    const CSArray<BuffData, 2>* buffs;
    const CSArray<RuneData, 2>* runes;
    const CSArray<byte, 3>* aiTriggers;
    const CSArray<PingData>* pings;
	const CSArray<AnnounceData>* announces;
    const CSArray<ThemeData, 2>* themes;
    const CSArray<EmoticonData, 2>* emoticons;
    const CSArray<UnitIndex>* openCards;
    const CSArray<RuneIndex>* openRunes;

    const TerrainData* terrain;
    
    const CSArray<CSAnimation, 3>* animations;
    const CSArray<CSLocaleString, 2>* strings;

	const CSArray<LocaleData>* locales;

	const CSFont* extraSmallFont;
	const CSFont* smallFont;
	const CSFont* mediumFont;
	const CSFont* largeFont;
	const CSFont* extraLargeFont;
	const CSFont* boldExtraSmallFont;
	const CSFont* boldSmallFont;
	const CSFont* boldMediumFont;
	const CSFont* boldLargeFont;
	const CSFont* boldExtraLargeFont;

    static const CSColor playerColor0;
    static const CSColor playerColor1;
    static const CSColor allianceColor;
    static const CSColor enemyColor;
    static const CSColor neutralColor;
    static const CSColor forceColors[];
    static const CSColor gradeColors[];
private:
    const CSResource* resourceDelegateCallback(CSResourceType resourceType, const ushort* indices, int indexCount);
public:
    CSResourceDelegate* resourceDelegate;
#ifdef AssetImpl
public:
#else
private:
#endif
	typedef CSPool<AssetPoolKey, CSObject> AssetPool;
	AssetPool* pool;
    CSDictionary<AssetPoolKey, double>* poolLoadAsyncs;
private:
    static Asset* __asset;
    Asset();
    ~Asset();
    
public:
#ifdef AssetImpl
    static inline Asset* sharedAsset() {
        return __asset;
    }
#else
    static inline const Asset* sharedAsset() {
        return __asset;
    }
#endif
    static void load();
    static void destroy();

    void setLocale(const char* originLocale);
private:
    void loadFonts();
    void loadLocales();
    void loadAnimations();
    void loadStrings();
    void loadGame();

    void destroyFonts();
    void destroyGame();
public:
    const CSArray<CSImage>* images(const ImageGroupIndex& group) const;
    const CSImage* image(const ImageIndex& index) const;
    const CSImage* image(const ImageGroupIndex& group, int index) const;
    const CSImage* image(const ImageGroupIndex& group) const;
    const MapThumb* map(const MapIndex& index) const;
    const CSImage* imageFromPath(const CSString* path, uint format) const;
    const CSImage* imageFromURL(const CSString* path, uint format) const;
    //====================================================================================================================================
    inline const CommandData& command(const CommandIndex& index) const {
        return commands->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
    inline const CommandData& command(int group, int index) const {
        return commands->objectAtIndex(group)->objectAtIndex(index);
    }
    inline const DamageTypeData& attackType(int attackType) const {
        return attackTypes->objectAtIndex(attackType);
    }
    inline const DamageTypeData& armorType(int armorType) const {
        return armorTypes->objectAtIndex(armorType);
    }
    inline fixed damage(int attackType, int armorType) const {
        return damages->objectAtIndex(attackType)->objectAtIndex(armorType);
    }
    inline const UnitData& unit(const UnitIndex& index) const {
        return units->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
    inline const SkinData* skin(const SkinIndex& index) const {
        return units->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]).skin(index.indices[2]);
    }
    inline const ResourceData& resource(int kind) const {
        return *resources[kind];
    }
    inline const BuffData& buff(const BuffIndex& index) const {
        return buffs->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
    inline const RuneData& rune(const RuneIndex& index) const {
        return runes->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
	inline const PingData& ping(int index) const {
		return pings->objectAtIndex(index);
	}
	inline const AnnounceData& announce(int index) const {
		return announces->objectAtIndex(index);
	}
    inline const ThemeData& theme(const ThemeIndex& index) const {
        return themes->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
    inline const EmoticonData& emoticon(const EmoticonIndex& index) const {
        return emoticons->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1]);
    }
    //====================================================================================================================================
    inline const CSString* string(const MessageIndex& index) const {
        return strings->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1])->value();
    }
    inline const CSString* string(int group, int index) const {
        return strings->objectAtIndex(group)->objectAtIndex(index)->value();
    }
    inline const CSString* string(const MessageIndex& index, const char* locale) const {
        return strings->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1])->localeValue(locale);
    }
    inline const CSString* string(int group, int index, const char* locale) const {
        return strings->objectAtIndex(group)->objectAtIndex(index)->localeValue(locale);
    }
    inline const CSAnimation* animation(const AnimationGroupIndex& group, int index) const {
        return animations->objectAtIndex(group.indices[0])->objectAtIndex(group.indices[1])->objectAtIndex(index);
    }
    inline const CSAnimation* animation(const AnimationIndex& index) const {
        return animations->objectAtIndex(index.indices[0])->objectAtIndex(index.indices[1])->objectAtIndex(index.indices[2]);
    }
	//====================================================================================================================================
	inline const LocaleData* locale(const CSString* locale) const {
		foreach(const LocaleData&, data, locales) {
			if (data.code()->isEqualToString(locale)) return &data;
		}
		return NULL;
	}
public:
    void reload();
    void purge(uint cost);
    void memoryWarning(CSMemoryWarningLevel level);

    inline uint poolingCount() const {
        return pool->count();
    }
    inline uint poolingMemory() const {
        return pool->cost();
    }
    static CSString* getFileKey(const char* subpath);
    
    enum DecryptionFileLocation {
        DecryptionFileLocationBundle,
        DecryptionFileLocationStorage,
        DecryptionFileLocationAll
    };
    static CSBuffer* readFromFileWithDecryption(const char* subpath, bool autorelease, bool compression = false, const char* fileKey = NULL, DecryptionFileLocation location = DecryptionFileLocationAll);
    static bool writeToFileWithEncryption(const CSBuffer* buffer, const char* subpath, bool compression = false, const char* fileKey = NULL);
    
    static const char* replaySubPath(int64 matchRecordId);
};

#endif /* Asset_h */
