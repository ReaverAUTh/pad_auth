# -*- coding: utf-8 -*-
"""
Created on Wed Nov 25 18:45:05 2020

@author: Angelos Spyrakis, Dimitris Spanos
"""

import matplotlib.pyplot as plt
import numpy as np


def mainV3(name,nodes,edges):
    
    x2, y2 = np.loadtxt('v3_cilk.txt', delimiter=',', unpack=True)
    plt.plot(x2,y2, label='V3 openCilk')
    x3, y3 = np.loadtxt('v3_openmp.txt', delimiter=',', unpack=True)
    plt.plot(x3,y3, label='V3 openMP')

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time(sec)')
    plt.title('%s:(n, e) = (%s, %s)' % (name,nodes, edges))
    plt.rcParams["figure.figsize"] = (8,8)

    
    plt.legend()
    plt.savefig('v3.png', bbox_inches='tight')
    plt.show()
    

def mainV4(name,nodes,edges):
    
    x2, y2 = np.loadtxt('v4_cilk.txt', delimiter=',', unpack=True)
    plt.plot(x2,y2, label='V4 openCilk')
    x3, y3 = np.loadtxt('v4_openmp.txt', delimiter=',', unpack=True)
    plt.plot(x3,y3, label='V4 openMP')
    x4, y4 = np.loadtxt('v4_thread.txt', delimiter=',', unpack=True)
    plt.plot(x4,y4, label='V4 pthreads')

    plt.xlabel('Number of Threads')
    plt.ylabel('Execution Time(sec)')
    plt.title('%s: (n, e) = (%s, %s)' % (name,nodes, edges))
    plt.rcParams["figure.figsize"] = (8,8)

    plt.legend()
    plt.savefig('v4.png', bbox_inches='tight')
    plt.show()    
    
name = input("name: ")
nodes = input("nodes: ")
edges = input("edges: ")
mainV3(name,nodes,edges)    
mainV4(name,nodes,edges)
     