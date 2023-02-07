#!/usr/bin/env python3

"""
Benchmarking script.
"""

import sys
import os
import argparse
import logging
import networkx as nx
from random import Random
import time
import subprocess

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

    parser = argparse.ArgumentParser(description='Twinwidth solver prototype.')
    parser.add_argument('-v', '--version', action='version', version=f'%(prog)s {__version__}')
    parser.add_argument('--log-level', choices=['crit', 'error', 'warn', 'info', 'debug', 'none'], default='info', help='log level')
    parser.add_argument('--seed', type=int, default=12345, help='seed for pseudorandom number generator (default:12345)')
    return parser


def generate_mw_bounded_graph(n: int, max_mw: int, rand: Random) -> nx.Graph:
    if n == 0:
        return nx.Graph()
    max_mw = max(max_mw, 3)

    G = nx.empty_graph(1)
    while len(G) < n:
        # pick a vertex to substitute
        prev_n = len(G)
        x = rand.randint(0, prev_n - 1)

        # create a replacement
        nn = min(n - prev_n + 1, max_mw)
        H = nx.erdos_renyi_graph(nn, 0.5, seed=rand)
        labels = {i: prev_n + i if i < nn - 1 else x for i in range(nn)}

        # substitue
        nbrs = list(G.neighbors(x))
        G.remove_node(x)
        G.add_node(x)
        G.add_nodes_from(range(prev_n, prev_n + nn - 1))

        G.add_edges_from((labels[u], labels[v]) for u, v, in H.edges())
        G.add_edges_from((labels[i], u) for i in range(nn) for u in nbrs)
    return G


def bench_cpp(G: nx.Graph) -> tuple[int, float, str]:
    elist = '\n'.join(f'{u} {v}' for u, v in G.edges())
    proc = subprocess.Popen('build/Release/modular-bench', stdin=subprocess.PIPE, stdout=subprocess.PIPE)
    out = proc.communicate(input=elist.encode('utf-8'))[0].decode('utf-8')
    lines = out.splitlines()
    return int(lines[0]), float(lines[1]), lines[2].strip()

def bench_one_instance(G: nx.Graph, num_iterations: int = 3):
    n = G.number_of_nodes()
    m = G.number_of_edges()

    expected_answer = None

    for _ in range(num_iterations):
        for solver in ['naive', 'linear', 'cpp']:
            if solver == 'cpp':
                mw, elapsed, ans = bench_cpp(G)
            else:
                time_start = time.time()
                t = modular_decomposition(G, solver=solver)
                time_end = time.time()
                elapsed = time_end - time_start
                mw = t.modular_width()
                t.sort()
                ans = str(t)

            if expected_answer is None:
                t.sort()
                expected_answer = ans
            else:
                t.sort()
                assert expected_answer == ans, 'Wrong Answer'

            print(f'{n},{m},{solver},{mw},{elapsed}')



def main(args):
    """Entry point of the program. """

    # get logger
    logger = get_logger({'crit': logging.CRITICAL, 'error': logging.ERROR, 'warn': logging.WARNING, 'info': logging.INFO, 'debug': logging.DEBUG, 'none': logging.CRITICAL + 1}[
        args.log_level])

    logger.info(f'Started: {SCRIPT_PATH}')
    logger.info(f'Seed: {args.seed}')
    logger.info('Output format: n, m, solver, modular-width, elapsed (sec)')

    rand = Random(args.seed)
    sys.setrecursionlimit(100000)

    # benchmark on Erdos-Renyi graphs
    logger.info('Benchmarking on Erdos-Renyi graphs.')
    for n in [100, 200, 500]:
        for p in [0.1, 0.2, 0.3, 0.4, 0.5]:
            G = nx.erdos_renyi_graph(n, p, seed=rand)
            bench_one_instance(G)
            bench_one_instance(nx.complement(G))

    # benchmark on modular-width bounded graphs
    logger.info('Benchmarking on mw-bounded graphs.')
    for n in [100, 200, 500, 1000]:
        for mw in [4, 5, 10, 20, 50]:
            G = generate_mw_bounded_graph(n, mw, rand)
            bench_one_instance(G)
            bench_one_instance(nx.complement(G))

    logger.info(f'Finished: {SCRIPT_PATH}')


if __name__ == '__main__':
    main(get_parser().parse_args())
