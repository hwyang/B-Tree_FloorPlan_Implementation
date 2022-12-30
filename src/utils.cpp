#include"utils.h"
using namespace std;

void net_read(const char *argv){
    ifstream file;
    string tmp;
    int degree;
    string name;

	file.open(argv);
    if(!file){
		cout << "Cannot read:" << argv << endl;
		return ;
	}
    file >> tmp >> tmp >> total_nets;
    file >> tmp >> tmp >> total_pins;
    //cout << total_pins << ' ' << total_nets;
    while(file >> tmp >> tmp >> degree){
        Net* net = new Net(degree);
        while(degree--){
            file >> name;
            if(name[0] == 's') block_list[name]->connected.push_back(net);
            else if(name[0] == 'p') terminal_list[name]->connected.push_back(net);
            else cout << "Error net read" << endl;
            net->connected.push_back(name);
        }
        net_list.push_back(net);
    }
    file.close();
}
void hardblocks_read(const char *argv){
    ifstream file;
    string tmp;
    string name;
    string x, y;
    int vertex_num;
    double coordinate_x, coordinate_y, area;
    
    file.open(argv);
    if(!file){
		cout << "Cannot read:" << argv << endl;
		return ;
	}
    file >> tmp >> tmp >> total_blocks;
    file >> tmp >> tmp >> total_terminals;
    for(int i = 0; i < total_blocks; i++){
        file >> name >> tmp >> vertex_num;
        Block *block = new Block(name);
        block_list[name] = block;
        while(vertex_num--){
            file >> x >> y;
            x.erase(x.begin());
            x.erase(x.end()-1);
            y.erase(y.end()-1);
            coordinate_x = stof(x);
            coordinate_y = stof(y);
            //cout << coordinate_x << endl;
            //cout << coordinate_y << endl;
            area = coordinate_x * coordinate_y;
            if(area){
                block->width = coordinate_x;
                block->height = coordinate_y;
                block->area = area;
                total_area += area;
            }
        }
    }
    for(int i = 0; i < total_terminals; i++){
        file >> name >> tmp;
        Terminal *terminal = new Terminal(name);
        terminal_list[name] = terminal;
    }
    file.close();
}
void pl_read(const char *argv){
    ifstream file;
    string name;
    double coordinate_x, coordinate_y;

    file.open(argv);
    if(!file){
		cout << "Cannot read:" << argv << endl;
		return ;
	}
    while(file >> name >> coordinate_x >> coordinate_y){
        //cout << coordinate_x << ' ' << coordinate_y << endl;
        terminal_list[name]->x = coordinate_x;
        terminal_list[name]->y = coordinate_y;
    }
    file.close();
}

BTree* Save_BTree(BTree *Tree){
    BTree *newTree = new BTree();
    newTree->root = DFCopy(Tree->root);
    newTree->Repack();
    return newTree;
}
BTNode* DFCopy(BTNode *cur){
    Block *copy_block = new Block(cur->block->name);
    copy_block->width = cur->block->width;
    copy_block->height = cur->block->height;
    copy_block->area = cur->block->area;
    copy_block->orientation = cur->block->orientation;

    BTNode *copy = new BTNode(copy_block);

    if(cur->left){
        copy->left = DFCopy(cur->left);
        copy->left->parent = copy;
    }
    if(cur->right){
        copy->right = DFCopy(cur->right);
        copy->right->parent = copy;
    }
    return copy;
}

void Resume_To_Saved(BTree *Original_Tree, BTree *Resuming_Tree){
    Resume_Block(Resuming_Tree->root);
    // for(auto const & iter : Resuming_Tree->node_of){
    //     Block *block = iter.first;
    //     block_list[block->name] = block;
    // }
    Kill_BTree(Original_Tree);
    Original_Tree = NULL;
    return;
}

void Resume_Block(BTNode *cur){
    if(!cur) return;
    Resume_Block(cur->left);
    Resume_Block(cur->right);
    Block *resuming_block = cur->block;
    block_list[resuming_block->name] = resuming_block;
    return;
}

void Kill_BTree(BTree *Tree){
    // Tree->contour.clear();
    // Tree->width = 0.0;
    // Tree->height = 0.0;
    // Tree->area = 0.0;
    DFKill(Tree->root);
    Tree = NULL;
}
void DFKill(BTNode *cur){
    if(!cur) return;
    DFKill(cur->left);
    DFKill(cur->right);
    delete cur->block;
    delete cur;
    return;
}

void print_nets(){
    cout << "Total Nets: " << total_nets << endl;
    cout << "Total Pins: " << total_pins << endl;
    for(auto const &net : net_list){
        cout << "Degree: " << net->degree << endl;
        cout << "Connected: ";
        for(auto &name : net->connected){
            cout << name << ' ';
        }
        cout << endl;
    }
}

void print_hard_blocks(){
    cout << "Hard Block Num: " << block_list.size() << endl;
    for(auto const &block : block_list){
        Block* b = block.second;
        cout << block.first << ' ' << b->name << ' ' << b->width << ' ' << b->height << ' ' << b->area << endl;
        for(auto const &net : b->connected){
            cout << net->degree << endl;
        }
    }
}

void print_terminals(){
    cout << "Terminal Num: " << terminal_list.size() << endl;
    for(auto const &terminal : terminal_list){
        Terminal* t = terminal.second;
        cout << terminal.first << ' ' << t->name << ' ' << t->x << ' ' << t->y << endl;
        for(auto const &net : t->connected){
            cout << net->degree << endl;
        }
    }
}

void print_Tree(const BTNode *cur){
    //static int count = 0;
    if(cur == NULL) return;
    cout << "name: " << cur->block->name <<  " x: " << cur->block->x << " y: " << cur->block->y << " width: " << cur->block->width << " height: " << cur->block->height << endl;
    //count++;
    print_Tree(cur->left);
    print_Tree(cur->right);
    //cout << count << endl;
    return;
}

void Output(BTree *Best_Tree, double wirelength, const char *argv){
    ofstream file;
    file.open(argv);
    file << "Wirelength " << wirelength << endl;
    file << "Blocks" << endl;
    for(auto & iter : block_list){
        Block *block = iter.second;
        file << block->name << ' ' << block->x << ' ' << block->y << ' ' << block->orientation << endl;
    }

    file.close();
}