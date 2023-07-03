//
//  CSDevice.cpp
//  CDK
//
//  Created by chan on 13. 4. 11..
//  Copyright (c) 2013년 brgames. All rights reserved.
//
//#import "stdafx.h"

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSDevice.h"

#import "CSLog.h"

#import <sys/utsname.h> // import it in your header or implementation file.
#import <mach/mach.h>

#import <CoreTelephony/CTCarrier.h>
#import <CoreTelephony/CTTelephonyNetworkInfo.h>

const char* CSDevice::brand() {
    return "Apple";
}

const char* CSDevice::model() {
    struct utsname systemInfo;
    uname(&systemInfo);
    NSString* model = [NSString stringWithCString:systemInfo.machine encoding:NSUTF8StringEncoding];
    
    return model.UTF8String;
}

const char* CSDevice::locale() {
	NSLocale* locale = [NSLocale currentLocale];
    
	NSString* code = [locale objectForKey:NSLocaleLanguageCode];
    
	if ([code isEqualToString:@"zh"]) {		//add country code for another language if necessary (ex:en-US...)
		code = [NSString stringWithFormat:@"%@-%@", code, [locale objectForKey:NSLocaleCountryCode]];
    }

    return code.UTF8String;
}

const char* CSDevice::countryCode(){
	CTTelephonyNetworkInfo *networkInfo = [[[CTTelephonyNetworkInfo alloc] init] autorelease];
	
	NSString* countryCode = nil;
	if (@available(iOS 12.0, *)) {
		NSDictionary<NSString*, CTCarrier*>* carriers = networkInfo.serviceSubscriberCellularProviders;
		for (NSString* key in carriers) {
			CTCarrier* carrier = carriers[key];
			countryCode = carrier.isoCountryCode;
			if (countryCode) break;
		}
	}
	else {
        CTCarrier* carrier = networkInfo.subscriberCellularProvider;
		countryCode = carrier.isoCountryCode;
	}
	if (!countryCode) countryCode = [[NSLocale currentLocale] objectForKey:NSLocaleCountryCode];
	
    return countryCode.UTF8String;
}

const char* CSDevice::systemVersion() {
	return [[[UIDevice currentDevice] systemVersion] UTF8String];
}

const char* CSDevice::uuid() {
	return [[[NSUUID UUID] UUIDString] UTF8String];
}

CSMemoryUsage CSDevice::memoryUsage() {
    vm_size_t page_size;
    mach_port_t host_port;
    mach_msg_type_number_t host_size;
    vm_statistics_data_t vm_stat;
    
    host_port = mach_host_self();
    host_size = sizeof(vm_statistics_data_t) / sizeof(integer_t);
    
    CSMemoryUsage usage;
    
    static const int64 totalMemory = [NSProcessInfo processInfo].physicalMemory;
    
    usage.totalMemory = totalMemory;
    if (host_page_size(host_port, &page_size) == KERN_SUCCESS && host_statistics(host_port, HOST_VM_INFO, (host_info_t)&vm_stat, &host_size) == KERN_SUCCESS) {
        usage.freeMemory = (int64_t)(vm_stat.inactive_count + vm_stat.free_count) * page_size;
    }
    else {
        usage.freeMemory = 0;
    }
    usage.threshold = 64 * 1024768;
    return usage;
}

int CSDevice::battery() {
    UIDevice* device = [UIDevice currentDevice];
    [device setBatteryMonitoringEnabled:YES]; // 필수
    return [device batteryLevel] * 100;
}

#endif

