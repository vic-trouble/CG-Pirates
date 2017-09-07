""" Game entities """

class Limits:
    """ Game limits """
    MAP_WIDTH = 23
    MAP_HEIGHT = 21
    COOLDOWN_CANNON = 2
    COOLDOWN_MINE = 5
    INITIAL_SHIP_HEALTH = 100
    MAX_SHIP_HEALTH = 100
    MIN_SHIPS = 1
    MIN_RUM_BARRELS = 10
    MAX_RUM_BARRELS = 26
    MIN_RUM_BARREL_VALUE = 10
    MAX_RUM_BARREL_VALUE = 20
    REWARD_RUM_BARREL_VALUE = 30
    MINE_VISIBILITY_RANGE = 5
    FIRE_DISTANCE_MAX = 10
    LOW_DAMAGE = 25
    HIGH_DAMAGE = 50
    MINE_DAMAGE = 25
    NEAR_MINE_DAMAGE = 10


class Entity:
    """ An entity """

    ENTITY_ID = 0

    def __init__(self, x, y):
        self.id = Entity.ENTITY_ID
        Entity.ENTITY_ID += 1
        self.x = x
        self.y = y


class Barrel(Entity):
    """ Barrel of rum """

    def __init__(self, x, y, health=10):
        super().__init__(x, y)
        self.health = health


class Ship(Entity):
    """ Ship """

    def __init__(self, x, y, orient=0, speed=0, health=100, mine=True):
        super().__init__(x, y)
        self.orient = orient
        self.speed = speed
        self.health = health
        self.mine = mine


class Shot(Entity):
    """ Cannon ball """

    def __init__(self, x, y, origin=None, turns_to_land=1):
        super().__init__(x, y)
        self.origin = origin
        self.turns_to_land = turns_to_land


class Mine(Entity):
    """ Sea mine """

    def __init__(self, x, y):
        super().__init__(x, y)


class Game:
    """ Game state """

    def __init__(self, tick=0, my_ships=None, their_ships=None, barrels=None, mines=None, shots=None):
        self.tick = tick
        self.my_ships = my_ships or []
        self.their_ships = their_ships or []
        self.barrels = barrels or []
        self.mines = mines or []
        self.shots = shots or []
