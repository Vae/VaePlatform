//
// Created by protocol on 3/2/21.
//

#ifndef BOOSTTESTING_DEFS_HPP
#define BOOSTTESTING_DEFS_HPP

#include <boost/noncopyable.hpp>

/**
 *
 * This should be renamed to util
 *
 */

namespace vae{
    typedef double TimeType;
    typedef double VectorDirectionType;
    typedef double SpeedType;


#define PI 3.1415926535

    double randomDouble(double fMin, double fMax);


    class ServerInstance : public boost::noncopyable{
    private:
        bool running;
    public:
        ServerInstance(): running(true) {}
        void stop() { running = false; }
        bool isRunning() const { return running; }
    };
    //instanced in main.cpp
    extern ServerInstance serverInstance;


    //This is an idea for wrapping a single resource with an associated mutex
    template<typename T, typename MUTEX_TYPE>
    class SharedResource: public T{
    public:
        //T t;
        //T &operator()(){return t;}
        MUTEX_TYPE mutex;
    };
}   //namespace vae

typedef signed long EntityId;



#define DEB(x) << #x << ": " << x << "  "

//These are the double's max range that'll round to an integer:
#define STAT_INT_MAX 9007199254740992
#define STAT_INT_MIN -9007199254740992

//double dbl = 9007199254000000; /* I started with 9007199254000000, a little less than 2^53 */
//while (dbl + 1 != dbl) dbl++;
//printf("%.0f\n", dbl - 1);
//printf("%.0f\n", dbl);
//printf("%.0f\n", dbl + 1);
//return 0;



/*
template <typename CT, typename ... A> struct function
        : public function<decltype(&CT::operator())(A...)> {};

template <typename C> struct function<C> {
private:
    C mObject;

public:
    function(const C & obj)
            : mObject(obj) {}

    template<typename... Args> typename
    std::result_of<C(Args...)>::type operator()(Args... a) {
        return this->mObject.operator()(a...);
    }

    template<typename... Args> typename
    std::result_of<const C(Args...)>::type operator()(Args... a) const {
        return this->mObject.operator()(a...);
    }
};

namespace make {
    template<typename C> auto function(const C & obj) {
        return ::function<C>(obj);
    }
}
*/

#endif //BOOSTTESTING_DEFS_HPP
