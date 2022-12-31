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

class Net{
    public:
        Net(int degree){
            this->degree = degree;
        }
        int degree;
        vector<string> connected;
};

class Block{
    public:
        Block(string name){
            this->name = name;
            this->width = 0.0;
            this->height = 0.0;
            this->area = 0.0;
            this->orientation = false;
            this->x = 0.0;
            this->y = 0.0;
        }

        void Rotate(){
            this->orientation = !(this->orientation);
            this->width = this->width + this->height;
            this->height = this->width - this->height;
            this->width = this->width - this->height;
        }

        string name;
        bool orientation;
        double width;
        double height;
        double area;
        double x;
        double y;
        vector<Net *> connected;
};

class Terminal{
    public:
        Terminal(string name){
            this->name = name;
            this->x = 0.0;
            this->y = 0.0;
        }
        string name;
        double x;
        double y;
        vector<Net *> connected;
};

class BTNode{
    public:
        BTNode(Block *block){
            this->left = NULL;
            this->right = NULL;
            this->parent = NULL;
            this->block = block;
        }
        BTNode *left;
        BTNode *right;
        BTNode *parent;
        Block *block;
};

class BTree{
    public:
        BTree(){
            this->root = NULL;
            this->width = 0.0;
            this->height = 0.0;
            this->area = 0.0;
        }

        void Repack(){
            this->contour.clear();
            this->contour.resize(1000);
            this->width = 0.0;
            this->height = 0.0;
            this->Recursive_Build(this->root, 0);
            this->area = this->width * this->height;
        }

        void Recursive_Build(BTNode* cur, int x){
            if(cur == NULL) return;
            int max_h = -1;
            Block *cur_block = cur->block;
            if(x + cur_block->width > this->contour.size()) this->contour.resize(this->contour.size()*2);
            for(int i = x; i < x + cur_block->width; i++){
                if(max_h < this->contour[i]) max_h = this->contour[i];
            }
            cur_block->x = x;
            cur_block->y = max_h;
            for(int i = x; i < x + cur_block->width; i++) this->contour[i] = cur_block->y + cur_block->height;

            if(cur_block->x + cur_block->width > this->width) this->width = cur_block->x + cur_block->width;
            if(cur_block->y + cur_block->height > this->height) this->height = cur_block->y + cur_block->height;
            if(cur->left != NULL) Recursive_Build(cur->left, cur_block->x + cur_block->width);
            if(cur->right != NULL) Recursive_Build(cur->right, cur_block->x);
            this->node_of[cur_block] = cur;
            return;
        }

        void DFTravereBuild(){
            stack<BTNode *> S;
            double cur_width = 0.0;
            BTNode *cur = NULL;

            cur = this->root;
            if(!cur) return;
            //set coordinate x, y
            cur->block->x = 0;
            cur->block->y = 0;
            //set cur_width
            cur_width = cur->block->width;
            //update tree shape
            this->width = cur->block->width;
            this->height = cur->block->height;
            //update contour
            if(cur->block->width > this->contour.size()) this->contour.resize(this->contour.size()*2);
            for(int i = 0; i < cur->block->width; i++) this->contour[i] = cur->block->height;
            //record relation btw node and block
            this->node_of[cur->block] = cur;
            //push child if any
            if(cur->right) S.push(cur->right);
            if(cur->left) S.push(cur->left);
            while(!S.empty()){
                cur = S.top();
                S.pop();
                //set coordination
                int x;
                if(cur->parent->left == cur) x = cur->parent->block->x + cur->parent->block->width;
                else if(cur->parent->right == cur) x = cur->parent->block->x;
                int y = -1;
                for(int i = x; i < x + cur->block->width; i++){
                    if(this->contour[i] > y) y = this->contour[i];
                }
                cur->block->x = x;
                cur->block->y = y;
                //set cur_width
                cur_width = x + cur->block->width;
                //update tree shape
                if(this->width < cur_width) this->width = cur_width;
                if(this->height < y + cur->block->height) this->height = y + cur->block->height;
                //update contour
                if(cur_width > this->contour.size()) this->contour.resize(this->contour.size()*2);
                for(int i = x; i < cur_width; i++) this->contour[i] = cur->block->y + cur->block->height;
                //record relation btw node and block
                this->node_of[cur->block] = cur;
                //push child if any
                if(cur->right) S.push(cur->right);
                if(cur->left) S.push(cur->left);
            }
        }

        void Rotate(BTNode *target){
            target->block->Rotate();
        }
        
        void Swap(BTNode *lhs, BTNode *rhs){
            swap(lhs->block, rhs->block); //only swap block, connection relation (left, right, parent) does change
        }

        void Move(BTNode *from, BTNode *to){
            if(from->parent == to) return;
            BTNode *target = from;
            while(target->left != NULL || target->right != NULL){
                if(target->left != NULL) target = target->left;
                else if(target->right != NULL) target = target->right;
            }
            if(to == target) return;
            this->Swap(target, from);
            
            if(target->parent->left == target){
                target->parent->left = NULL;
                if(to->left != NULL){ //left child occupied
                    BTNode *child = to->left;
                    to->left = target;
                    target->parent = to;
                    child->parent = target;
                    target->left = child;
                }
                else{
                    to->left = target;
                    target->parent = to;
                }
            }
            if(target->parent->right == target){
                target->parent->right = NULL;
                if(to->right != NULL){ //right child occupied
                    BTNode *child = to->right;
                    to->right = target;
                    target->parent = to;
                    child->parent = target;
                    target->right = child;
                }
                else{
                    to->right = target;
                    target->parent = to;
                }
            }
        }
        
        BTNode *root;
        double width;
        double height;
        double area;

        vector<int> contour;
        map<Block*, BTNode*> node_of;
};

#endif