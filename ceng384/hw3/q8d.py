import numpy
from matplotlib import pyplot
from scipy.signal import sawtooth
from q8a import spectral_coefficients
from q8b import SignalFromSpectralCoefficients

t = numpy.linspace(-0.5, 0.5, 1000)

sawtooth_wave = sawtooth(2 * numpy.pi * t)
for n in (1, 5, 10, 50, 100):
    pyplot.plot(t, sawtooth_wave, label="sawtooth wave")
    coefficients = spectral_coefficients(sawtooth_wave, len(sawtooth_wave), n)
    reconstructed = SignalFromSpectralCoefficients(coefficients, 1000)
    reconstructed.plot("Reconstructed Sawtooth Wave", SAVE_PATH + f"sawtooth_wave_{n}.png")