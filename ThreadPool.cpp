
#include <assert.h>
#include "ThreadPool.hpp"



void * ThreadPool::ThreadRouteTimte(void *arg)
{
	ThreadData *data = (ThreadData *)arg;
    ThreadPool *pool = (ThreadPool *)data->pool;
	Context * job = NULL;
	
	cout << "thread start " << pthread_self() <<std::endl;

	/*signal thread pool main thread to create next thread*/
	data->wait->signal();
	data->wait = NULL;

	while ((job = pool->GetTask()) != NULL) {
		/*do this job*/
		cout << "thread " << pthread_self() << " handling job: " << job->mArg<<std::endl;
		delete job;
		job = NULL;
	}

	/*job == NULL, this means that thread pool will exit*/
	cout << "thread " << pthread_self() << " exits!" << std::endl;
}

int ThreadPool::StartPool()
{
	int err = 0;
	pthread_t id;
	ThreadData *data = NULL;

	do {
		err = Init();
		if (err != 0) {
			break;
		}

		for (int i=0; i < mThreadNum; ++i) {
			Sync sync;
			data = new ThreadData;
			data->pool = this;
			data->wait = &sync;
			err = pthread_create(&id, NULL, ThreadPool::ThreadRouteTimte, data);
			assert(err == 0);
			mThreads.insert(std::make_pair(id, data));
			data = NULL;
			sync.wait();
		}
	} while(0);

	return err;
}

int ThreadPool::StopPool()
{
	pthread_mutex_lock(&mMutex);
	mRunning = false;
	pthread_cond_broadcast(&mCond);
	pthread_mutex_unlock(&mMutex);
	
	/*mThreads will not be changed cause mRunnig is false*/
	/*join thread and delte ThreadData*/
	std::map<pthread_t, ThreadData *>::iterator iter;
	for (iter = mThreads.begin(); iter != mThreads.end(); ++iter) {
		cout<< "try to join thread " << iter->first<<std::endl;
		pthread_join(iter->first, NULL);
		if (iter->second != NULL) {
			delete iter->second;
			iter->second = NULL;
		}
	}

	/*free context that is not handled*/
	while (mQueue.size() > 0) {
		Context *ctx = mQueue.front();
		if (NULL != ctx) {
			delete ctx;
			ctx = NULL;
		}
		mQueue.pop_front();
	}

	Finit();
}

int ThreadPool::Enque(Context* ctx)
{
	pthread_mutex_lock(&mMutex);
	if (NULL != ctx) {
		mQueue.push_back(ctx);
		pthread_cond_signal(&mCond);
		cout<<"enque context!"<<std::endl;
	}
	pthread_mutex_unlock(&mMutex);
}


