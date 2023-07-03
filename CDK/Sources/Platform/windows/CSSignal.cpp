#ifdef CDK_WINDOWS

#define CDK_IMPL

#include "CSSignal.h"

#include "CSFile.h"

#include "CSApplication.h"

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <string>
#include <sstream>
#include <vector>
#include <Psapi.h>
#include <algorithm>
#include <iterator>

#pragma pack( push, before_imagehlp, 8 )
#include <imagehlp.h>
#pragma pack( pop, before_imagehlp )

const char* const CSSignal::FileName = "signal.txt";

static char* __path;

const char* CSSignal::filePath() {
	return __path;
}

struct module_data {
	std::string image_name;
	std::string module_name;
	void *base_address;
	DWORD load_size;
};

class symbol {
	typedef IMAGEHLP_SYMBOL64 sym_type;
	sym_type *sym;
	static const int max_name_len = 1024;

public:
	symbol(HANDLE process, DWORD64 address) : sym((sym_type *)::operator new(sizeof(*sym) + max_name_len)) {
		memset(sym, '\0', sizeof(*sym) + max_name_len);
		sym->SizeOfStruct = sizeof(*sym);
		sym->MaxNameLength = max_name_len;
		DWORD64 displacement;

		SymGetSymFromAddr64(process, address, &displacement, sym);
	}

	std::string name() { return std::string(sym->Name); }
	std::string undecorated_name() {
		if (*sym->Name == '\0')
			return "<couldn't map PC to fn name>";
		std::vector<char> und_name(max_name_len);
		UnDecorateSymbolName(sym->Name, &und_name[0], max_name_len, UNDNAME_COMPLETE);
		return std::string(&und_name[0], strlen(&und_name[0]));
	}
};


class get_mod_info {
	HANDLE process;
	static const int buffer_length = 4096;
public:
	get_mod_info(HANDLE h) : process(h) {}

	module_data operator()(HMODULE module) {
		module_data ret;
		char temp[buffer_length];
		MODULEINFO mi;

		GetModuleInformation(process, module, &mi, sizeof(mi));
		ret.base_address = mi.lpBaseOfDll;
		ret.load_size = mi.SizeOfImage;

		GetModuleFileNameEx(process, module, temp, sizeof(temp));
		ret.image_name = temp;
		GetModuleBaseName(process, module, temp, sizeof(temp));
		ret.module_name = temp;
		std::vector<char> img(ret.image_name.begin(), ret.image_name.end());
		std::vector<char> mod(ret.module_name.begin(), ret.module_name.end());
		SymLoadModule64(process, 0, &img[0], &mod[0], (DWORD64)ret.base_address, ret.load_size);
		return ret;
	}
};

static LONG WINAPI exceptionHandler(EXCEPTION_POINTERS * ep) {
	CSFile::finalize();

	remove(__path);

	FILE* fp = fopen(__path, "w");

	fprintf(fp, "platform:windows\n");
	fprintf(fp, "device:windows\n");
	fprintf(fp, "app version:%s\n", (const char*)*CSApplication::sharedApplication()->version());
	fprintf(fp, "signal:%d (", ep->ExceptionRecord->ExceptionCode);

	switch (ep->ExceptionRecord->ExceptionCode) {
		case EXCEPTION_ACCESS_VIOLATION:
			fprintf(fp, "EXCEPTION_ACCESS_VIOLATION");
			break;
		case EXCEPTION_ARRAY_BOUNDS_EXCEEDED:
			fprintf(fp, "EXCEPTION_ARRAY_BOUNDS_EXCEEDED");
			break;
		case EXCEPTION_BREAKPOINT:
			fprintf(fp, "EXCEPTION_BREAKPOINT");
			break;
		case EXCEPTION_DATATYPE_MISALIGNMENT:
			fprintf(fp, "EXCEPTION_DATATYPE_MISALIGNMENT");
			break;
		case EXCEPTION_FLT_DENORMAL_OPERAND:
			fprintf(fp, "EXCEPTION_FLT_DENORMAL_OPERAND");
			break;
		case EXCEPTION_FLT_DIVIDE_BY_ZERO:
			fprintf(fp, "EXCEPTION_FLT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_FLT_INEXACT_RESULT:
			fprintf(fp, "EXCEPTION_FLT_INEXACT_RESULT");
			break;
		case EXCEPTION_FLT_INVALID_OPERATION:
			fprintf(fp, "EXCEPTION_FLT_INVALID_OPERATION");
			break;
		case EXCEPTION_FLT_OVERFLOW:
			fprintf(fp, "EXCEPTION_FLT_OVERFLOW");
			break;
		case EXCEPTION_FLT_STACK_CHECK:
			fprintf(fp, "EXCEPTION_FLT_STACK_CHECK");
			break;
		case EXCEPTION_FLT_UNDERFLOW:
			fprintf(fp, "EXCEPTION_FLT_UNDERFLOW");
			break;
		case EXCEPTION_ILLEGAL_INSTRUCTION:
			fprintf(fp, "EXCEPTION_ILLEGAL_INSTRUCTION");
			break;
		case EXCEPTION_IN_PAGE_ERROR:
			fprintf(fp, "EXCEPTION_IN_PAGE_ERROR");
			break;
		case EXCEPTION_INT_DIVIDE_BY_ZERO:
			fprintf(fp, "EXCEPTION_INT_DIVIDE_BY_ZERO");
			break;
		case EXCEPTION_INT_OVERFLOW:
			fprintf(fp, "EXCEPTION_INT_OVERFLOW");
			break;
		case EXCEPTION_INVALID_DISPOSITION:
			fprintf(fp, "EXCEPTION_INVALID_DISPOSITION");
			break;
		case EXCEPTION_NONCONTINUABLE_EXCEPTION:
			fprintf(fp, "EXCEPTION_NONCONTINUABLE_EXCEPTION");
			break;
		case EXCEPTION_PRIV_INSTRUCTION:
			fprintf(fp, "EXCEPTION_PRIV_INSTRUCTION");
			break;
		case EXCEPTION_SINGLE_STEP:
			fprintf(fp, "EXCEPTION_SINGLE_STEP");
			break;
		case EXCEPTION_STACK_OVERFLOW:
			fprintf(fp, "EXCEPTION_STACK_OVERFLOW");
			break;
		default:
			fprintf(fp, "EXCEPTION_UNKNOWN");
			break;
	}
	fprintf(fp, ")\n");

	HANDLE process = GetCurrentProcess();

	if (SymInitialize(process, NULL, false)) {
		HANDLE hThread = GetCurrentThread();
		IMAGEHLP_LINE64 line = {};
		std::vector<module_data> modules;
		DWORD cbNeeded;
		std::vector<HMODULE> module_handles(1);

		DWORD symOptions = SymGetOptions();
		symOptions |= SYMOPT_LOAD_LINES | SYMOPT_UNDNAME;
		SymSetOptions(symOptions);
		EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);
		module_handles.resize(cbNeeded / sizeof(HMODULE));
		EnumProcessModules(process, &module_handles[0], module_handles.size() * sizeof(HMODULE), &cbNeeded);
		std::transform(module_handles.begin(), module_handles.end(), std::back_inserter(modules), get_mod_info(process));
		void *base = modules[0].base_address;

		CONTEXT* context = ep->ContextRecord;
#ifdef _M_X64
		STACKFRAME64 frame;
		frame.AddrPC.Offset = context->Rip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context->Rsp;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context->Rbp;
		frame.AddrFrame.Mode = AddrModeFlat;
#else
		STACKFRAME64 frame;
		frame.AddrPC.Offset = context->Eip;
		frame.AddrPC.Mode = AddrModeFlat;
		frame.AddrStack.Offset = context->Esp;
		frame.AddrStack.Mode = AddrModeFlat;
		frame.AddrFrame.Offset = context->Ebp;
		frame.AddrFrame.Mode = AddrModeFlat;
#endif
		line.SizeOfStruct = sizeof line;
		IMAGE_NT_HEADERS *h = ImageNtHeader(base);
		DWORD image_type = h->FileHeader.Machine;
		DWORD offset_from_symbol = 0;
		int n = 0;

		std::ostringstream builder;
		do {
			if (frame.AddrPC.Offset != 0) {
				std::string fnName = symbol(process, frame.AddrPC.Offset).undecorated_name();
				
				builder << "at ";
				builder << fnName;
				if (SymGetLineFromAddr64(process, frame.AddrPC.Offset, &offset_from_symbol, &line)) {
					std::string filename = line.FileName;
					const size_t last_slash_idx = filename.find_last_of("\\/");
					if (std::string::npos != last_slash_idx) filename.erase(0, last_slash_idx + 1);
					builder << " (" << filename << ":" << line.LineNumber << ")\n";
				}
				else builder << "\n";

				if (fnName == "main" || fnName == "RaiseException") break;
			}
			else builder << "at null\n";
			
			if (!StackWalk64(image_type, process, hThread, &frame, context, NULL, SymFunctionTableAccess64, SymGetModuleBase64, NULL)) break;
				
			if (++n > 10) break;

		} while (frame.AddrReturn.Offset != 0);

		fprintf(fp, builder.str().c_str());
	}
	else {
		fprintf(fp, "unable to dump stack\n");
	}
	fclose(fp);

	SymCleanup(process);

	return EXCEPTION_EXECUTE_HANDLER;
}

void CSSignal::initialize() {
	__path = strdup(CSFile::storagePath(FileName));

	SetUnhandledExceptionFilter(&exceptionHandler);
}

void CSSignal::finalize() {
	free(__path);
}

const char* CSSignal::readLog() {
	return *CSString::stringWithContentOfFile(__path);
}
void CSSignal::deleteLog() {
	CSFile::deleteFile(__path);
}

#endif