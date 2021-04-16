//
// Created by protocol on 3/2/21.
//

#ifndef BOOSTTESTING_DEFS_H
#define BOOSTTESTING_DEFS_H

typedef signed long EntityID;

//These are the double's max range that'll round to an integer:
#define STAT_INT_MAX 9007199254740992
#define STAT_INT_MIN -9007199254740992

//double dbl = 9007199254000000; /* I started with 9007199254000000, a little less than 2^53 */
//while (dbl + 1 != dbl) dbl++;
//printf("%.0f\n", dbl - 1);
//printf("%.0f\n", dbl);
//printf("%.0f\n", dbl + 1);
//return 0;


#endif //BOOSTTESTING_DEFS_H
