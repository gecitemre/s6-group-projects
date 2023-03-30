"""CENG384 - HW1 - Q7b Solution"""

from matplotlib import pyplot

IMAGES_PATH = "assets/q7b/"


class Signal:
    def __init__(self, signal, start, a, b):
        self.signal = signal
        self.start = start
        self.a = a
        self.b = b

    def __getitem__(self, index):
        return self.signal[self.a * index + self.b - self.start]


def shift_n_scale(signal_name):
    """
    Read the CSV file with the signal name, shift and scale the signal,
        and save the results as PNG files.

    This functions reads a signal x[n], and produces x[a*n + b] for a and b
    """

    with open(signal_name + ".csv", "r", encoding="ascii") as file:
        data = [float(item) for item in file.read().split(",")]
    start = int(data[0])
    a = int(data[1])
    b = int(data[2])
    signal = Signal(data[3:], start, a, b)
    end = start + len(signal.signal) - 1

    new_start = (start - b) // a
    new_end = (end - b) // a
    pyplot.xlim(new_start, new_end)

    pyplot.plot(range(start, end + 1), signal.signal,linewidth=1)

    if new_start > new_end:
        domain = range(new_start, new_end, -1)
    else:
        domain = range(new_start, new_end + 1)
    pyplot.plot(
        domain,
        [signal[i] for i in domain],
        linewidth=1,
    )
    pyplot.legend(
        ["x[n]", "x[" + str(a) + "n " + ("+" if b >= 0 else "") + str(b) + "]"],
        loc="lower right",
        fontsize=8,
    )
    pyplot.savefig(IMAGES_PATH + signal_name)
    pyplot.clf()


shift_n_scale("sine_part_b")
shift_n_scale("shifted_sawtooth_part_b")
shift_n_scale("chirp_part_b")
