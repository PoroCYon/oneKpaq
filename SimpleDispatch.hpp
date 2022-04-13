/* Copyright (c) Teemu Suutari */

#ifndef SIMPLEDISPATCH_H
#define SIMPLEDISPATCH_H

#if defined(__APPLE__) || defined(HAS_LIBDISPATCH)

#include <dispatch/dispatch.h>

template<typename F,typename... Args>
void DispatchLoop(size_t iterations,size_t stride,F func,Args&&... args)
{
	if (!iterations) return;
	if (stride<1) stride=1;
	dispatch_apply((iterations+stride-1)/stride,dispatch_get_global_queue(DISPATCH_QUEUE_PRIORITY_DEFAULT,0),^(size_t majorIt) {
		majorIt*=stride;
		for (size_t minorIt=0;minorIt<stride;minorIt++) {
			size_t iter=majorIt+minorIt;
			if (iter>=iterations) break;
			func(iter,args...);
		}
	});
}

#else
// #warning "Parallelization disabled!!!"
#include <thread>
#include <mutex>
#include <queue>
#include "onekpaq_common.h"

//simple c++11 threads implementation
template<typename F,typename... Args>
void DispatchLoop(size_t iterations,size_t stride,F func,Args&&... args)
{
	if (!iterations) return;
	if (stride<1) stride=1;
	// Take a coffee, come back, wait, go to lunch, come back, wait, take a coffee...
	// for (size_t i=0;i<iterations;i++) func(i,args...);

	size_t numJobs = (iterations+stride-1)/stride;
	std::queue<size_t> jobs;
	for (size_t i = 0; i < numJobs; i++) {
		jobs.push(i * stride);
	}

	std::mutex mutex;
	size_t numThreads = std::thread::hardware_concurrency();
	// DebugPrint("iters: %d\n", iterations);
	std::vector<std::thread> threads;
	for (size_t i = 0; i < numThreads; i++)
	{
		threads.push_back(std::thread([&]() {
			while (true) {
				size_t majorIt = 0;
				{
					std::unique_lock<std::mutex> lock(mutex);
					if (jobs.empty()) {
						break;
					}
					majorIt = jobs.front();
					jobs.pop();
				}
				for (size_t minorIt=0;minorIt<stride;minorIt++) {
					size_t iter=majorIt+minorIt;
					if (iter>=iterations) break;
					func(iter,args...);
				}
			}
		}));
	}

	for (std::thread &thread : threads)
	{
		thread.join();
	}
}

#endif

#endif
