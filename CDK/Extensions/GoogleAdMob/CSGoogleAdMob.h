
#ifndef CSGoogleAdMob_h
#define CSGoogleAdMob_h

class CSGoogleAdMobDelegate {
public:
    virtual void onGoogleAdMobReward() = 0;
    virtual void onGoogleAdMobClose() = 0;
    virtual void onGoogleAdMobFail(const char* msg) = 0;
};

class CSGoogleAdMob {
public:
    static void show(CSGoogleAdMobDelegate* delegate);
    
    static void initialize();
    static void finalize();
};

#endif



