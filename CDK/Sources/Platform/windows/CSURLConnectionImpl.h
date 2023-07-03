#ifdef CDK_WINDOWS

#ifndef __CDK__CSURLConnectionImpl__
#define __CDK__CSURLConnectionImpl__

#include "CSURLConnection.h"

#include "CSURLRequestImpl.h"

#include <vector>
#include <pthread.h>
#include <curl/curl.h>

enum CSURLConnectionHandleState {
	CSURLConnectionHandleStateNone,
	CSURLConnectionHandleStateQueue,
	CSURLConnectionHandleStateWaiting,
	CSURLConnectionHandleStateReceiving
};

struct CSURLConnectionHandle {
	CSURLRequestHandle* request;
	CSURLConnection* target;
	CSURLConnectionHandleState state;
	CURL* curl;
	uint readLength;
	uint writeLength;

	CSURLConnectionHandle(CSURLRequestHandle* request, CSURLConnection* target);

private:
	CSURLConnectionHandle(const CSURLConnectionHandle&);
	CSURLConnectionHandle& operator=(const CSURLConnectionHandle&);
};

class CSURLConnectionHandleManager {
private:
	struct Thread {
		CSURLConnectionHandleManager* parent;
		pthread_t tid;
		CSLock lock;
		bool active;
		bool alive;
	};
	CURLSH* _share;
	CSLock _connLock;
	CSLock _queueLock;
	CSLock _threadLock;
	std::vector<Thread*> _threads;
	std::vector<CSURLConnectionHandle*> _queue;

	static CSURLConnectionHandleManager* __sharedManager;

	CSURLConnectionHandleManager();
	~CSURLConnectionHandleManager();
public:
	static inline CSURLConnectionHandleManager* sharedManager() {
		return __sharedManager;
	}
	static void initialize();
	static void finalize();

	void start(CSURLConnectionHandle* handle);
	void cancel(CSURLConnectionHandle* handle);
private:
	static void lock_cb(CURL *handle, curl_lock_data data, curl_lock_access access, void *userptr);
	static void unlock_cb(CURL *handle, curl_lock_data data, void *userptr);

	static void* run(void* param);
public:
	static CSData* sendSynchronizedRequest(const CSURLRequestHandle* request, int* statusCode);
};

#endif
#endif