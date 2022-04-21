#!/usr/bin/env python

import os
import argparse as cli

def isWindows():
    return os.name == 'nt'

def LibrarySymbols(lib):
    sym_list = []
    if isWindows():
        stream = os.popen('dumpbin /SYMBOLS /EXPORTS %s' % lib)
        for line in stream:
            sym = line.split('|')[1].strip()
            sym_list.append(sym)
    else:
        stream = os.popen('nm %s' % lib)
        for line in stream:
            if ' T ' in line:
                sym = line.split()[2].strip()
                sym_list.append(sym)
    return sym_list


parser = cli.ArgumentParser()

parser.add_argument(
    'lib',
    help='Library to check.')
parser.add_argument(
    '--prefix',
    help='Mangling prefix.')
args = parser.parse_args()

if not args.prefix:
    print('No prefix to check.')
    exit(0)

if not os.path.exists(args.lib):
    print('Library not found %s' % args.lib)

exemptions = ['GetVTKVersion']

bad_sym = []
for sym in LibrarySymbols(args.lib):
    if not args.prefix in sym:
        if not sym in exemptions:
            bad_sym.append(sym)

if bad_sym:
    print('Found symbols that are missing mangling.')
    for sym in bad_sym:
        print("  + %s" % sym)
    exit(1)
