#!/usr/bin/python3

import matplotlib.pyplot as plt
import numpy as py

fname_pos = './data/data.position'
fname_force = './data/data.force'
fname_adapt = './data/data.adaptation'

# ===================
# === Position
# ===================
# Time_rscv   Cur_h       Refpos      Refforce    
time, h = [], []
with open(fname_pos) as f:
    contents = f.readlines()

del contents[0]
for line in contents:
    value = [float(s) for s in line.split()]
    time.append(value[0])
    h.append(value[1])

plt.plot(time, h)
plt.show()

