#include"utils.h"
using namespace std;

#ifndef ALGO_H
#define ALGO_H

bool Comparator(const Block* a, const Block* b);
BTree* INIT_BTree(map<string, Block*> block_list);

BTree* Perturb(BTree* Tree);
double HPWL();
double Cost_Function(BTree *Tree);
void Fast_Simulated_Annealing(BTree *Tree, int seed, clock_t start);

#endif