""" Bot commands """

class Move:
    """ Move command """

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def encode(self):
        """ Encode for referee """
        return 'MOVE {x} {y}'.format_map(self.__dict__)


class Wait:
    """ No-op """

    def encode(self):
        """ Encode for referee """
        return 'WAIT'


class Fire:
    """ Fire a cannonball """

    def __init__(self, x, y):
        self.x = x
        self.y = y

    def encode(self):
        return 'FIRE {x} {y}'.format_map(self.__dict__)


class PlantMine:
    """ Plant a mine """

    def encode(self):
        return 'MINE'


class SlowDown:
    """ Slow down """

    def encode(self):
        return 'SLOWER'
