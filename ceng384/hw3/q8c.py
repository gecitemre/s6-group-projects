import numpy
from matplotlib import pyplot
from scipy.signal import sawtooth
from q8a import spectral_coefficients
from q8b import SignalFromSpectralCoefficients

t = numpy.linspace(-0.5, 0.5, 1000)

square_wave = [-10] * 500 + [10] * 500
for n in (1, 5, 10, 50, 100):
    pyplot.plot(t, square_wave, label="square wave")
    coefficients = spectral_coefficients(square_wave, len(square_wave), n)
    reconstructed = SignalFromSpectralCoefficients(coefficients, 1000)
    reconstructed.plot("Reconstructed Square Wave", f"square_wave_{n}.png")