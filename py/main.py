from ai import stupid
from game import Barrel, Game, Mine, Ship, Shot


def parse_entity(type, id, x, y, *args):
    """ Create entity from referee's input """
    if type == 'SHIP':
        return Ship(x, y, orient=args[0], speed=args[1], health=args[2], mine=args[3]==1)
    elif type == 'BARREL':
        return Barrel(x, y, health=args[0])
    elif type == 'CANNONBALL':
        return Shot(x, y, origin=args[0], turns_to_land=args[1])
    elif type == 'MINE':
        return Mine(x, y)
    else:
        raise RuntimeError('unknown entity ' + type)


def main():
    tick = 0
    while True:
        my_ship_count = int(input())  # the number of remaining ships
        entity_count = int(input())  # the number of entities (e.g. ships, mines or cannonballs)
        game = Game(tick=tick)
        for i in range(entity_count):
            entity_id, entity_type, x, y, arg_1, arg_2, arg_3, arg_4 = input().split()
            entity = parse_entity(entity_type, *[int(i) for i in (entity_id, x, y, arg_1, arg_2, arg_3, arg_4)])
            entity.id = int(entity_id)
            if isinstance(entity, Ship):
                if entity.mine:
                    game.my_ships.append(entity)
                else:
                    game.their_ships.append(entity)
            elif isinstance(entity, Barrel):
                game.barrels.append(entity)
            elif isinstance(entity, Mine):
                game.mines.append(entity)
            elif isinstance(entity, Shot):
                game.shots.append(entity)

        for my_ship in game.my_ships:
            print(stupid(my_ship, game).encode())

        tick += 1


if __name__ == '__main__':
    main()
