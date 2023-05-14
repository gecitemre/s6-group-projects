from matplotlib import pyplot
from numpy import exp, pi, linspace

SAVE_FOLDER = "figures"

t = linspace(-0.5, 0.5, 1000)

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
    
    def plot(self, name):
        pyplot.plot(t, self, label="Reconstructed Signal")
        pyplot.legend()
        pyplot.savefig(SAVE_FOLDER + "/" + name + ".svg", format = "svg")
        pyplot.clf()