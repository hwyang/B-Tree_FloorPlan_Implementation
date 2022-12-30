#include"class.cpp"
using namespace std;

#ifndef UTILS_H
#define UTILS_H

extern int total_nets;
extern int total_pins;
extern int total_blocks;
extern int total_terminals;
extern double total_area;

extern vector<Net *> net_list;
extern map<string, Block*> block_list;
extern map<string, Terminal*> terminal_list;
extern map<Block*, BTNode*> node_of;

extern double dead_space_ratio;
extern double FLOORPLAN_H, FLOORPLAN_W;

void net_read(const char *argv);
void hardblocks_read(const char *argv);
void pl_read(const char *argv);

BTree* Save_BTree(BTree *Tree);
BTNode* DFCopy(BTNode *cur);
void Resume_To_Saved(BTree *Original_Tree, BTree *Resuming_Tree);
void Resume_Block(BTNode *cur);
void Kill_BTree(BTree *Tree);
void DFKill(BTNode *cur);

void print_nets();
void print_hard_blocks();
void print_terminals();
void print_Tree(const BTNode *cur);
void Output(BTree *Best_Tree, double wirelength, const char *argv);

#endif