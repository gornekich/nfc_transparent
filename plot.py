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
        ["./a.out"], stdout=subprocess.PIPE, stderr=subprocess.PIPE)
    print(result.stderr)
    inp = result.stdout.split(b';')

    signal_start = int(inp[0]) == 1
    signal_raw = inp[1].split()
    signal = []
    for s in signal_raw:
        signal.append(float(s))

    result_start = int(inp[2]) == 1
    result_raw = inp[3].split()
    result = []
    for s in result_raw:
        result.append(float(s))

    plot = PlotSignals()
    plot.add_signal(signal_start, signal)
    plot.add_signal(result_start, result)
    plot.plot()
    
    max_err = 0
    for i in range(len(signal) - 1):
        err = abs(signal[i] - result[i])
        if(err > max_err):
            max_err = err
    print(f"Max error: {max_err * 1000000000} ns")
    print(f"Timer resolution: {1000000000 / 64000000} ns")
