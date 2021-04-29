//
// Created by protocol on 4/27/21.
//

#include "test.h"

std::string vae::test::randString(int len){
    static std::vector<char> letters = {'a','b','c','d','e','f','g','h','i','j','k','l','m','n','o','p','q',
                                        'r','s','t','u','v','w','x',
                                        'y','z',
                                        'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P','Q',
                                        'R','S','T','U','V','W','X',
                                        'Y','Z'
    };
    std::string s = "";
    for(int a = 0; a < len; a++)
        s += letters[rand()%letters.size()];
    return s;
}