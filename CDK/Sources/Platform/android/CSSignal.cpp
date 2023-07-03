//
//  CSSignal.cpp
//  CDK
//
//  Created by 김찬 on 2015. 3. 17..
//  Copyright (c) 2015년 brgames. All rights reserved.
//

#ifdef CDK_ANDROID

#define CDK_IMPL

#include "CSLog.h"
#include "CSFile.h"
#include "CSApplication.h"
#include "CSDevice.h"
#include "CSSignal.h"

#include <android/log.h>

#define UNW_LOCAL_ONLY
#include "libunwind.h"
#include "demangle.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <sys/types.h>
#include <signal.h>
#if defined(__arm__) && !defined(__BIONIC_HAVE_UCONTEXT_T) && !defined(__BIONIC_HAVE_STRUCT_SIGCONTEXT)
#include <asm/sigcontext.h>
#endif
#include <asm/signal.h>
#include <dlfcn.h>

#define SIG_CODE_MAX    32

static const int sig_codes[] = {
    SIGABRT, SIGILL, SIGTRAP, SIGBUS, SIGFPE, SIGSEGV
#ifdef SIGSTKFLT
    , SIGSTKFLT
#endif
};

const char* const CSSignal::FileName = "signal.txt";
const char* CSSignal::filePath() {
	return CSFile::storagePath(FileName);
}

struct {
    struct sigaction sa_old[SIG_CODE_MAX];
    char brand[32];
    char model[32];
    char systemVersion[32];
    char path[256];
    char log[4096];
    int logLen;
#if defined(__arm__)
    unw_context_t unw_context;
#endif
    unw_cursor_t unw_cursor;
}_context;

static const char* descSignal(int sig, int code) {
    switch(sig) {
        case SIGILL:
            switch(code) {
                case ILL_ILLOPC:
                    return "Illegal opcode";
                case ILL_ILLOPN:
                    return "Illegal operand";
                case ILL_ILLADR:
                    return "Illegal addressing mode";
                case ILL_ILLTRP:
                    return "Illegal trap";
                case ILL_PRVOPC:
                    return "Privileged opcode";
                case ILL_PRVREG:
                    return "Privileged register";
                case ILL_COPROC:
                    return "Coprocessor error";
                case ILL_BADSTK:
                    return "Internal stack error";
                default:
                    return "Illegal operation";
            }
            break;
        case SIGFPE:
            switch(code) {
                case FPE_INTDIV:
                    return "Integer divide by zero";
                case FPE_INTOVF:
                    return "Integer overflow";
                case FPE_FLTDIV:
                    return "Floating-point divide by zero";
                case FPE_FLTOVF:
                    return "Floating-point overflow";
                case FPE_FLTUND:
                    return "Floating-point underflow";
                case FPE_FLTRES:
                    return "Floating-point inexact result";
                case FPE_FLTINV:
                    return "Invalid floating-point operation";
                case FPE_FLTSUB:
                    return "Subscript out of range";
                default:
                    return "Floating-point";
            }
            break;
        case SIGSEGV:
            switch(code) {
                case SEGV_MAPERR:
                    return "Address not mapped to object";
                case SEGV_ACCERR:
                    return "Invalid permissions for mapped object";
                default:
                    return "Segmentation violation";
            }
            break;
        case SIGBUS:
            switch(code) {
                case BUS_ADRALN:
                    return "Invalid address alignment";
                case BUS_ADRERR:
                    return "Nonexistent physical address";
                case BUS_OBJERR:
                    return "Object-specific hardware error";
                default:
                    return "Bus error";
            }
            break;
        case SIGTRAP:
            switch(code) {
                case TRAP_BRKPT:
                    return "Process breakpoint";
                case TRAP_TRACE:
                    return "Process trace trap";
                default:
                    return "Trap";
            }
            break;
        case SIGCHLD:
            switch(code) {
                case CLD_EXITED:
                    return "Child has exited";
                case CLD_KILLED:
                    return "Child has terminated abnormally and did not create a core file";
                case CLD_DUMPED:
                    return "Child has terminated abnormally and created a core file";
                case CLD_TRAPPED:
                    return "Traced child has trapped";
                case CLD_STOPPED:
                    return "Child has stopped";
                case CLD_CONTINUED:
                    return "Stopped child has continued";
                default:
                    return "Child";
            }
            break;
        case SIGPOLL:
            switch(code) {
                case POLL_IN:
                    return "Data input available";
                case POLL_OUT:
                    return "Output buffers available";
                case POLL_MSG:
                    return "Input message available";
                case POLL_ERR:
                    return "I/O error";
                case POLL_PRI:
                    return "High priority input available";
                case POLL_HUP:
                    return "Device disconnected";
                default:
                    return "Pool";
            }
            break;
        case SIGABRT:
            return "Process abort signal";
        case SIGALRM:
            return "Alarm clock";
        case SIGCONT:
            return "Continue executing, if stopped";
        case SIGHUP:
            return "Hangup";
        case SIGINT:
            return "Terminal interrupt signal";
        case SIGKILL:
            return "Kill";
        case SIGPIPE:
            return "Write on a pipe with no one to read it";
        case SIGQUIT:
            return "Terminal quit signal";
        case SIGSTOP:
            return "Stop executing";
        case SIGTERM:
            return "Termination signal";
        case SIGTSTP:
            return "Terminal stop signal";
        case SIGTTIN:
            return "Background process attempting read";
        case SIGTTOU:
            return "Background process attempting write";
        case SIGUSR1:
            return "User-defined signal 1";
        case SIGUSR2:
            return "User-defined signal 2";
        case SIGPROF:
            return "Profiling timer expired";
        case SIGSYS:
            return "Bad system call";
        case SIGVTALRM:
            return "Virtual timer expired";
        case SIGURG:
            return "High bandwidth data is available at a socket";
        case SIGXCPU:
            return "CPU time limit exceeded";
        case SIGXFSZ:
            return "File size limit exceeded";
        default:
            switch(code) {
                case SI_USER:
                    return "Signal sent by kill()";
                case SI_QUEUE:
                    return "Signal sent by the sigqueue()";
                case SI_TIMER:
                    return "Signal generated by expiration of a timer set by timer_settime()";
                case SI_ASYNCIO:
                    return "Signal generated by completion of an asynchronous I/O request";
                case SI_MESGQ:
                    return
                    "Signal generated by arrival of a message on an empty message queue";
                default:
                    return "Unknown signal";
            }
            break;
    }
}

static void log(const char* format, ...) {
    if (_context.logLen < sizeof(_context.log)) {
        va_list args;
        va_start(args, format);
        int len = vsnprintf(_context.log + _context.logLen, sizeof(_context.log) - _context.logLen, format, args);
        if (len > 0) _context.logLen += len;
        va_end(args);
    }
}

static bool logStack(ucontext_t* sc) {
#if defined(__arm__)
    unw_getcontext(&_context.unw_context);

    if (unw_init_local(&_context.unw_cursor, &_context.unw_context)) {
        return false;
    }
    const sigcontext* sc_uc = &(sc->uc_mcontext);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R0, sc_uc->arm_r0);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R1, sc_uc->arm_r1);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R2, sc_uc->arm_r2);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R3, sc_uc->arm_r3);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R4, sc_uc->arm_r4);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R5, sc_uc->arm_r5);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R6, sc_uc->arm_r6);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R7, sc_uc->arm_r7);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R8, sc_uc->arm_r8);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R9, sc_uc->arm_r9);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R10, sc_uc->arm_r10);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R11, sc_uc->arm_fp);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R12, sc_uc->arm_ip);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R13, sc_uc->arm_sp);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R14, sc_uc->arm_lr);
    unw_set_reg(&_context.unw_cursor, UNW_ARM_R15, sc_uc->arm_pc);
    unw_set_reg(&_context.unw_cursor, UNW_REG_IP, sc_uc->arm_pc);
    unw_set_reg(&_context.unw_cursor, UNW_REG_SP, sc_uc->arm_sp);
#else
    if (unw_init_local(&_context.unw_cursor, sc)) {
        return false;
    }
#endif
    do {
        unw_word_t ip;
        unw_get_reg(&_context.unw_cursor, UNW_REG_IP, &ip);

        Dl_info info;
        if (dladdr((void *)ip, &info)) {
            const char *fname = info.dli_fname;
            if (fname) {
                fname += strlen(fname);
                while (*fname != '/' && fname > info.dli_fname) fname--;
            }
			
			void* pc = (void*)((uint64)ip - (uint64)info.dli_fbase);
			
            if (info.dli_sname) {
                const char* sname = info.dli_sname;
                char demangled[128];
                if (Demangle(sname, demangled, sizeof(demangled))) {
                    sname = demangled;
                }
                log("at %s/%s + %p (%p)\n", fname, sname, (void*)((uint64)ip - (uint64)info.dli_saddr), pc);
            }
            else {
                log("at %s (%p)\n", fname, pc);
            }
        }
        else {
            log("at unknown\n");
        }
    } while (unw_step(&_context.unw_cursor) > 0);

    return true;
}

static void handleSignal(int code, siginfo_t* si, void* sc) {
#if defined(__arm__)
    #if defined(__ARM_ARCH_7A__)
        #if defined(__ARM_NEON__)
            #if defined(__ARM_PCS_VFP)
                #define ABI "armeabi-v7a/NEON (hard-float)"
            #else
                #define ABI "armeabi-v7a/NEON"
            #endif
        #else
            #if defined(__ARM_PCS_VFP)
                #define ABI "armeabi-v7a (hard-float)"
            #else
                #define ABI "armeabi-v7a"
            #endif
        #endif
    #else
        #define ABI "armeabi"
    #endif
#elif defined(__aarch64__)
    #define ABI "arm64-v8a"
#elif defined(__i386__)
    #define ABI "x86"
#elif defined(__x86_64__)
    #define ABI "x86_64"
#endif
    log("platform:android\n");
    log("device:%s %s " ABI " %s\n", _context.brand, _context.model, _context.systemVersion);
    log("app version:%s\n", (const char*)*CSApplication::sharedApplication()->version());
    log("signal %d (%s)", si->si_signo, descSignal(si->si_signo, si->si_code));
    if (si->si_errno) {
        log(" : %s", strerror(si->si_errno));
    }
    log("\n");

    if (!logStack((ucontext_t*)sc)) {
        log("unabled to dump stack\n");
    }

    CSFile::writeRawToFile(_context.path, _context.log, _context.logLen, false);

    //CSErrorLog(_context.log);
    __android_log_print(ANDROID_LOG_ERROR, "CSSignal.cpp", _context.log);

    if (_context.sa_old[code].sa_sigaction) {
        _context.sa_old[code].sa_sigaction(code, si, sc);
    }
    else if (_context.sa_old[code].sa_handler) {
        _context.sa_old[code].sa_handler(code);
    }
    CSSignal::finalize();
    exit(-1);
}

void CSSignal::initialize() {
    struct sigaction act;
    memset(&act, 0, sizeof(act));
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handleSignal;
    act.sa_flags = SA_SIGINFO;

    strcpy(_context.brand, CSDevice::brand());
    strcpy(_context.model, CSDevice::model());
    strcpy(_context.systemVersion, CSDevice::systemVersion());
    strcpy(_context.path, CSSignal::filePath());
    _context.logLen = 0;

    for (int i = 0; i < countof(sig_codes); i++) {
        const int sig = sig_codes[i];
        
        if (sigaction(sig, &act, &_context.sa_old[sig]) != 0) {
            CSErrorLog("setup signal failed");
        }
    }
}

void CSSignal::finalize() {
    for (int i = 0; i < countof(sig_codes); i++) {
        const int sig = sig_codes[i];

        sigaction(sig, &_context.sa_old[sig], NULL);
    }
}

const char* CSSignal::readLog() {
    return *CSString::stringWithContentOfFile(filePath());
}
void CSSignal::deleteLog() {
    CSFile::deleteFile(filePath());
}

#endif

