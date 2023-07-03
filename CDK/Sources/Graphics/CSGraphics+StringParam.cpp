#define CDK_IMPL

#include "CSGraphics.h"

#include "CSGraphicsImpl.h"

CSGraphics::StringParam::StringParam() : _display(NULL), _freeDisplay(false), start(0), end(0) {

}

CSGraphics::StringParam::StringParam(const char* str) : StringParam() {
	if (str) {
		_display = new CSStringDisplay(str);
		end = _display->characters()->count();
		_freeDisplay = true;
	}
}

CSGraphics::StringParam::StringParam(const CSString* str) : StringParam() {
	if (str) {
		_display = str->display();
		end = _display->characters()->count();
	}
}
CSGraphics::StringParam::StringParam(const CSString* str, uint offset, uint length) : StringParam() {
	if (str) {
		_display = str->display();
		if (length == 0 || offset >= _display->characters()->count()) {
			_display = NULL;
			return;
		}
		start = offset;
		end = CSMath::min(offset + length, _display->characters()->count());
	}
}

CSGraphics::StringParam::StringParam(const CSLocaleString* str) : StringParam(str->value()) {
	
}

CSGraphics::StringParam::StringParam(const StringParam& other) :
	_display(other._display),
	_freeDisplay(false),
	start(other.start),
	end(other.end)
{
	
}

CSGraphics::StringParam::~StringParam() {
	if (_freeDisplay) delete _display;
}
