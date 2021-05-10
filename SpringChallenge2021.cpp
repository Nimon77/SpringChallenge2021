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

ostream& operator<<(ostream& os, const vector<tuple<string,int,int>>& vec) {
    for (int i = 0; i < vec.size(); i++)
        os << vec[i] << endl;
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
        int numberOfCells = 0;
        vector<Cell> board;
        int numberOfTrees = 0;
        vector<Tree> trees;
        int numberOfPossibleActions;
        vector<tuple<string,int,int>> possible_actions;
        int mySun;
        int oppSun;
        int score;
        int oppScore;
        int oppIsWaiting;

        vector<tuple<string,int,int>> grows;
        vector<tuple<string,int,int>> seeds;
        vector<tuple<string,int,int>> completes;

public:
    void inputInitData() {
        int numberOfCells;
        cin >> numberOfCells;
        this->numberOfCells = numberOfCells;
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
        this->numberOfTrees = numberOfTrees;
        for (int i = 0; i < numberOfTrees; i++) {
            Tree tree;
            tree.input();
            trees.push_back(tree);
        }

        // input possible actions
        possible_actions.clear();
        int numberOfPossibleMoves;
        cin >> numberOfPossibleMoves;
        this->numberOfPossibleActions = numberOfPossibleMoves;
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

    void printGameStartInfo()
    {
        cerr << numberOfCells << endl;
        for (int i = 0; i < numberOfCells; i++) {
            cerr << board[i].cell_index << " " << board[i].richness << " ";
            for (int j = 0; j < 6; j++) {
                cerr << board[i].neighbors[j];
                if (j < 5)
                    cerr << " ";
            }
            cerr << endl;
        }
    }

    void printGameDayInfo() {
        cerr << day << endl;
        cerr << nutrients << endl;
        cerr << mySun << " " << score << endl;
        cerr << oppSun << " " << oppScore << " " << oppIsWaiting << endl;
        cerr << numberOfTrees << endl;
        for (int i = 0; i < numberOfTrees; i++)
            cerr << trees[i].cell_index << " " << trees[i].size << " " << trees[i].is_mine << " " << trees[i].is_dormant << endl;
        cerr << numberOfPossibleActions << endl;
        for (int i = 0; i < numberOfPossibleActions; i++) {
            cerr << possible_actions[i];
            if (i < numberOfPossibleActions - 1)
                cerr << " ";
        }
    }

    Tree* find_tree(int cell_id) {
        for (int i = 0; i < trees.size(); i++)
            if (trees[i].cell_index == cell_id)
                return &trees[i];
        return NULL;
    }

    int countTreeSize(int size) {
        int nbr = 0;
        for (int i = 0; i < trees.size(); i++)
            if (trees[i].size == size && trees[i].is_mine)
                nbr++;
        return nbr;
    }

    void calcPossibleActions() {
        grows.clear(); seeds.clear(); completes.clear();
        int size0 = countTreeSize(0);
        int size1 = countTreeSize(1);
        int size2 = countTreeSize(2);
        int size3 = countTreeSize(3);
//        cerr << size0 << " " << size1 << " " << size2 << " " << size3 << endl;
        bool canSeed = mySun >= size0 ? true : false;
        for (int i = 0; i < trees.size(); i++) {
            if (trees[i].is_dormant == 1 || trees[i].is_mine == 0)
                continue;
            if (trees[i].size < 3)    //GROW
                if ((trees[i].size == 0 && mySun >= 1 + size1) || (trees[i].size == 1 && mySun >= 3 + size2) || (trees[i].size == 2 && mySun >= 7 + size3))
                    grows.push_back(make_tuple("GROW", trees[i].cell_index, -1));
            if (trees[i].size == 3 && mySun >= 4)   //COMPLETE
                completes.push_back(make_tuple("COMPLETE", trees[i].cell_index, -1));
            if (canSeed)    //SEED
                for (int orient = 0; orient < 6; orient++) {
                    Cell cell = board[trees[i].cell_index];
                    for (int j = 0; j < trees[i].size; j++) {
                        if (cell.neighbors[orient] == -1)
                            break;
                        cell = board[cell.neighbors[orient]];
                        if (!find_tree(cell.cell_index) && cell.richness != 0)
                            seeds.push_back(make_tuple("SEED", trees[i].cell_index, cell.cell_index));
                    }
                }
        }
//        cerr << "calc action :" << endl << "WAIT" << endl << grows << seeds << completes << endl << endl << "given actions :" << endl << possible_actions;
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

    string compute_next_action() {
        calcPossibleActions();
        tuple<string,int,int> selected = possible_actions[0];
        for (int i = 0; i < possible_actions.size(); i++){
            if (get<0>(possible_actions[i]) != "WAIT") {
                if (get<0>(possible_actions[i]) == "GROW") {
                    if (get<0>(selected) == "WAIT")
                        selected = possible_actions[i];
                    else if (board[get<1>(possible_actions[i])].richness >= board[get<1>(selected)].richness)
                        selected = possible_actions[i];
                }
                else if (get<0>(possible_actions[i]) == "SEED" && day < 15 && countTreeSize(0) < 2) {
                    if (!is_in_shadow(get<2>(possible_actions[i]))) {
                        if (get<0>(selected) == "WAIT")
                            selected = possible_actions[i];
                        else if (board[get<2>(possible_actions[i])].richness > board[get<2>(selected)].richness)
                            selected = possible_actions[i];
                    }
                }
                else if (get<0>(possible_actions[i]) == "COMPLETE") {
                    if (countTreeSize(3) > 5 || day >= 22) {
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
//    game.printGameStartInfo();
    while (true) {
        game.inputInfo();
//        game.printGameDayInfo();
        cout << game.compute_next_action() << endl;
    }
}
