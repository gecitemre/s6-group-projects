import numpy
from matplotlib import pyplot


def fourier_series(signal, period, num_coefficients):
    coefficients = []
    for k in range(num_coefficients + 1):
        S = 0
        for n in range(period):
            S += signal[n] * numpy.exp(-1j * 2 * numpy.pi * n * k / period)
        coefficients.append(S / period)
    return coefficients


def inverse_fourier_series(coefficients, period, n):
    S = 0
    for k, coefficient in enumerate(coefficients):
        S += coefficient * numpy.exp(1j * 2 * numpy.pi * n * k / period)
    return S


square_wave = [-10] * 500 + [10] * 500

for n in (1, 5, 10, 50, 100, 500, 999):
    pyplot.plot(square_wave, label="square wave")
    coefficients = fourier_series(square_wave, len(square_wave), n)
    reconstructed = [
        inverse_fourier_series(coefficients, len(square_wave), n)
        for n in range(len(square_wave))
    ]
    print(reconstructed)
    print("sa")
    pyplot.plot(reconstructed, label="reconstructed")
    pyplot.show()
    print("sa2")
