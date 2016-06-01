

#ifndef __THREAD_POOL_HPP__

#include <pthread.h>
#include <errno.h>
#include <string>
#include <iostream>
#include <deque>
#include <map>

using namespace std;

class Sync
{
public:
	Sync()
	{
		pthread_mutex_init(&mMutex, NULL);
		pthread_cond_init(&mCond, NULL);
		pthread_mutex_lock(&mMutex);
	}
	~Sync()
	{
		pthread_mutex_destroy(&mMutex);
		pthread_cond_destroy(&mCond);
		pthread_mutex_unlock(&mMutex);
	}

public:
	void signal()
	{
		pthread_mutex_lock(&mMutex);
		pthread_cond_broadcast(&mCond);
		pthread_mutex_unlock(&mMutex);
	}

	void wait()
	{
		pthread_cond_wait(&mCond, &mMutex);
	}

private:
	pthread_cond_t mCond;
	pthread_mutex_t mMutex;
};

class Context
{
public:
	enum Type {
		ECT_job  = 0,
		ECT_stop,
	};

public:
	Context(int type, std::string arg):mType(type), mArg(arg){}
	~Context()
	{}
public:
	int mType;
	string mArg;
};

typedef struct _ThreadData
{
	void * pool;
	Sync * wait;
}ThreadData;

class ThreadPool
{

public:
	ThreadPool(int threadNum):mThreadNum(threadNum),mRunning(false)
	{
	}
	~ThreadPool()
	{
	}

private:
	int Init()
	{
		int err = 0;
		pthread_mutexattr_t attr;
		pthread_mutexattr_init(&attr);
		pthread_mutexattr_settype(&attr, PTHREAD_MUTEX_RECURSIVE);
		pthread_mutex_init(&mMutex, &attr);
		pthread_cond_init(&mCond, NULL);
		pthread_mutexattr_destroy(&attr);

		/*set running*/
		mRunning = true;
		return err;
	}

	int Finit()
	{
		pthread_mutex_destroy(&mMutex);
		pthread_cond_destroy(&mCond);

		/*join threads*/
	}

public:
	/*return NULL if pool not runnig any more*/
	Context *GetTask()
	{
		Context *ctx = NULL;
		pthread_mutex_lock(&mMutex);
		while (mQueue.size() <= 0 && mRunning) {
			pthread_cond_wait(&mCond, &mMutex);
		}

		if (mRunning) {
			cout<< "get a task" << std::endl;
			ctx = mQueue.front();
			mQueue.pop_front();
		}
		pthread_mutex_unlock(&mMutex);
		return ctx;
	}

	int StartPool();
	int StopPool();
	int Enque(Context* ctx);

public:
	void Lock()
	{
		pthread_mutex_lock(&mMutex);
	}

	void UnLock()
	{
		pthread_mutex_unlock(&mMutex);
	}

	int GetTaskSize()
	{
		int size = 0;
		Lock();
		size = mQueue.size();
		UnLock();
		return size;
	}

public:
	static void * ThreadRouteTimte(void *arg);

private:
	/*context queue*/
	std::deque<Context*> mQueue;

	/*thread map*/
	std::map<pthread_t, ThreadData*> mThreads;

	/*thread num*/
	int mThreadNum;

	/*condt*/
	pthread_cond_t  mCond;

	/*mutex lock*/
	pthread_mutex_t mMutex;

	/*pool runnig*/
	bool mRunning;
};

#endif


