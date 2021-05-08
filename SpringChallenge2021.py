import sys
import math
from enum import Enum
import random

class Cell:
    def __init__(self, cell_index, richness, neighbors):
        self.cell_index = cell_index
        self.richness = richness
        self.neighbors = neighbors

class Tree:
    def __init__(self, cell_index, size, is_mine, is_dormant):
        self.cell_index = cell_index
        self.size = size
        self.is_mine = is_mine
        self.is_dormant = is_dormant

class ActionType(Enum):
    WAIT = "WAIT"
    SEED = "SEED"
    GROW = "GROW"
    COMPLETE = "COMPLETE"

class Action:
    def __init__(self, type, target_cell_id=None, origin_cell_id=None):
        self.type = type
        self.target_cell_id = target_cell_id
        self.origin_cell_id = origin_cell_id

    def __str__(self):
        if self.type == ActionType.WAIT:
            return 'WAIT'
        elif self.type == ActionType.SEED:
            return f'SEED {self.origin_cell_id} {self.target_cell_id}'
        else:
            return f'{self.type.name} {self.target_cell_id}'

    @staticmethod
    def parse(action_string):
        split = action_string.split(' ')
        if split[0] == ActionType.WAIT.name:
            return Action(ActionType.WAIT)
        if split[0] == ActionType.SEED.name:
            return Action(ActionType.SEED, int(split[2]), int(split[1]))
        if split[0] == ActionType.GROW.name:
            return Action(ActionType.GROW, int(split[1]))
        if split[0] == ActionType.COMPLETE.name:
            return Action(ActionType.COMPLETE, int(split[1]))

class Game:
    def __init__(self):
        self.day = 0
        self.nutrients = 0
        self.board = []
        self.trees = []
        self.possible_actions = []
        self.my_sun = 0
        self.my_score = 0
        self.opponents_sun = 0
        self.opponent_score = 0
        self.opponent_is_waiting = 0
        self.sun_direction = -1

    def is_in_shadow(self, cell_number):
        for tree in self.trees:
            if tree.is_mine:
                for i in range(6):
                    cell = self.board[tree.cell_index]
                    for j in range(2):
                        if cell.neighbors[i] != -1:
                            cell = self.board[cell.neighbors[i]]
                        if cell.cell_index == cell_number:
                            return 1
        return 0

    def is_in_shadow_half(self, cell_number):
        count = [0] * 6
        for tree in self.trees:
            if tree.is_mine:
                for i in range(6):
                    cell = self.board[tree.cell_index]
                    for j in range(tree.size):
                        if cell.neighbors[i] != -1:
                            cell = self.board[cell.neighbors[i]]
                        if cell.cell_index == cell_number:
                            count[i] += 1
        total = 0
        for i in count:
            if i > 0:
                total += 1
        print(count, file=sys.stderr)
        print(total, file=sys.stderr)
        if total > 3:
            return 1
        return 0
            

    def compute_next_action(self):
        self.selected = self.possible_actions[0]
        for action in self.possible_actions:
            print(f'{action}', file=sys.stderr)
            if action.type != ActionType.WAIT:
                if action.type == ActionType.COMPLETE:
                    if self.board[action.target_cell_id].richness > 1 or self.day >= 22:
                        if self.selected.type == ActionType.WAIT:
                            self.selected = action
                        elif self.board[action.target_cell_id].richness > self.board[self.selected.target_cell_id].richness:
                            self.selected = action
                elif action.type == ActionType.SEED:
#                    if not self.is_in_shadow_half(action.target_cell_id):
                    if not self.is_in_shadow(action.target_cell_id):
                        if self.selected.type == ActionType.WAIT:
                            self.selected = action
                        elif self.board[action.target_cell_id].richness > self.board[self.selected.target_cell_id].richness:
                            self.selected = action
        if self.selected.type == ActionType.WAIT:
            for action in self.possible_actions:
                if action.type == ActionType.GROW:
                    self.selected = action
                    break
        return self.selected


number_of_cells = int(input())
game = Game()
for i in range(number_of_cells):
    cell_index, richness, neigh_0, neigh_1, neigh_2, neigh_3, neigh_4, neigh_5 = [int(j) for j in input().split()]
    game.board.append(Cell(cell_index, richness, [neigh_0, neigh_1, neigh_2, neigh_3, neigh_4, neigh_5]))

while True:
    _day = int(input())
    game.day = _day
    nutrients = int(input())
    game.nutrients = nutrients
    sun, score = [int(i) for i in input().split()]
    game.my_sun = sun
    game.my_score = score
    opp_sun, opp_score, opp_is_waiting = [int(i) for i in input().split()]
    game.opponent_sun = opp_sun
    game.opponent_score = opp_score
    game.opponent_is_waiting = opp_is_waiting
    number_of_trees = int(input())
    game.sun_direction = game.day % 6
    game.trees.clear()
    for i in range(number_of_trees):
        inputs = input().split()
        cell_index = int(inputs[0])
        size = int(inputs[1])
        is_mine = inputs[2] != "0"
        is_dormant = inputs[3] != "0"
        game.trees.append(Tree(cell_index, size, is_mine == 1, is_dormant))

    number_of_possible_actions = int(input())
    game.possible_actions.clear()
    for i in range(number_of_possible_actions):
        possible_action = input()
        game.possible_actions.append(Action.parse(possible_action))

    print(game.compute_next_action())
