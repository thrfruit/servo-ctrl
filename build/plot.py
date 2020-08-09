#!/usr/bin/python3
import matplotlib.pyplot as plt
import numpy as np
from matplotlib.animation import FuncAnimation
from matplotlib import animation
import time as tm

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

# ===================
# === Force
# ===================
# Time    Refforce    Curforce   Refpos
time, curf, reff, refpos = [], [], [], []
with open(fname_force) as f:
    contents = f.readlines()

del contents[0]
for line in contents:
    value = [float(s) for s in line.split()]
    time.append(value[0])
    reff.append(value[1])
    curf.append(value[2])
    refpos.append(value[3])

# ===================
# === Adaptation
# ===================
# hr   s    dh    a_hat   b_hat   c_hat    Refforce    
time_rscv, hr, s, dh, a_hat = [], [], [], [], []
with open(fname_adapt) as f:
    contents = f.readlines()

del contents[0]
for line in contents:
    value = [float(s) for s in line.split()]
    time_rscv.append(value[0])
    a_hat.append(value[3])



# ===================
# === Plot
# ===================
# === Fig 1
fig1 = plt.figure(figsize=(16, 9))
# --- Subplot
axh = fig1.add_subplot(2,2,1)
axdu = fig1.add_subplot(2,2,2)
axf = fig1.add_subplot(2,2,3)
axpos = fig1.add_subplot(2,2,4)

# --- Refh & Curh
axh.plot(time, refh, '--', label='参考位置')
axh.plot(time, curh, label='实际位置')
axh.legend()
axh.set_title('物体位移曲线')

# --- du
axdu.plot(time_rscv, a_hat)
axdu.set_title('自适应控制参数变化曲线')

# --- Reff & Curf
axf.plot(time, reff, '--', label='压力信号')
axf.plot(time, curf, label='实际压力')
axf.legend()
axf.set_title('压力变化曲线')

# --- Refpos
axpos.plot(time, refpos)
axpos.set_title('夹具位置信号')

# === Gif
gif, ax = plt.subplots()

xdata, ydata = [], []
ax.plot(time,refh, '--', label='参考位置')
ax.plot(time,curh,label='实际位置')
ax.legend()
ln, = ax.plot(xdata,ydata,"ro")
def init():
    ax.set_xlim(0, time[-1]+0.3)
    ax.set_ylim(-0.025,0.001)
    return ln,

def update(n):
    xdata=time[n]
    ydata=curh[n]
    ln.set_data(xdata,ydata)
    return ln,

ani = FuncAnimation(gif, update, frames=np.arange(len(time)),
        interval=time[-1]/len(time)*1000,init_func=init, blit=True)


# ===================
# === Save Figures
# ===================
# --- Give a filename
fname = tm.strftime('%y%m%d_%H%M',tm.localtime(tm.time()))
fig = fig1

# --- Display Figures
plt.show()

# --- Save Figures
save_flag = input('Save or not (y/n)')
if save_flag == '':
    fig.savefig('./pics/'+fname+'.svg')
elif save_flag in 'nN':
    pass
else:
    fig.savefig('./pics/'+fname+'.svg')


