#!/usr/bin/env python3

"""
plotting a porkchop plot given a file with data using matplotlib

author: alexander 
date: 2022-06-05
"""

import numpy as np
import math
import json
import datetime
import matplotlib as mpl
import matplotlib.pyplot as plt
from matplotlib import dates


def label_line(line, label_text, near_i=None, near_x=None, near_y=None, rotation_offset=0, offset=(0,0)):
    """call 
        l, = plt.loglog(x, y)
        label_line(l, "text", near_x=0.32)
    """

    def put_label(i):
        """put label at given index"""

        i = min(i, len(x)-2)
        dx = sx[i+1] - sx[i]
        dy = sy[i+1] - sy[i]

        rotation = np.rad2deg(math.atan2(dy, dx)) + rotation_offset
        pos = [(x[i] + x[i+1])/2. + offset[0], (y[i] + y[i+1])/2 + offset[1]]

        plt.text(pos[0], pos[1], label_text, size=9, rotation=rotation, color = line.get_color(),
        ha="center", va="center", bbox = dict(ec='1',fc='1'))

    x = line.get_xdata()
    y = line.get_ydata()
    ax = line.axes

    if ax.get_xscale() == 'log':
        sx = np.log10(x)

    else:
        sx = x

    if ax.get_yscale() == 'log':
        sy = np.log10(y)

    else:
        sy = y

    # find index
    if near_i is not None:
        i = near_i

        if i < 0: # sanitize negative i
            i = len(x) + i
        put_label(i)

    elif near_x is not None:
        for i in range(len(x)-2):
            if (x[i] < near_x and x[i+1] >= near_x) or (x[i+1] < near_x and x[i] >= near_x):
                put_label(i)

    elif near_y is not None:
        for i in range(len(y)-2):
            if (y[i] < near_y and y[i+1] >= near_y) or (y[i+1] < near_y and y[i] >= near_y):
                put_label(i)
    else:
        raise ValueError("Need one of near_i, near_x, near_y")
                                                                
def draw_flight_time_line(x,y,c):
        """draw a transfer time line"""
        
        l, = plt.plot(x,y,c)
        label_line(l, str(int(y[0]-x[0]))+' days', near_i=np.size(x)-1, rotation_offset=-25, offset=[6,5])


def main():
    # read the dates from the input data
    with open("input.json") as f:
        data = json.load(f)

        y1 = data["window"]["departure_lower"]["y"]
        m1 = data["window"]["departure_lower"]["m"]
        d1 = data["window"]["departure_lower"]["d"]

        y2 = data["window"]["departure_upper"]["y"]
        m2 = data["window"]["departure_upper"]["m"]
        d2 = data["window"]["departure_upper"]["d"]

        a = datetime.date(y1, m1, d1);

        b = datetime.date(y2, m2, d2);

        x_axis = int((b-a).days) + 1
        y_axis = data["window"]["tof_upper"] - data["window"]["tof_lower"] + 1


    # C3 arrays 
    c30_t1 = np.zeros((x_axis, y_axis))
    c3f_t1 = np.zeros((x_axis, y_axis))
    c30_t2 = np.zeros((x_axis, y_axis))
    c3f_t2 = np.zeros((x_axis, y_axis))
    t0 = np.zeros((x_axis, y_axis))
    tf = np.zeros((x_axis, y_axis))

    # parsing the input file
    # which has the format: c30_t1, c3f_t1, c30_t2, c3f_t2, dep_epoch, arr_epoch
    x_it = -1
    y_it = 0

    with open("porkchop_data") as f:
        for line_number, line in enumerate(f):

            data = [float(x.strip()) for x in line.split(",")]

            if ((line_number % y_axis) == 0):
                y_it = 0
                x_it += 1
            else:
                y_it += 1

            c30_t1[x_it][y_it] = data[0]
            c3f_t1[x_it][y_it] = data[1]
            c30_t2[x_it][y_it] = data[2]
            c3f_t2[x_it][y_it] = data[3]
            t0[x_it][y_it] = data[4]
            tf[x_it][y_it] = data[5]


    mpl.rcParams['font.size'] = 10.
    mpl.rcParams['font.family'] = 'Serif'
    mpl.rcParams['axes.labelsize'] = 10.
    mpl.rcParams['xtick.labelsize'] = 10.
    mpl.rcParams['ytick.labelsize'] = 10.
    mpl.rcParams["contour.linewidth"] = 1.

    contour_range = list(range(1,30,2))

    hfmt = dates.DateFormatter("%Y/%m/%d")
    fmt = "%i"

    # ************************************************************************
    # plotting the target porkchop
    fig = plt.figure(figsize=(10,10))
    ax = fig.add_subplot(111)
    plt.subplots_adjust(right=0.8,bottom=0.2)

    # tof lines
    draw_flight_time_line(t0[:,0],tf[:,0],'r')	
    draw_flight_time_line(t0[:,int(np.size(t0,1)/4)],tf[:,int(np.size(t0,1)/4)],'r')
    draw_flight_time_line(t0[:,int(np.size(t0,1)/2)],tf[:,int(np.size(t0,1)/2)],'r')
    draw_flight_time_line(t0[:,int(3*np.size(t0,1)/4)],tf[:,int(3*np.size(t0,1)/4)],'r')
    draw_flight_time_line(t0[:,int(np.size(t0,1)-1)],tf[:,int(np.size(t0,1)-1)],'r')

    # ************************************************************************
    # porkchop contours
    CS_1 = ax.contour(t0, tf, c3f_t1, levels=contour_range, colors="g")
    ax.clabel(CS_1,inline=1,fmt=fmt)
    CS_2 = ax.contour(t0, tf, c3f_t2, levels=contour_range, colors="b")
    ax.clabel(CS_2,inline=1,fmt=fmt)

    # ************************************************************************
    # labels and axes
    # rightax -- ARRIVAL JD-2400000.0
#    rightax = ax.twinx()
    # bottomax -- DEPARTURE JD-2400000.0
#    bottomax = ax.twiny()

    # set to bottom instead of top (given its a twiny)
#    bottomax.xaxis.set_ticks_position("bottom")
#    bottomax.xaxis.set_label_position("bottom")

    # labels
    ax.set_xlabel("DEPARTURE DATE",fontsize=10)
    ax.set_ylabel("ARRIVAL DATE",fontsize=10)
#    rightax.set_ylabel("ARRIVAL JD-2400000.0",fontsize=10)
#    bottomax.set_xlabel("DEPARTURE JD-2400000.0",fontsize=10)

    # primary axes ticks
    ax.set_xticks(np.arange(t0.min(), t0.max(), 5))
    ax.set_yticks(np.arange(tf.min(), tf.max(), 20))

    # set rot of xlabels
    ax.set_xticklabels(ax.get_xticks(), rotation=90)

    ax.xaxis.set_major_formatter(hfmt)
    ax.yaxis.set_major_formatter(hfmt)

    # shift
#    rightax.spines["right"].set_position(("outward",40))
#    bottomax.spines["bottom"].set_position(("axes",-0.18))

#    fig.text(0.5,0.5, "WORK IN PROGRESS", fontsize=50, color="gray",
#             rotation=45, ha="center", va="center", alpha=0.5)


    plt.title(f"EARTH-MARS BALLISTIC TRANSFER TRAJECTORIES \n {y1}-{m1}-{d1} - {y2}-{m2}-{d2} \n ARRIVAL C3 [$km^2/s^2$]",
              fontsize=12)
    ax.grid("on",linestyle=":")

    plt.savefig("plots/porkchop.jpg")
#    plt.show()

if __name__=="__main__":
    main()
