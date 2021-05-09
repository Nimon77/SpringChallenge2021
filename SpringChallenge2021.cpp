#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <tuple>

using namespace std;

ostream& operator<<(ostream& os, const tuple<string,int,int>& tup) {
    os << get<0>(tup);
    if (get<1>(tup) > -1)
        os << " " << get<1>(tup);
    if (get<2>(tup) > -1)
        os << " " << get<2>(tup);
    return os;
}

string to_string(tuple<string,int,int>& tup) {
    string str = get<0>(tup);

    if (get<1>(tup) > -1)
        str.append(" " + to_string(get<1>(tup)));
    if (get<2>(tup) > -1)
        str.append(" " + to_string(get<2>(tup)));
    return str;
}

class Cell {
public:
    Cell () {
        neighbors.resize(6);
    }

    void input() {
        cin >> cell_index >> richness;
        for (auto& n: neighbors) {
            cin >> n;
        }
    }
    int cell_index;
    int richness;
    vector<int> neighbors;
};

class Tree {
public:
    Tree () = default;
    Tree (int cell_index, int size, bool is_mine, bool is_dormant) :
        cell_index{cell_index}, size{size}, is_mine{is_mine}, is_dormant{is_dormant} {}
    void input() {
        cin >> cell_index >> size >> is_mine >> is_dormant;
    }
    int cell_index;
    int size;
    bool is_mine;
    bool is_dormant;
};

class Game {
private:
        int day = 0;
        int nutrients = 0;
        vector<Cell> board;
        vector<Tree> trees;
        vector<tuple<string,int,int>> possible_actions;
        int mySun;
        int oppSun;
        int score;
        int oppScore;
        int oppIsWaiting;

public:
    void inputInitData() {
        int numberOfCells;
        cin >> numberOfCells;
        for (int i = 0; i < numberOfCells; i++) {
            Cell cell;
            cell.input();
            board.push_back(cell);
        }
    }

    void inputInfo() {
        // input game info
        cin >> day;
        cin >> nutrients;
        cin >> mySun >> score;
        cin >> oppSun >> oppScore >> oppIsWaiting;

        // input trees info
        trees.clear();
        int numberOfTrees;
        cin >> numberOfTrees;
        for (int i = 0; i < numberOfTrees; i++) {
            Tree tree;
            tree.input();
            trees.push_back(tree);
        }

        // input possible actions
        possible_actions.clear();
        int numberOfPossibleMoves;
        cin >> numberOfPossibleMoves;
        for (int i = 0; i < numberOfPossibleMoves; i++) {
            string type;
            int arg1 = -1;
            int arg2 = -1;
            cin >> type;
            
            if (type == "WAIT") {
                possible_actions.push_back(make_tuple(type, arg1,arg2));
            } else if (type == "COMPLETE") {
                cin >> arg1;
                possible_actions.push_back(make_tuple(type, arg1,arg2));
            }
            else if (type == "GROW") {
                cin >> arg1;
                possible_actions.push_back(make_tuple(type, arg1,arg2));
            }
            else if (type == "SEED") {
                cin >> arg1;
                cin >> arg2;
                possible_actions.push_back(make_tuple(type, arg1,arg2));
            }
        }
    }
    
    bool is_in_shadow(int cell_id) {
        for (int i = 0; i < trees.size(); i++) {
            if (trees[i].is_mine)
                for (int j = 0; j < 6; j++) {
                    Cell cell = board[trees[i].cell_index];
                    for (int k = 0; k < 2; k++) {
                        if (cell.neighbors[j] != -1)
                            cell = board[cell.neighbors[j]];
                        if (cell.cell_index == cell_id)
                            return true;
                    }
                }
        }
        return false;
    }

    Tree* find_tree(int cell_id) {
        for (int i = 0; i < trees.size(); i++)
            if (trees[i].cell_index == cell_id)
                return &trees[i];
        return NULL;
    }

    int countTreeSizeMax() {
        int nbr = 0;
        for (int i = 0; i < trees.size(); i++)
            if (trees[i].size == 3)
                nbr++;
        return nbr;
    }

    string compute_next_action() {
        tuple<string,int,int> selected = possible_actions[0];
        for (int i = 0; i < possible_actions.size(); i++){
            cerr << possible_actions[i] << endl;
            if (get<0>(possible_actions[i]) != "WAIT") {
                if (get<0>(possible_actions[i]) == "GROW") {
                    if (get<0>(selected) == "WAIT")
                        selected = possible_actions[i];
                    else if (board[get<1>(possible_actions[i])].richness > board[get<1>(selected)].richness)
                        selected = possible_actions[i];
                }
                else if (get<0>(possible_actions[i]) == "SEED") {
                    if (!is_in_shadow(get<2>(possible_actions[i]))) {
                        if (get<0>(selected) == "WAIT")
                            selected = possible_actions[i];
                        else if (board[get<2>(possible_actions[i])].richness > board[get<2>(selected)].richness)
                            selected = possible_actions[i];
                    }
                }
                else if (get<0>(possible_actions[i]) == "COMPLETE") {
                    if (countTreeSizeMax() > 5 || day >= 22) {
                        if (get<0>(selected) == "WAIT")
                            selected = possible_actions[i];
                        else if (board[get<1>(possible_actions[i])].richness > board[get<1>(selected)].richness)
                            selected = possible_actions[i];
                    }
                }
            }
        }
        return to_string(selected);
    }
};

int main()
{
    Game game;
    game.inputInitData();

    while (true) {
        game.inputInfo();
        
        cout << game.compute_next_action() << endl;
    }
}
