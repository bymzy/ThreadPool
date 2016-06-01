

#include <unistd.h>
#include <sstream>
#include "ThreadPool.hpp"

using namespace std;

std::string n2s(int value)
{
	ostringstream ss;
    ss << value;
	return ss.str();
}

int main()
{
	int err = 0;
	ThreadPool pool(5);

	do {
		err = pool.StartPool();
		if (0 != err) {
			cerr << "start pool failed";
			break;
		}

		for (int i = 0;i < 100; ++i) {
			std::string value = n2s(i);
			Context *ctx = new Context(Context::ECT_job, value);
			pool.Enque(ctx);
		}

		pool.StopPool();
	} while(0);

	return err;
}


