"""CENG384 - HW1 - Q7a Solution"""

from matplotlib import pyplot


def decompose(signal_name):
    """Open the CSV file with the signal name, decompose it into even and odd components,
    and save the results as PNG files."""

    with open(signal_name + ".csv", "r", encoding="ascii") as file:
        data = [float(item) for item in file.read().split(",")]
    start = int(data[0])
    signal = data[1:]
    end = start + len(signal) - 1

    pyplot.title("Original Signal")
    pyplot.plot(range(start, end + 1), signal)
    pyplot.savefig("images/" + signal_name + "_original.png")
    pyplot.clf()

    if abs(start) > end:
        signal = signal + [0] * (abs(start) - end)
        end = -start
    else:
        signal = [0] * (end - abs(start)) + signal
        start = -end

    even = [(x + y) / 2 for x, y in zip(signal, signal[::-1])]
    odd = [(x - y) / 2 for x, y in zip(signal, signal[::-1])]

    pyplot.title("Even Component")
    pyplot.plot(range(start, end + 1), even)
    pyplot.savefig("images/" + signal_name + "_even.png")
    pyplot.clf()

    pyplot.title("Odd Component")
    pyplot.plot(range(start, end + 1), odd)
    pyplot.savefig("images/" + signal_name + "_odd.png")
    pyplot.clf()


decompose("sine_part_a")
decompose("shifted_sawtooth_part_a")
decompose("chirp_part_a")
