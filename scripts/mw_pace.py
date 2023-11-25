#!/usr/bin/env python3

"""
Benchmarking script.
"""

import sys
import os
import argparse
import logging
import networkx as nx
import subprocess

from typing import TextIO

__version__ = '0.0.1'
__license__ = 'Apache License, Version 2.0'

SCRIPT_PATH = os.path.realpath(__file__)
SCRIPT_DIR = os.path.dirname(SCRIPT_PATH)
PROJECT_DIR = os.path.dirname(SCRIPT_DIR)
PYTHON_MAIN = os.path.join(PROJECT_DIR, 'src', 'main', 'python')
DATA_DIR = os.path.join(PROJECT_DIR, 'data')

if PYTHON_MAIN not in sys.path:
    sys.path.insert(0, PYTHON_MAIN)

from modular import *


def get_logger(log_level=logging.CRITICAL + 1):
    """Logger settings."""

    logging.basicConfig(level=log_level, format='%(asctime)s [%(levelname)s] %(message)s', stream=sys.stderr)
    logger = logging.getLogger(__name__)
    return logger


def get_parser():
    """Argument parser."""

    parser = argparse.ArgumentParser(description='Benchmark for PACE 2023 instances.')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {__version__}')
    parser.add_argument('--log-level', choices=['crit', 'error', 'warn', 'info', 'debug', 'none'], default='info', help='log level')
    parser.add_argument('path', nargs='+', help='input paths')
    return parser


def bench_cpp(G: nx.Graph) -> tuple[int, float, str]:
    elist = '\n'.join(f'{u} {v}' for u, v in G.edges())
    proc = subprocess.Popen('build/Release/modular-bench', stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    out = proc.communicate(input=elist.encode('utf-8'))[0].decode('utf-8')
    lines = out.splitlines()
    return int(lines[0]), float(lines[1])


def bench_one_instance(path: str, num_iterations: int = 3):
    G = load_pace_2023(path)
    n = G.number_of_nodes()
    m = G.number_of_edges()

    for _ in range(num_iterations):
        mw, elapsed = bench_cpp(G)
        print(f'{path},{n},{m},{mw},{elapsed}')


def read_pace_2023(input: TextIO) -> nx.Graph:
    G = None
    for line in input.readlines():
        line = line.strip()
        if not line:
            continue
        if line.startswith('c'):
            continue  # ignore comments

        if line.startswith('p'):
            _, _, nn, mm = line.split()
            n, m = int(nn), int(mm)
            G = nx.empty_graph(n)
        else:
            u, v = map(int, line.split())
            assert G is not None
            G.add_edge(u - 1, v - 1)

    assert G is not None
    assert m == G.number_of_edges(), 'inconsistent edges'
    return G


def load_pace_2023(path: str) -> nx.Graph:
    with open(path) as f:
        return read_pace_2023(f)


def main(args):
    """Entry point of the program. """

    # get logger
    logger = get_logger({'crit': logging.CRITICAL, 'error': logging.ERROR, 'warn': logging.WARNING, 'info': logging.INFO, 'debug': logging.DEBUG, 'none': logging.CRITICAL + 1}[
        args.log_level])

    logger.info(f'Started: {SCRIPT_PATH}')
    logger.info('Output format: path, n, m, modular-width, elapsed (sec)')

    for path in args.path:
        bench_one_instance(path, 1)

    logger.info(f'Finished: {SCRIPT_PATH}')


if __name__ == '__main__':
    main(get_parser().parse_args())
