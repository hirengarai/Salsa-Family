"""
 *  REFERENCE IMPLEMENTATION OF complexity computation programe
 *
 * Filename: complexity.py
 *
 * created: 24/09/23
 * updated: 24/09/2023
 *
 * by Hiren
 * Research Scholar,
 * BITS Pilani, Hyderabad Campus
 *
 * Synopsis:
 *  This file contains the complexity calculation progarame.
 * running command: python complex.py
"""



from statistics import NormalDist
import math


complexity = 0.0
temp = 0
KEYsize = 256

q = 1  # stages
SIGcount = [226, 56, 50]
fwepsilon = [0.0055, 0.0005, 0.0005]
bwepsilon = [0.05, 0.183, 0.715]
alpha = [5, 68, 68]

constant = NormalDist(mu=0, sigma=1).inv_cdf(0.0013 / q)
if q == 1:
    temp += SIGcount[0]
else:
    for i in SIGcount:
        temp += i

PNBcount = KEYsize - temp
totalSIG = []
N = [] # data complexity


for i in range(q):
    totalSIG.append(2 ** SIGcount[i])
    epsilon = fwepsilon[i] * bwepsilon[i]
    temp = 1 - (epsilon * epsilon)
    N.append(
        (
            (math.sqrt(alpha[i] * math.log(4)) - constant * math.sqrt(temp))
            / (bwepsilon[i] * fwepsilon[i])
        )
        ** 2
    )
    complexity += N[i] * totalSIG[i]
if q == 1:
    complexity += 2**PNBcount + 2 ** (KEYsize - alpha[0])  # single step
else:
    complexity += 2**PNBcount  # fse
    # complexity +=
print("Total time complexity 2^(%0.2f)" % math.log2(complexity))