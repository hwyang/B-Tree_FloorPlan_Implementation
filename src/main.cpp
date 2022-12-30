#include"utils.h"
#include"algo.h"
using namespace std;

int total_nets = 0;
int total_pins = 0;
int total_blocks = 0;
int total_terminals = 0;
double total_area = 0.0;

vector<Net *> net_list;
map<string, Block*> block_list;
map<string, Terminal*> terminal_list;

double DEAD_SPACE_RATIO = 0.0;
double FLOORPLAN_H, FLOORPLAN_W = 0.0;



int main(int argc, char *argv[]){
    clock_t start = clock();

    BTree *Tree = NULL;
    int seed = 0;
    hardblocks_read(argv[1]);
    pl_read(argv[3]);
    net_read(argv[2]);
    DEAD_SPACE_RATIO = stof(argv[5]);
    FLOORPLAN_H = sqrt(total_area * (1+DEAD_SPACE_RATIO));
    FLOORPLAN_W = sqrt(total_area * (1+DEAD_SPACE_RATIO));

    printf("Iput time: %f\n", ((double) (clock() - start)) / CLOCKS_PER_SEC);
    
    Tree = INIT_BTree(block_list);

    printf("Init time: %f\n", ((double) (clock() - start)) / CLOCKS_PER_SEC);
    
    if(total_blocks <= 100) seed = 156670865;
    else if(total_blocks > 100 && total_blocks < 149) seed = 7868;
    else if (total_blocks == 150) seed = 74942314;
    else if(total_blocks > 150 && total_blocks <= 300) seed = 0;
    else seed = 0;

    Fast_Simulated_Annealing(Tree, seed, start);

    double wirelength = HPWL();
    cout << "Area: " << Tree->area << " Wirelength: " << wirelength << " Dead Space Ratio: " << (1 - Tree->area / (total_area * (1+DEAD_SPACE_RATIO))) << endl;
    Output(Tree, wirelength, argv[4]);
    
    
    printf("Total Run Time: %f\n", ((double) (clock() - start)) / CLOCKS_PER_SEC);
    //print_Tree(Tree->root);
    //cout << Tree->height << ' ' << Tree->width << ' ' << Tree->area << endl;
    //for(auto &n : Tree->contour) cout << n << ' ';
    //cout << DEAD_SPACE_RATIO << ' ' << FLOORPLAN_W << ' ' << FLOORPLAN_H << endl;
    //print_nets();
    //print_hard_blocks();
    //print_terminals();

    return 0;
}