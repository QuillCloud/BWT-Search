//
//  bwtsearch.hpp
//  assignment 2
//
//  Created by Zhang Yunhe on 17/04/2017.
//  Copyright © 2017 Zhang Yunhe. All rights reserved.
//

#ifndef bwtsearch_h
#define bwtsearch_h

#include <stdio.h>
#include <string>
#include <fstream>
#include <stdlib.h>
#include <map>
#include <iterator>
#include <cstring>

using namespace std;

int occurrence(int c_num, int num);

int inverse_occ(int c_num, int num, int row);

string back_search(int number);

string forward_search(int number, int row);

#endif /* bwtsearch_h */
