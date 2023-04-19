from matplotlib import pyplot


class Signal:
    def __init__(self, data, start):
        self.data = data
        self.start = start
        self.end = start + len(data) - 1

    def __getitem__(self, index):
        if index < self.start or index > self.end:
            return 0
        return self.data[index - self.start]

    def __mul__(self, other):
        return ConvolutionResult(self, other)
    
    def plot(self, title, save_path):
        pyplot.title(title)
        pyplot.plot(range(self.start, self.end + 1), self.data)
        pyplot.savefig(save_path)
        pyplot.clf()


class ConvolutionResult(Signal):
    def __init__(self, signal1, signal2):
        self.signal1 = signal1
        self.signal2 = signal2

    @property
    def start(self):
        return self.signal1.start + self.signal2.start

    @property
    def end(self):
        return self.signal1.end + self.signal2.end

    @property
    def data(self):
        return [self[i] for i in range(self.start, self.end + 1)]

    def __getitem__(self, index):
        result = 0
        for i in range(self.start, self.end + 1):
            result += self.signal1[i] * self.signal2[index - i]
        return result


with open("hw2_signal.csv", "r", encoding="ascii") as file:
    raw_data = [float(item) for item in file.read().split(",")]

x = Signal(raw_data[1:], int(raw_data[0]))
x.plot("Original Signal", "images/original.png")

h = Signal([1], 5)
convolution = h * x
convolution.plot("Convolution Result", "images/convolution.png")

for N in (3, 5, 10, 20):
    m = Signal([1 / N] * N, 0)
    moving_average = m * x
    moving_average.plot(f"Moving Average Convolution (N={N})", f"images/moving_average_{N}.png")
