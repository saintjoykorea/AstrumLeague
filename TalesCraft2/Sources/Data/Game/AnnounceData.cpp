#include "AnnounceData.h"

AnnounceData::AnnounceData(CSBuffer* buffer) {
	name = CSObject::retain(buffer->readLocaleString());
	description = CSObject::retain(buffer->readLocaleString());

	CSDictionary<CSString, SoundGroupIndex>* voices = new CSDictionary<CSString, SoundGroupIndex>();

	int count = buffer->readLength();
	for (int i = 0; i < count; i++) {
		SoundGroupIndex group(buffer);

		int localeCount = buffer->readLength();
		for (int j = 0; j < localeCount; j++) {
			voices->setObject(buffer->readString(), group);
		}
	}
	this->voices = voices;
}

AnnounceData::~AnnounceData() {
	name->release();
	description->release();
	voices->release();
}

SoundIndex AnnounceData::voice(AnnounceIndex index) const {
#ifdef UseVoiceLocale
	const CSString* locale = new CSString(UseVoiceLocale);
#else
	const CSString* locale = CSLocaleString::locale();
#endif
	const SoundGroupIndex* group = voices->tryGetObjectForKey(locale);

	if (group) return SoundIndex(*group, index);
		
	group = voices->tryGetObjectForKey(CSLocaleDefault);

	if (group) return SoundIndex(*group, index);

	return SoundIndex::None;
}
