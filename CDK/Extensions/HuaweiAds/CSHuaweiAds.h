
#ifndef CSHuaweiAds_h
#define CSHuaweiAds_h

class CSHuaweiAdsDelegate {
public:
    virtual void onHuaweiAdsReward() = 0;
    virtual void onHuaweiAdsClose() = 0;
    virtual void onHuaweiAdsFail(const char* msg) = 0;
};

class CSHuaweiAds {
public:
    static void show(CSHuaweiAdsDelegate* delegate);
    
    static void initialize(bool testMode);
    static void finalize();
};

#endif



