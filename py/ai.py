""" AI """

import math

from commands import Fire, Move, Wait
from game import Game, Limits, Ship

bombed_mines = set([])
def stupid(my_ship: Ship, game: Game):
    """ AI """
    dist_to_me = lambda e: math.hypot(e.x - my_ship.x, e.y - my_ship.y)

    # first try to attack enemy
    for their_ship in game.their_ships:
        dist = dist_to_me(their_ship)
        if dist < 5 and game.tick & 1 == 1:
            next_x = int(round(their_ship.x + their_ship.speed * dist * math.cos(their_ship.orient * math.pi / 3)))
            next_y = int(round(their_ship.y + their_ship.speed * dist * math.sin(their_ship.orient * math.pi / 3)))
            next_x = min(max(0, next_x), Limits.MAP_WIDTH)
            next_y = min(max(0, next_y), Limits.MAP_HEIGHT)
            return Fire(next_x, next_y)

    # then attack mines
    if game.mines:
        closest_mine = min(game.mines, key=dist_to_me)
        if dist_to_me(closest_mine) < 3 and game.tick & 1 == 1 and closest_mine.id not in bombed_mines:
            bombed_mines.add(closest_mine.id)
            return Fire(closest_mine.x, closest_mine.y)

    # then collect barrels
    if game.barrels:
        closest = min(game.barrels, key=dist_to_me)
        return Move(closest.x, closest.y)

    # then pursue their ships
    closest_enemy = min(game.their_ships, key=dist_to_me)
    return Move(closest_enemy.x, closest_enemy.y)

