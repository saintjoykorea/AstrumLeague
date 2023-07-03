
#ifndef CSVungleAd_h
#define CSVungleAd_h

class CSVungleAdDelegate {
public:
    virtual void onVungleAdReward() = 0;
    virtual void onVungleAdClose() = 0;
    virtual void onVungleAdFail(const char* msg) = 0;
};

class CSVungleAd {
public:
    static void show(CSVungleAdDelegate* delegate);
    
    static void initialize();
    static void finalize();
};

#endif



