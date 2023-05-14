from matplotlib import pyplot
from scipy.signal import sawtooth
from q8a import spectral_coefficients
from q8b import SignalFromSpectralCoefficients, t

square_wave = [-10] * 500 + [10] * 500
for n in (1, 5, 10, 50, 100):
    pyplot.plot(t, square_wave, label="Square Wave")
    coefficients = spectral_coefficients(square_wave, len(square_wave), n)
    reconstructed = SignalFromSpectralCoefficients(coefficients, 1000)
    reconstructed.plot(f"square_wave_{n}")