//
// Created by protocol on 4/19/21.
//

#ifndef BOOSTTESTING_THREADMANAGER_H
#define BOOSTTESTING_THREADMANAGER_H

#include <boost/noncopyable.hpp>
#include <boost/thread/thread.hpp>
#include <boost/thread/xtime.hpp>

namespace vae {
    class Thread: private boost::noncopyable {
        void operator()(){

        }
    };

    class ThreadManager: private boost::noncopyable {
        boost::thread_group threads;
    public:
        ThreadManager(int threads){

        }

        void go(){

        }
    };
}

#endif //BOOSTTESTING_THREADMANAGER_H
