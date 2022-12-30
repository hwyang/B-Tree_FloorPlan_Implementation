#include<iostream>
#include<stdio.h>
#include<stdlib.h>
#include<fstream>
#include<string>
#include<vector>
#include<stack>
#include<queue>
#include<algorithm>
#include<cmath>
#include<time.h>
#include<map>
#include<limits.h>
using namespace std;

#ifndef CLASS_H
#define CLASS_H

extern int total_nets;
extern int total_pins;
extern int total_blocks;
extern int total_terminals;
extern double total_area;

extern vector<Net *> net_list;
extern map<string, Block*> block_list;
extern map<string, Terminal*> terminal_list;
extern map<string, bool> rotate_state;

extern double dead_space_ratio;
extern double FLOORPLAN_H, FLOORPLAN_W;

#endif