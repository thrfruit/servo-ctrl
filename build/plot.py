#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as py

plt.rcParams['font.sans-serif']=['SimHei'] #用来正常显示中文标签
plt.rcParams['axes.unicode_minus']=False #用来正常显示负号

fname_pos = './data/data.position'
fname_force = './data/data.force'
fname_adapt = './data/data.adaptation'

# ===================
# === Position
# ===================
# Time_rscv   Cur_h   Ref_h
time, curh, refh = [], [], []
with open(fname_pos) as f:
    contents = f.readlines()

del contents[0]
for line in contents:
    value = [float(s) for s in line.split()]
    time.append(value[0])
    curh.append(value[1])
    refh.append(value[2])

#  plt.plot(time, curh)
#  plt.show()

# ===================
# === Force
# ===================
# Time    Curforce    Refforce Refpos
time, curf, reff, refpos = [], [], [], []
with open(fname_force) as f:
    contents = f.readlines()

del contents[0]
for line in contents:
    value = [float(s) for s in line.split()]
    time.append(value[0])
    curf.append(value[1])
    reff.append(value[2])
    refpos.append(value[3])

# ===================
# === Adaptation
# ===================
# hr   s    dh    a_hat   b_hat   c_hat    Refforce    
hr, s, dh, a_hat = [], [], [], []


# ===================
# === Plot
# ===================
# === Fig 1
fig1 = plt.figure(figsize=(16, 24))
# --- Subplot
axh = fig1.add_subplot(2,2,1)
axdu = fig1.add_subplot(2,2,2)
axf = fig1.add_subplot(2,2,3)
axpos = fig1.add_subplot(2,2,4)

# --- Refh & Curh
axh.plot(time, refh, '--')
axh.plot(time, curh)
axh.set_title('物体位移曲线')

# --- du

# --- Reff & Curf
axf.plot(time, reff, '--', label='压力信号')
axf.plot(time, curf, label='实际压力')
axf.legend()
axf.set_title('压力变化曲线')

# --- Refpos
axpos.plot(time, refpos)
axpos.set_title('夹具位置信号')


plt.show()

