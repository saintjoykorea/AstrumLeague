//
//  Asset.cpp
//  TalesCraft2
//
//  Created by 김찬 on 13. 11. 28..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
#define AssetImpl

#include "Asset.h"

#define AssetPoolTimeout    1.0f

const CSColor Asset::playerColor0 (0, 255, 64, 255);
const CSColor Asset::playerColor1 (0, 128, 0, 255);
const CSColor Asset::allianceColor (0, 128, 255, 255);
const CSColor Asset::enemyColor (255, 0, 0, 255);
const CSColor Asset::neutralColor (255, 255, 0, 255);
const CSColor Asset::forceColors[] {
    CSColor(0, 255, 255, 255),          //allience 0
    CSColor(0, 64, 128, 255),           //allience 0
    CSColor(128, 128, 255, 255),        //allience 0
    CSColor(255, 64, 0, 255),           //allience 1
    CSColor(128, 0, 0, 255),            //allience 1
    CSColor(255, 128, 128, 255),        //allience 1
    CSColor(255, 255, 128, 255),        //etc 1
    CSColor(128, 255, 128, 255)         //etc 2
};
const CSColor Asset::gradeColors[] {
    CSColor(0, 255, 0, 255),
    CSColor(40, 150, 255, 255),
    CSColor(230, 40, 230, 255),
    CSColor(245, 130, 10, 255)
};

AssetPoolKey::AssetPoolKey() {
    memset(this, 0, sizeof(AssetPoolKey));
}

AssetPoolKey::AssetPoolKey(AssetPoolType type, const AssetIndex4& index) : type(type) {
#ifdef CS_ASSERT_DEBUG
    switch (type) {
        case AssetPoolImage:
        case AssetPoolMesh:
        case AssetPoolMap:
            break;
        default:
            CSAssert(false, "invalid operation");
            break;
    }
#endif
    context.index = index;
    
    hash = (type << 28) | (index.indices[0] << 16) | index.indices[1];
}

AssetPoolKey::AssetPoolKey(AssetPoolType type, const CSString* imagePath, uint imageFormat) : type(type) {
#ifdef CS_ASSERT_DEBUG
    switch (type) {
        case AssetPoolImageFromFile:
        case AssetPoolImageFromURL:
            break;
        default:
            CSAssert(false, "invalid operation");
            break;
    }
#endif
    context.externImage.path = CSObject::retain(imagePath);
    context.externImage.format = imageFormat;

    hash = (type << 28) | imagePath->hash() | imageFormat;
}

AssetPoolKey::AssetPoolKey(const AssetPoolKey& other) : type(other.type), hash(other.hash){
    switch (type) {
        case AssetPoolImage:
        case AssetPoolMesh:
        case AssetPoolMap:
            context.index = other.context.index;
            break;
        case AssetPoolImageFromFile:
        case AssetPoolImageFromURL:
            context.externImage.path = CSObject::retain(other.context.externImage.path);
            context.externImage.format = other.context.externImage.format;
            break;
        default:
            CSAssert(false, "invalid operation");
            break;
    }
}

AssetPoolKey::~AssetPoolKey() {
    switch (type) {
        case AssetPoolImageFromFile:
        case AssetPoolImageFromURL:
            CSObject::release(context.externImage.path);
            break;
    }
}

AssetPoolKey& AssetPoolKey::operator=(const AssetPoolKey &other) {
    switch (type) {
        case AssetPoolImageFromFile:
        case AssetPoolImageFromURL:
            CSObject::release(context.externImage.path);
            break;
    }
    type = other.type;
    switch (type) {
        case AssetPoolImage:
        case AssetPoolMesh:
        case AssetPoolMap:
            context.index = other.context.index;
            break;
        case AssetPoolImageFromFile:
        case AssetPoolImageFromURL:
            context.externImage.path = CSObject::retain(other.context.externImage.path);
            context.externImage.format = other.context.externImage.format;
            break;
        default:
            CSAssert(false, "invalid operation");
            break;
    }
    hash = other.hash;
    return *this;
}

bool AssetPoolKey::operator ==(const AssetPoolKey& other) const {
    if (type == other.type) {
        switch (type) {
            case AssetPoolImage:
            case AssetPoolMesh:
            case AssetPoolMap:
                return context.index == other.context.index;
            case AssetPoolImageFromFile:
            case AssetPoolImageFromURL:
                return context.externImage.path->isEqualToString(other.context.externImage.path) && context.externImage.format == other.context.externImage.format;
            default:
                CSAssert(false, "invalid operation");
                break;
        }
    }
    return false;
}


class PoolingImage : public CSObject {
public:
    const CSRootImage* rootImage;
    const CSArray<CSImage>* subImages;
private:
    ~PoolingImage();
public:
    static PoolingImage* image(CSBuffer* buffer);
};

PoolingImage::~PoolingImage() {
    release(rootImage);
    release(subImages);
}
PoolingImage* PoolingImage::image(CSBuffer* buffer) {
    const CSRootImage* root = CSRootImage::createWithBuffer(buffer);
    if (!root) {
        return NULL;
    }
    const CSArray<CSImage>* subs = retain(buffer->readArrayFuncWithParam<CSImage>(CSImage::subImageWithBuffer, static_cast<const CSImage*>(root)));
    
    PoolingImage* rtn = autorelease(new PoolingImage());
    rtn->rootImage = root;
    rtn->subImages = subs;
    return rtn;
}

static const char* webCachePath(const char* path) {
	char* str = (char*)alloca(11 + strlen(path));
	strncpy(str, "web-cache/", 10);
	strcpy(str + 10, path);
	char* pstr = str + 10;
	while (*pstr) {
		switch (*pstr) {
			case '/':
			case ':':
			case '\\':
			case '?':
			case '*':
			case '<':
			case '>':
			case '|':
				*pstr = '_';
				break;
		}
		pstr++;
	}
	return CSFile::storagePath(str);
}

static void assetPoolLoadImageFromURL(CSData* data, int statusCode, AssetPoolKey key);

static CSPoolLoadReturn<CSObject> assetPoolLoad(const AssetPoolKey& key) {
    switch (key.type) {
        case AssetPoolImage:
            {
                const char* path = CSFile::findPath(CSString::cstringWithFormat ("images/images[%03d][%03d].xmf", key.context.index.indices[0], key.context.index.indices[1]));
                if (!path) {
                    CSErrorLog("load image fail(path not found):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(false);
                }
                CSBuffer* buffer = CSBuffer::createWithContentOfFile(path, true);
                if (!buffer) {
                    CSWarnLog("load image fail(buffer error):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(true);
                }
                PoolingImage* rtn = PoolingImage::image(buffer);
                buffer->release();
                if (!rtn) {
                    CSWarnLog("load image fail(content error):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(true);
                }

                CSLog("load image:%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);

                return CSPoolLoadReturn<CSObject>::success(rtn, rtn->rootImage->memory());
            }
        case AssetPoolMesh:
            {
                const char* path = CSFile::findPath(CSString::cstringWithFormat ("meshes/mesh[%03d][%03d].xmf", key.context.index.indices[0], key.context.index.indices[1]));
                if (!path) {
                    CSErrorLog("load mesh fail(path not found):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(false);
                }
                CSBuffer* buffer = CSBuffer::createWithContentOfFile(path);
                if (!buffer) {
                    CSWarnLog("load mesh fail(buffer error):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(true);
                }
                CSMesh* rtn = CSMesh::meshWithBuffer(buffer);
                buffer->release();
                if (!rtn) {
                    CSWarnLog("load mesh fail(content error):%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);
                    return CSPoolLoadReturn<CSObject>::fail(true);
                }

                CSLog("load mesh:%03d, %03d", key.context.index.indices[0], key.context.index.indices[1]);

                return CSPoolLoadReturn<CSObject>::success(rtn, rtn->memory());
            }
        case AssetPoolMap:
            {
                MapThumb* rtn = MapThumb::map(key.context.index);
                if (!rtn) {
                    CSWarnLog("load map fail(content error):%03d, %03d, %03d, %03d", key.context.index.indices[0], key.context.index.indices[1], key.context.index.indices[2], key.context.index.indices[3]);
                    return CSPoolLoadReturn<CSObject>::fail(false);
                }

                CSLog("load map:%03d, %03d, %03d, %03d", key.context.index.indices[0], key.context.index.indices[1], key.context.index.indices[2], key.context.index.indices[3]);

                return CSPoolLoadReturn<CSObject>::success(rtn, rtn->memory());
            }
        case AssetPoolImageFromFile:
            {
                const char* subpath = *key.context.externImage.path;
                const char* path = CSFile::findPath(subpath);
                if (!path) {
                    CSErrorLog("load image file fail(path not found):%s", subpath);
                    return CSPoolLoadReturn<CSObject>::fail(false);
                }
                CSLog("load image file:%s, %d", path, key.context.externImage.format);
                CSRootImage* rtn = CSRootImage::imageWithContentOfFile(path, key.context.externImage.format);
                if (!rtn) {
                    CSWarnLog("load image file fail(content error):%s, %d", path, key.context.externImage.format);
                    return CSPoolLoadReturn<CSObject>::fail(true);
                }

                CSLog("load image file:%s", path);

                return CSPoolLoadReturn<CSObject>::success(rtn, rtn->memory());
            }
        case AssetPoolImageFromURL:
            if (!Asset::sharedAsset()->poolLoadAsyncs->containsKey(key)) {
				const char* path = webCachePath(*key.context.externImage.path);

				if (CSFile::fileExists(path)) {
					CSRootImage* rtn = CSRootImage::imageWithContentOfFile(path, key.context.externImage.format);
					if (rtn) {
						CSLog("load image url from cache:%s, %d", (const char*)*key.context.externImage.path, key.context.externImage.format);

						return CSPoolLoadReturn<CSObject>::success(rtn, rtn->memory());
					}
				}

				Asset::sharedAsset()->poolLoadAsyncs->setObject(key, CSTime::currentTime());
                
                CSLog("load image url request:%s, %d", (const char*)*key.context.externImage.path, key.context.externImage.format);

                CSURLRequest* request = CSURLRequest::request(*key.context.externImage.path);
                
                if (request) {
                    CSURLConnection::sendAsynchronousRequest(request, CSAsynchronousURLRequestDelegate1<AssetPoolKey>::delegate(&assetPoolLoadImageFromURL, key));
                }
            }
            break;
    }
    return CSPoolLoadReturn<CSObject>::fail(false);
}

static void assetPoolLoadImageFromURL(CSData* data, int statusCode, AssetPoolKey key) {
    Asset* asset = Asset::sharedAsset();
    
    if (asset) {
        const char* path = *key.context.externImage.path;

        double delay = CSTime::currentTime() - asset->poolLoadAsyncs->objectForKey(key);
        
        if (statusCode == 200) {
            CSRootImage* rtn = CSRootImage::imageWithImage(data->bytes(), data->length(), key.context.externImage.format);
            
            if (rtn) {
				data->writeToFile(webCachePath(path));

                CSLog("load image url success:%.2f, %s, %d", delay, path, key.context.externImage.format);
                asset->pool->add(key, rtn, rtn->memory());
            }
            else {
                CSLog("load image url fail:%.2f, %s, %d", delay, path, key.context.externImage.format);
            }
            asset->poolLoadAsyncs->removeObject(key);
        }
        else {
            CSWarnLog("load image url http error(%d):%.2f, %s, %d", statusCode, delay, path, key.context.externImage.format);
            
            if (statusCode == -1) asset->poolLoadAsyncs->removeObject(key);
        }
    }
}

const CSResource* Asset::resourceDelegateCallback(CSResourceType resourceType, const ushort* indices, int indexCount) {
    switch (resourceType) {
        case CSResourceTypeImage:
            {
                CSAssert(indexCount == 2 || indexCount == 3, "invalid data");
                
                AssetPoolKey key(AssetPoolImage, AssetIndex4(indices[0], indices[1], 0, 0));
                
                PoolingImage* entry = static_cast<PoolingImage*>(pool->entry(key));
                
                return entry ? (indexCount == 3 ? entry->subImages->objectAtIndex(indices[2]) : entry->rootImage) : NULL;
            }
        case CSResourceTypeMesh:
            {
                CSAssert (indexCount == 2, "invalid data");
                
                AssetPoolKey key(AssetPoolMesh, AssetIndex4(indices[0], indices[1], 0, 0));
                
                CSMesh* entry = static_cast<CSMesh*>(pool->entry(key));
                
                return entry;
            }
        case CSResourceTypeString:
            CSAssert(indexCount == 2, "invalid data");
            
            return string(indices[0], indices[1]);
    }
    return NULL;
}

Asset* Asset::__asset = NULL;

Asset::Asset() {
    memset(this, 0, sizeof(Asset));

	__asset = this;

    CSFile::makeDirectory(CSFile::storagePath("data"));
    CSFile::makeDirectory(CSFile::storagePath("images"));
    CSFile::makeDirectory(CSFile::storagePath("meshes"));
    CSFile::makeDirectory(CSFile::storagePath("sounds"));
    CSFile::makeDirectory(CSFile::storagePath("replays"));
    CSFile::makeDirectory(CSFile::storagePath("maps"));
    CSFile::makeDirectory(CSFile::storagePath("results"));
	CSFile::makeDirectory(CSFile::storagePath("web-cache"));
    
	pool = new AssetPool(AssetPool::Delegate::delegate(&assetPoolLoad));
    pool->setTimeout(AssetPoolTimeout);

	poolLoadAsyncs = new CSDictionary<AssetPoolKey, double>();

	resourceDelegate = new CSResourceDelegate0(this, &Asset::resourceDelegateCallback);

    loadFonts();
	loadLocales();
    loadAnimations();
    loadStrings();
    loadGame();
    terrain = new TerrainData();
}

void Asset::setLocale(const char* originLocale) {
    int originLocaleLen = strlen(originLocale);

    const char* locale = "en";
    foreach(const LocaleData&, data, locales) {
        foreach(const CSString*, code, data.codes) {
            if (code->length() <= originLocaleLen && strncasecmp(originLocale, *code, code->length()) == 0) {
                locale = *data.code();
                break;
            }
        }
    }
    foreach(const LocaleData&, data, locales) {
        if (data.code()->isEqualToString(locale)) {
            CSLocaleString::setLocale(locale);
            CSLocaleString::setDigitGroupSeperator(data.digitGroupSeperator);
            break;
        }
    }
}

void Asset::loadFonts() {
	static const char* FontName = "NotoSans";

	foreach(const CSString*, path, CSFile::filePaths(CSFile::bundlePath("fonts/normal"), true)) {
		CSLog("load font:%s", (const char*)*path);
		CSFont::load(FontName, CSFontStyleNormal, *path);
	}
	foreach(const CSString*, path, CSFile::filePaths(CSFile::bundlePath("fonts/bold"), true)) {
		CSLog("load font:%s", (const char*)*path);
		CSFont::load(FontName, CSFontStyleBold, *path);
	}

	extraSmallFont = new CSFont(FontName, 16, CSFontStyleNormal);
	smallFont = new CSFont(FontName, 20, CSFontStyleNormal);
	mediumFont = new CSFont(FontName, 24, CSFontStyleNormal);
	largeFont = new CSFont(FontName, 30, CSFontStyleNormal);
	extraLargeFont = new CSFont(FontName, 36, CSFontStyleNormal);
	boldExtraSmallFont = new CSFont(FontName, 16, CSFontStyleBold);
	boldSmallFont = new CSFont(FontName, 20, CSFontStyleBold);
	boldMediumFont = new CSFont(FontName, 24, CSFontStyleBold);
	boldLargeFont = new CSFont(FontName, 30, CSFontStyleBold);
	boldExtraLargeFont = new CSFont(FontName, 36, CSFontStyleBold);

	CSGraphics::setDefaultFont(smallFont);
}

void Asset::destroyFonts() {
	extraSmallFont->release();
	smallFont->release();
	mediumFont->release();
	largeFont->release();
	extraLargeFont->release();
	boldExtraSmallFont->release();
	boldSmallFont->release();
	boldMediumFont->release();
	boldLargeFont->release();
	boldExtraLargeFont->release();
}

void Asset::loadLocales() {
	CSBuffer* buffer = CSBuffer::createWithContentOfFile(CSFile::findPath("locale.xmf"));
    locales = CSObject::retain(buffer->readArray<LocaleData>());
	CSAssert(buffer->position() == buffer->length(), "invalid data");
	buffer->release();

#ifdef Locale
    setLocale(Locale);
#else
    setLocale(CSDevice::locale());
#endif
}

void Asset::loadAnimations() {
    CSBuffer* buffer = CSBuffer::createWithContentOfFile(CSFile::findPath("animations.xmf"));
    animations = CSObject::retain(buffer->readArray<CSAnimation, 3>());
    CSAssert (buffer->position() == buffer->length(), "invalid data");
    buffer->release();
}

void Asset::loadStrings() {
    CSBuffer* buffer = CSBuffer::createWithContentOfFile(CSFile::findPath("strings.xmf"));
    strings = CSObject::retain(buffer->readArrayFunc<CSLocaleString, 2>(&CSLocaleString::stringWithBuffer));
    CSAssert (buffer->position() == buffer->length(), "invalid data");
    buffer->release();
}

void Asset::loadGame() {
    CSBuffer* buffer = Asset::readFromFileWithDecryption("data/game.xmf", false);
    
	gamePreference.maxSupply = buffer->readShort();
	gamePreference.maxCardLevel = buffer->readShort();
    gamePreference.maxRuneLevel = buffer->readShort();
    gamePreference.resourceChargeMax = buffer->readShort();
    gamePreference.resourceChargeTime = buffer->readFixed();

    abilities = CSObject::retain(buffer->readArray<AbilityData>());
    abilityTargets = CSObject::retain(buffer->readArray<AbilityTargetData>());
    int abilityCount = buffer->readLength();
    CSAssert(abilityCount == abilityCount, "invalid data");
    for (int i = 0; i < abilityCount; i++) {
        abilityRefs[i] = buffer->readShort();
    }
    commands = CSObject::retain(buffer->readArray<CommandData, 2>());
    attackTypes = CSObject::retain(buffer->readArray<DamageTypeData>());
    armorTypes = CSObject::retain(buffer->readArray<DamageTypeData>());
    if (attackTypes && armorTypes) {
        CSArray<fixed, 2>* damages = new CSArray<fixed, 2>(attackTypes->count());
        for (int i = 0; i < attackTypes->count(); i++) {
            CSArray<fixed>* subdamages = new CSArray<fixed>(armorTypes->count());
            damages->addObject(subdamages);
            subdamages->release();
            for (int j = 0; j < armorTypes->count(); j++) {
                subdamages->addObject(buffer->readFixed());
            }
        }
        this->damages = damages;
    }
    units = CSObject::retain(buffer->readArray<UnitData, 2>());
    resources[0] = new ResourceData(buffer);
    resources[1] = new ResourceData(buffer);
    buffs = CSObject::retain(buffer->readArray<BuffData, 2>());
    runes = CSObject::retain(buffer->readArray<RuneData, 2>());
    aiTriggers = CSObject::retain(buffer->readArray<byte, 3>());
    pings = CSObject::retain(buffer->readArray<PingData>());
	announces = CSObject::retain(buffer->readArray<AnnounceData>());
    themes = CSObject::retain(buffer->readArray<ThemeData, 2>());
    emoticons = CSObject::retain(buffer->readArray<EmoticonData, 2>());
    openCards = CSObject::retain(buffer->readArray<UnitIndex>());
    openRunes = CSObject::retain(buffer->readArray<RuneIndex>());

    CSAssert(buffer->position() == buffer->length(), "invalid data");

    buffer->release();
}

void Asset::destroyGame() {
    CSObject::release(abilities);
    CSObject::release(abilityTargets);
    CSObject::release(commands);
    CSObject::release(attackTypes);
    CSObject::release(armorTypes);
    CSObject::release(damages);
    CSObject::release(units);
    if (resources[0]) delete resources[0];
    if (resources[1]) delete resources[1];
    CSObject::release(buffs);
    CSObject::release(runes);
    CSObject::release(aiTriggers);
    CSObject::release(pings);
	CSObject::release(announces);
	CSObject::release(themes);
	CSObject::release(emoticons);
}

Asset::~Asset() {
	destroyFonts();
	locales->release();
    animations->release();
    strings->release();
    if (terrain) delete terrain;

    destroyGame();

    pool->release();
    poolLoadAsyncs->release();
    resourceDelegate->release();

	__asset = NULL;
}

void Asset::load() {
    if (!__asset) new Asset();
}

void Asset::destroy() {
    if (__asset) delete __asset;
}

const CSArray<CSImage>* Asset::images(const ImageGroupIndex& group) const {
    AssetPoolKey key(AssetPoolImage, AssetIndex4(group, 0, 0));
    
    PoolingImage* entry = static_cast<PoolingImage*>(pool->entry(key));
    
    return entry ? entry->subImages : NULL;
}

const CSImage* Asset::image(const ImageIndex& index) const {
    AssetPoolKey key(AssetPoolImage, AssetIndex4(index.indices[0], index.indices[1], 0, 0));
    
    PoolingImage* entry = static_cast<PoolingImage*>(pool->entry(key));
    
    return entry && entry->subImages ? entry->subImages->objectAtIndex(index.indices[2]) : NULL;
}

const CSImage* Asset::image(const ImageGroupIndex& group, int index) const {
    AssetPoolKey key(AssetPoolImage, AssetIndex4(group, 0, 0));
    
    PoolingImage* entry = static_cast<PoolingImage*>(pool->entry(key));
    
    return entry && entry->subImages ? entry->subImages->objectAtIndex(index) : NULL;
}

const CSImage* Asset::image(const ImageGroupIndex& group) const {
    AssetPoolKey key(AssetPoolImage, AssetIndex4(group, 0, 0));

    PoolingImage* entry = static_cast<PoolingImage*>(pool->entry(key));
    
    return entry ? entry->rootImage : NULL;
}

const MapThumb* Asset::map(const MapIndex &index) const {
    AssetPoolKey key(AssetPoolMap, index);
    
    MapThumb* entry = static_cast<MapThumb*>(pool->entry(key));
    
    return entry;
}

const CSImage* Asset::imageFromPath(const CSString* path, uint format) const {
    AssetPoolKey key(AssetPoolImageFromFile, path, format);
    
    CSRootImage* image = static_cast<CSRootImage*>(pool->entry(key));
    
    return image;
}

const CSImage* Asset::imageFromURL(const CSString* path, uint format) const {
    AssetPoolKey key(AssetPoolImageFromURL, path, format);
    
    CSRootImage* image = static_cast<CSRootImage*>(pool->entry(key));
    
    return image;
}

void Asset::reload() {
    CSLog("reload");
    pool->clear();
    
    if (terrain) {
        delete terrain;
        
        terrain = new TerrainData();
    }
}

void Asset::purge(uint cost) {
    pool->purge(cost, false);
}

void Asset::memoryWarning(CSMemoryWarningLevel level) {
    switch (level) {
        case CSMemoryWarningLevelNormal:
            CSWarnLog("purge normal");
            //pool->purge(20 * 1024768);
            break;
        case CSMemoryWarningLevelLow:
            CSWarnLog("purge low");
            pool->purge(32 * 1024768);
            break;
        case CSMemoryWarningLevelCritical:
            CSErrorLog("purge critical");
            pool->purge(64 * 1024768);
            break;
    }
}

CSString* Asset::getFileKey(const char* subpath) {
    CSString* key = CSString::sha1(CSString::cstringWithFormat(AssetFileKey "%s", subpath));
    if (key) {
        if (key->length() > 32) {
            CSString* trimKey = key->substringWithRange(0, 32);
            key->hide();
            key = trimKey;
        }
        else if (key->length() < 32) {
            int padding = 32 - key->length();
            for (int i = 0; i < padding; i++) {
                key->append("0");
            }
        }
        CSLog("local key:%s = %s", subpath, (const char*)*key);
    }
    else {
        CSErrorLog("key invalid:%s", subpath);
    }
    return key;
}

CSBuffer* Asset::readFromFileWithDecryption(const char *subpath, bool autorelease, bool compression, const char* fileKey, DecryptionFileLocation location) {
    const char* path;
    switch (location) {
        case DecryptionFileLocationBundle:
            path = CSFile::bundlePath(subpath);
            if (!CSFile::fileExists(path)) {
                CSErrorLog("path not exists(bundle):%s", subpath);
                return NULL;
            }
            break;
        case DecryptionFileLocationStorage:
            path = CSFile::storagePath(subpath);
            if (!CSFile::fileExists(path)) {
                CSErrorLog("path not exists(storage):%s", subpath);
                return NULL;
            }
            break;
        default:
            path = CSFile::findPath(subpath);
            if (!path) {
                CSErrorLog("path not exists:%s", subpath);
                return NULL;
            }
            break;
    }
    CSLog("data loading:%s", path);
    
    CSString* key = NULL;
    if (!fileKey) {
        key = getFileKey(subpath);
        if (!key) {
            return NULL;
        }
    }
    else {
        key = CSString::string(fileKey);
    }
    
    CSBuffer* rtn = CSBuffer::createWithContentOfFile(path);
    if (rtn) {
        if (!rtn->AESDecrypt(*key, true)) {
            CSErrorLog("decrypt error:%s", subpath);
            rtn->release();
            rtn = NULL;
        }
        else if (compression && !rtn->uncompress()) {
            CSErrorLog("uncompress error:%s", subpath);
            rtn->release();
            rtn = NULL;
        }
        else if (autorelease) {
            rtn->autorelease();
        }
    }
    else {
        CSErrorLog("file error:%s", subpath);
    }
    key->hide();
    return rtn;
}

bool Asset::writeToFileWithEncryption(const CSBuffer *buffer, const char* subpath, bool compression, const char* fileKey) {
    CSString* key = NULL;
    if (!fileKey) {
        key = getFileKey(subpath);
        if (!key) {
            return false;
        }
    }
    else {
        key = CSString::string(fileKey);
    }
    bool rtn = false;
    CSData* data = new CSData(buffer->data());
    if (!compression || data->compress()) {
        if (data->AESEncrypt(*key, true)) {
            if (data->writeToFile(CSFile::storagePath(subpath))) {
                rtn = true;
            }
            else {
                CSErrorLog("file error:%s", subpath);
            }
        }
        else {
            CSErrorLog("encrypt error:%s", subpath);
        }
    }
    else {
        CSErrorLog("compress error:%s", subpath);
    }
    data->release();
    key->hide();
    return rtn;
}

const char* Asset::replaySubPath(int64 matchRecordId) {
    return CSString::cstringWithFormat("replays/%" PRId64 ".xmf", matchRecordId);
}
