""" Concat individual python files together for submitting """

import glob
import re

def main():
    modules = [
        'game',
        'commands',
        'ai',
        'main'
    ]
    rstrip = lambda s: s[:-1] if s[-1] == '\n' else s

    for module in modules:
        with open(module.replace('.', '/') + '.py') as f:
            lines = f.readlines()
        print('# {}'.format(module))
        print('\n'.join([rstrip(line) for line in lines if not re.match('from ({}) import .+'.format('|'.join(modules)), line)]))
        print()


if __name__ == '__main__':
    main()
