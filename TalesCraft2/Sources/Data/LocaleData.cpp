#include "LocaleData.h"

LocaleData::LocaleData(CSBuffer* buffer) :
	codes(CSObject::retain(buffer->readArrayFuncWithParam<CSString>(&CSString::stringWithBuffer, CSStringEncodingUTF8))),
	name(CSObject::retain(buffer->readString())),
	digitGroupSeperator(buffer->readByte())
{

}

LocaleData::~LocaleData() {
	codes->release();
	name->release();
}
