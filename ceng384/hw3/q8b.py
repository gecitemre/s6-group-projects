from matplotlib import pyplot
from numpy import exp, pi

SAVE_FOLDER = "figures"

class SignalFromSpectralCoefficients:
    def __init__(self, coefficients, period):
        self.coefficients = coefficients
        self.period = period

    def __getitem__(self, n):
        S = 0
        for k, coefficient in enumerate(self.coefficients):
            S += coefficient * exp(1j * 2 * pi * n * k / self.period)
        return S

    def __iter__(self):
        for n in range(self.period):
            yield self[n]

    def __len__(self):
        return self.period
    
    def plot(self, title, save_path):
        pyplot.title(title)
        pyplot.plot(range(self.period), [abs(item) for item in self])
        pyplot.savefig(SAVE_FOLDER + "/" + save_path)
        pyplot.clf()