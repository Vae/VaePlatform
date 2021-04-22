//
// Created by protocol on 3/2/21.
//

#ifndef BOOSTTESTING_DEFS_H
#define BOOSTTESTING_DEFS_H

typedef signed long EntityID;

#define DEB(x) std::cout << #x << ": " << x

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

#endif //BOOSTTESTING_DEFS_H
