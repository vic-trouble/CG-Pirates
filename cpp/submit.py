''' Concat individual source files together for submitting '''

import glob
import re

def main():
    print('#pragma GCC optimize("-O3")')
    print('#pragma GCC optimize("inline")')
    print('#pragma GCC optimize("omit-frame-pointer")')
    print('#define SUBMISSION')
    print()

    src_files = [
        'cxxtweaks.h',
        'log.h',
        'hex.h',
        'game.h', 
        'commands.h',
        'simulation.h',
        'ai_naive.h',
        'ai_score.h',
        'ai_tactics.h',
        'ai_search.h',
        'io.h',     
        'hex.cpp',
        'io.cpp',
        'game.cpp',
        'commands.cpp',
        'simulation.cpp',
        'ai_naive.cpp', 
        'ai_score.cpp',
        'ai_tactics.cpp',
        'ai_search.cpp',
        'main.cpp']
    rstrip = lambda s: s[:-1] if s[-1] == '\n' else s
    fix = lambda s: re.sub(r'({})\.'.format('|'.join(src_files)), '', s)

    for src_file in src_files:
        with open(src_file) as f:
            lines = f.readlines()
        print('// {}'.format(src_file))
        print('\n'.join([fix(rstrip(line)) for line in lines if not re.match(r'#include .+\.h', line)]))
        print()


if __name__ == '__main__':
    main()
