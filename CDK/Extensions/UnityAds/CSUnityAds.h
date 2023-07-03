
#ifndef CSUnityAds_h
#define CSUnityAds_h

class CSUnityAdsDelegate {
public:
    virtual void onUnityAdsReward() = 0;
    virtual void onUnityAdsClose() = 0;
    virtual void onUnityAdsFail(const char* msg) = 0;
};

class CSUnityAds {
public:
    static void show(CSUnityAdsDelegate* delegate);
    
    static void initialize(bool testMode);
    static void finalize();
};

#endif



