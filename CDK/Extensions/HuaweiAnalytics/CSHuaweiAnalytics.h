
#ifndef CSHuaweiAnalytics_h
#define CSHuaweiAnalytics_h

class CSHuaweiAnalytics {
public:
    static void initialize();
    static void finalize();
    
    static void log(const char* name, int paramCount, ...);
};

#endif /* CSHuaweiAnalytics_h */
