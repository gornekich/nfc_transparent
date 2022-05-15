#! /usr/bin/python3

import struct
import subprocess
import sys
from typing import Text
import matplotlib.pyplot as plt


class PlotSignals:
    def __init__(this):
        this.offset = 0

    def add_signal(this, start, edges):
        y_start = 0
        if start == True:
            y_start = 1
        else:
            y_start = 0

        y = [y_start + this.offset]
        x = [0.0]
        y_state = y_start
        x_accum = 0
        for edge in edges:
            y.append(y_state + this.offset)
            x.append(x_accum + edge)
            if y_state == 1:
                y_state = 0
            else:
                y_state = 1
            y.append(y_state + this.offset)
            x.append(x_accum + edge)
            x_accum += edge

        plt.plot(x[:-1], y[:-1])
        this.offset += 0.01

    def plot(this):
        plt.show()


if __name__ == "__main__":
    result = subprocess.run(
        ["build/signal"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(result.stderr)
    inp = result.stdout.split(b';')

    plot = PlotSignals()

    for i in range(0, int(len(inp)) - 1, 2):
        signal_start = int(inp[i]) == 1
        signal_raw = inp[i + 1].split()
        signal = []
        for s in signal_raw:
            signal.append(float(s))
        plot.add_signal(signal_start, signal)

    plot.plot()
    