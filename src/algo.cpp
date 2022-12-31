#include"algo.h"
using namespace std;

bool Comparator(const Block* a, const Block* b){
    return a->height > b->height;
}

BTree* INIT_BTree(map<string, Block*> block_list){
    BTNode *cur_node, *row_head, *new_node = NULL;
    vector<Block *> ordered_blocks;
    double cur_width = 0.0;
    BTree *Tree = new BTree();
    Tree->contour.resize(FLOORPLAN_W+1);
    //Sort Blocks by Area
    for(auto const &block : block_list){
        Block *b = block.second;
        if(b->width > b->height) b->Rotate();
        ordered_blocks.push_back(b);
    }
    sort(ordered_blocks.begin(), ordered_blocks.end(), Comparator);
    
    //Build Tree Root
    new_node = new BTNode(ordered_blocks[0]);
    //build connection
    Tree->root = new_node;
    //set coordination
    new_node->block->x = 0;
    new_node->block->y = 0;
    //set cur_width
    cur_width = new_node->block->width;
    //update tree shape
    Tree->width = new_node->block->width;
    Tree->height = new_node->block->height;
    //update contour
    for(int i = 0; i < new_node->block->width; i++){
        Tree->contour[i] = new_node->block->height;
    }
    //move to next node
    row_head = new_node;
    cur_node = new_node;
    for(int i = 1; i < ordered_blocks.size(); i++){
        new_node = new BTNode(ordered_blocks[i]);
        //Put on right side
        if(cur_width + new_node->block->width <= FLOORPLAN_W){
            //build connection
            cur_node->left = new_node;
            new_node->parent = cur_node;
            //set coordination
            int x = cur_node->block->x + cur_node->block->width;
            int y = -1;
            for(int i = x; i < x + new_node->block->width; i++){
                if(Tree->contour[i] > y) y = Tree->contour[i];
            }
            new_node->block->x = x;
            new_node->block->y = y;
            //set cur_width
            cur_width = x + new_node->block->width;
            //update tree shape
            if(Tree->width < cur_width) Tree->width = cur_width;
            if(Tree->height < y+new_node->block->height) Tree->height = y + new_node->block->height;
            //update contour
            for(int i = x; i < x + new_node->block->width; i++) Tree->contour[i] = new_node->block->y + new_node->block->height;
            //move to next node
            cur_node = new_node;
        }
        //Put on top side
        else{
            //build connection
            row_head->right = new_node;
            new_node->parent = row_head;
            //set coordination
            int x = row_head->block->x;
            int y = -1;
            for(int i = x; i < x+new_node->block->width; i++){
                if(Tree->contour[i] > y) y = Tree->contour[i];
            }
            new_node->block->x = x;
            new_node->block->y = y;
            //set cur_width
            cur_width = new_node->block->width;
            //update tree shape
            if(Tree->width < cur_width) Tree->width = cur_width;
            if(Tree->height < y + new_node->block->height) Tree->height = y + new_node->block->height;
            //update contour
            for(int i = x; i < x+new_node->block->width; i++) Tree->contour[i] = new_node->block->y + new_node->block->height;
            //move to next node
            row_head = new_node;
            cur_node = new_node;
        }
    }
    Tree->Repack();
    return Tree;
}

BTree* Perturb(BTree* Tree){
    map<string, Block*>::iterator block_iter;
    Block *target = NULL;
    int operation = rand() % 6;
    int target_index = rand() % total_blocks;
    block_iter = next(block_list.begin(), target_index);
    
    target = block_iter->second;
    
    //cout << operation << endl;
    if(operation == 0){
        Tree->Rotate(Tree->node_of[target]);
    }
    else if(operation == 1 || operation == 2){
        do{
            target_index = rand() % total_blocks;
            block_iter = next(block_list.begin(), target_index);
        }while(block_iter->second == target);
        Tree->Swap(Tree->node_of[block_iter->second], Tree->node_of[target]);
    }
    else if(operation == 3 || operation == 4 || operation == 5){
        do{
            target_index = rand() % total_blocks;
            block_iter = next(block_list.begin(), target_index);
        }while(block_iter->second == target);

        Tree->Move(Tree->node_of[block_iter->second], Tree->node_of[target]);
    }
    return Tree;
}

double HPWL(){
    double hpwl = 0.0;
    map<string, Block*>::iterator block_iter;
    map<string, Terminal*>::iterator terminal_iter;
    int rounded_lower_x, rounded_higher_x, rounded_lower_y, rounded_higher_y;
    int center_x, center_y;
    for(auto const & net : net_list){
        rounded_lower_x = INT_MAX;
        rounded_higher_x = INT_MIN;
        rounded_lower_y = INT_MAX;
        rounded_higher_y = INT_MIN;
        for(auto const &pin : net->connected){
            block_iter = block_list.find(pin);
            terminal_iter = terminal_list.find(pin);
            if(block_iter != block_list.end()){
                center_x = block_iter->second->x + (block_iter->second->width/2);
                center_y = block_iter->second->y + (block_iter->second->height/2);
            }
            else{
                if(terminal_iter != terminal_list.end()){
                    center_x = terminal_iter->second->x;
                    center_y = terminal_iter->second->y;
                }
            }
            rounded_lower_x = (center_x < rounded_lower_x) ? center_x : rounded_lower_x;
            rounded_lower_y = (center_y < rounded_lower_y) ? center_y : rounded_lower_y;
            rounded_higher_x = (center_x > rounded_higher_x) ? center_x : rounded_higher_x;
            rounded_higher_y = (center_y > rounded_higher_y) ? center_y : rounded_higher_y;
        }
        hpwl += (rounded_higher_x - rounded_lower_x) + (rounded_higher_y - rounded_lower_y);
    }
    return hpwl;
}

double Cost_Function(BTree *Tree){
    double ex_width = max(Tree->width - FLOORPLAN_W, 0.0);
    double ex_height = max(Tree->height - FLOORPLAN_H, 0.0);
    
    return 1000.0 * (ex_height + ex_width) + ((300.0 / total_blocks) * HPWL());
}

void Fast_Simulated_Annealing(BTree *Tree, int seed, clock_t start){
    if(seed != 0) srand(seed);

    double P = 0.9;
    int k = 8;
    double delta_avg = 1e-5;

    double cur_temperature = 0.0;
    double actual_temperature = 0.0;
    double termination_temperature = 1e-6;
    double accept_prob = 0.0;
    int N = k * total_blocks;
    int uphill = 0;
    bool update_best = false;


    BTree *Best_Tree = NULL;
    BTree *Original_Tree = NULL;
    double best_cost = INT_MAX;
    double last_iter_cost = 0.0;
    double cur_cost = 0.0;
    double delta_cost = 0.0;

    cur_temperature = -delta_avg / log(P);

    Best_Tree = Save_BTree(Tree);
    Original_Tree = Save_BTree(Tree);

    best_cost = Cost_Function(Tree);
    last_iter_cost = best_cost;

    int count = 0;
    int bad_count_streak = 0;
    do{
        count++;
        update_best = false;
        uphill = 0;
        int iter = 0;
        for(; iter < 2*N && uphill < N; iter++){
            
            Perturb(Tree);
            Tree->Repack();
    
            
            cur_cost = Cost_Function(Tree);
            
            delta_cost = cur_cost - last_iter_cost;
            accept_prob = exp(-delta_cost / cur_temperature);

            double dice = double(rand()) / RAND_MAX;
            if(delta_cost <= 0 || dice < accept_prob){
                
                Kill_BTree(Original_Tree);

                Original_Tree = Save_BTree(Tree);

                last_iter_cost = cur_cost;
                if(delta_cost > 0) uphill++;
                
                // keep best solution
                if(cur_cost < best_cost && (Tree->width <= FLOORPLAN_W && Tree->height <= FLOORPLAN_H)){
                    Kill_BTree(Best_Tree);
                    Best_Tree = Save_BTree(Tree);

                    best_cost = cur_cost;
                    bad_count_streak = 0;
                    update_best = true;
                }
            }
            else{
                Resume_To_Saved(Tree, Original_Tree);
                Tree = Original_Tree;
                Original_Tree = Save_BTree(Tree);
            }
        }

        cur_temperature = 0.99 * cur_temperature;
        if(!update_best) bad_count_streak++;
    }while(cur_temperature > termination_temperature && bad_count_streak <= 10 && ((((double) (clock() - start)) / CLOCKS_PER_SEC) <= 1150));

    Resume_To_Saved(Tree, Best_Tree);
    Tree = Best_Tree;
    Tree->Repack();
}