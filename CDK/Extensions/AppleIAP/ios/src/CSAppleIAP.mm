//
//  CSAppleIAP.mm
//  CDK
//
//  Created by 김수현 on 2017. 11. 23..
//  Copyright © 2017년 brgames. All rights reserved.
//

#ifdef CDK_IOS

#define CDK_IMPL

#import "CSAppleIAP.h"

#import <StoreKit/StoreKit.h>

enum Error {
    ErrorUnabled = 30000,
    ErrorInvalidProduct,
    ErrorInvalidPayload
};

@interface CSAppleIAPImpl : NSObject<SKProductsRequestDelegate, SKPaymentTransactionObserver> {
    CSIAPDelegate* _delegate;
    NSMutableDictionary* _products;
    NSMutableDictionary* _payloads;
	BOOL _connected;
}

@property (readwrite, assign) CSIAPDelegate* delegate;

-(id)init;
-(void)dealloc;
-(void)connect;
-(void)sync:(NSSet*)productIds;
-(void)purchase:(NSString*)productId payload:(NSString*)payload;

@end

@implementation CSAppleIAPImpl

@synthesize delegate = _delegate;

-(id)init {
    if (self = [super init]) {
        _products = [[NSMutableDictionary alloc] init];
        _payloads = [[NSMutableDictionary alloc] init];
    }
    return self;
}

-(void)dealloc {
    [_products release];
    [_payloads release];
    
    if (_connected) [[SKPaymentQueue defaultQueue] removeTransactionObserver:self];
    
    [super dealloc];
}

-(void)connect {
    if ([SKPaymentQueue canMakePayments]) {
        if (!_connected) {
			[[SKPaymentQueue defaultQueue] addTransactionObserver:self];

			_connected = YES;
		}
        if (_delegate) _delegate->onIAPConnected();
    }
    else {
		NSLog(@"CSAppleIAP connect unabled");
		
        if (_delegate) _delegate->onIAPError(ErrorUnabled);
    }
}

-(void)sync:(NSSet*)productIds {
    SKProductsRequest* request = [[SKProductsRequest alloc] initWithProductIdentifiers:productIds];
    request.delegate = self;
    [request start];
    [request release];
}

-(void)notifySyncProduct:(SKProduct*)product {
	if (_delegate) {
		NSNumberFormatter *numberFormatter = [[NSNumberFormatter alloc] init];
            
        NSString* pricev = [numberFormatter stringFromNumber:product.price];
        
        [numberFormatter setNumberStyle:NSNumberFormatterCurrencyStyle];
		[numberFormatter setLocale:product.priceLocale];
		NSString* price = [numberFormatter stringFromNumber:product.price];

        NSString* nstr = [numberFormatter currencyCode];

		[numberFormatter release];
		
        //(const char *productId, const char *price, const char *originPrice, const char* currencyCode) {
        _delegate->onIAPSyncProduct(product.productIdentifier.UTF8String, price.UTF8String,pricev.UTF8String,  nstr.UTF8String);
	}
}

-(void)notifySync {
	if (_delegate) _delegate->onIAPSync();
}

-(void)productsRequest:(SKProductsRequest*)request didReceiveResponse:(SKProductsResponse*)response {
    for (SKProduct *product in response.products) {
        [_products setObject:product forKey:product.productIdentifier];
        
        if (_delegate) {
			[self performSelectorOnMainThread:@selector(notifySyncProduct:) withObject:product waitUntilDone:NO];
        }
    }
    if (_delegate) {
		[self performSelectorOnMainThread:@selector(notifySync) withObject:nil waitUntilDone:NO];
	}
}

-(void)notifyError:(NSNumber*)error {
	NSLog(@"CSAppleIAP error:%d", error.intValue);
	
	if (_delegate) _delegate->onIAPError(error.intValue);	
}

-(void)purchase:(NSString*)productId payload:(NSString*)payload {
    if ([SKPaymentQueue canMakePayments]) {
        SKProduct* product = [_products objectForKey:productId];
        
        if (product) {
            SKMutablePayment *payment = [SKMutablePayment paymentWithProduct:product];
            [_payloads setObject:payload forKey:payment];
            [[SKPaymentQueue defaultQueue] addPayment:payment];
        }
        else if (_delegate) {
			[self performSelectorOnMainThread:@selector(notifyError:) withObject:[NSNumber numberWithInt:ErrorInvalidProduct] waitUntilDone:NO];
        }
    }
    else if (_delegate) {
		[self performSelectorOnMainThread:@selector(notifyError:) withObject:[NSNumber numberWithInt:ErrorUnabled] waitUntilDone:NO];
    }
}

-(void)request:(SKRequest *)request didFailWithError:(NSError *)error {
	NSLog(@"CSAppleIAP request:didFailWithError:%@", error);
	[request cancel];
	[self performSelectorOnMainThread:@selector(notifyError:) withObject:[NSNumber numberWithInt:error.code] waitUntilDone:NO];
}

-(void)notifyComplete:(NSArray*)params {
	if (_delegate) {
		NSString* productIdentifier = [params objectAtIndex:0];
		NSString* receipt = [params objectAtIndex:1];
		NSString* transactionId = [params objectAtIndex:2];
		NSString* payload = [params objectAtIndex:3];
		
		_delegate->onIAPComplete(productIdentifier.UTF8String, receipt.UTF8String, transactionId.UTF8String, payload.UTF8String);
	}
}

-(void)purchasedTransaction:(SKPaymentTransaction *)transaction {
    if (_delegate) {
        NSString* payload = [_payloads objectForKey:transaction.payment];
        
        if (payload) {
            NSData *receiptData = [NSData dataWithContentsOfURL:[[NSBundle mainBundle] appStoreReceiptURL]];
            NSString* receipt = [receiptData base64EncodedStringWithOptions:0];
        
			NSMutableArray* params = [NSMutableArray arrayWithCapacity:4];
			[params addObject:transaction.payment.productIdentifier];
			[params addObject:receipt];
			[params addObject:transaction.transactionIdentifier];
			[params addObject:payload];
		
			[self performSelectorOnMainThread:@selector(notifyComplete:) withObject:params waitUntilDone:NO];
        }
        else {
			[self performSelectorOnMainThread:@selector(notifyError:) withObject:[NSNumber numberWithInt:ErrorInvalidPayload] waitUntilDone:NO];
        }
    }
    [_payloads removeObjectForKey:transaction.payment];
    
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void)restoreTransaction:(SKPaymentTransaction *)transaction {
	NSLog(@"CSAppleIAP restoreTransaction can not be called. consumable product only");
	
    [_payloads removeObjectForKey:transaction.payment];
    
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void)notifyCancelled {
	if (_delegate) _delegate->onIAPCancelled();
}

-(void)failedTransaction:(SKPaymentTransaction *)transaction {
    NSLog(@"CSAppleIAP failedTransaction");
    
    if (_delegate) {
        if (transaction.error.code == SKErrorPaymentCancelled) {
			[self performSelectorOnMainThread:@selector(notifyCancelled) withObject:nil waitUntilDone:NO];
        }
        else {
            NSLog(@"CSAppleIAP failedTransaction:%@", transaction.error.description);
            
			[self performSelectorOnMainThread:@selector(notifyError:) withObject:[NSNumber numberWithInt:transaction.error.code] waitUntilDone:NO];
        }
    }
    [_payloads removeObjectForKey:transaction.payment];
    
    [[SKPaymentQueue defaultQueue] finishTransaction:transaction];
}

-(void)paymentQueue:(SKPaymentQueue*)queue updatedTransactions:(NSArray*)transactions {
    for (SKPaymentTransaction *transaction in transactions) {
        switch(transaction.transactionState) {
            case SKPaymentTransactionStatePurchased : // 구매완료
                [self purchasedTransaction:transaction];
                break;
            case SKPaymentTransactionStateFailed : // 구매실패
                [self failedTransaction:transaction];
                break;
            case SKPaymentTransactionStateRestored : // 재구매
                [self restoreTransaction:transaction];
                break;
        }
    }
}

/*
////재구매
-(void)retoreItem {
    //재구매 버튼 클릭시 호출
    [[SKPaymentQueue defaultQueue] restoreCompletedTransactions];
}

//복원. 실패시
-(void)paymentQueue:(SKPaymentQueue *)queue restoreCompletedTransactionsFailedWithError:(NSError *)error{
    if(error.code!= SKErrorPaymentCancelled) {
        //error
    }
    else {
        //cancel
    }
}

//복원 완료시
-(void)paymentQueueRestoreCompletedTransactionsFinished:(SKPaymentQueue *)queue {
    if(queue.transactions.count == 0) {
        //구매한 영구 아이템이 없을 경우
        NSString *failMessage;
        failMessage = @"no record of your purchase";
        return;
    }
    
    for(SKPaymentTransaction *transaction in queue.transactions) {
        //구매한 아이템들에 대한 복원 처리.
        NSString *productId = transaction.payment.productIdentifier;
        //서버에 아이템 복원 요청.
    }
}
*/

@end

static CSAppleIAPImpl* __sharedImpl = nil;

void CSAppleIAP::initialize() {
    if (!__sharedImpl) {
        __sharedImpl = [[CSAppleIAPImpl alloc] init];
    }
}

void CSAppleIAP::finalize() {
    if (__sharedImpl) {
        [__sharedImpl release];
        __sharedImpl = nil;
    }
}

void CSAppleIAP::setDelegate(CSIAPDelegate* delegate) {
    [__sharedImpl setDelegate:delegate];
}

void CSAppleIAP::connect() {
    [__sharedImpl connect];
}

void CSAppleIAP::sync(const char *const *productIds, uint count) {
    NSMutableSet* set = [[NSMutableSet alloc] initWithCapacity:count];
    for (int i = 0; i < count; i++) {
        [set addObject:[NSString stringWithUTF8String:productIds[i]]];
    }
    [__sharedImpl sync:set];
    [set release];
}

void CSAppleIAP::purchase(const char* productId, const char* payload) {
    [__sharedImpl purchase:[NSString stringWithUTF8String:productId] payload:[NSString stringWithUTF8String:payload]];
}
#endif

