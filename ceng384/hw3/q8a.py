from numpy import exp, pi

def spectral_coefficients(signal, period, num_coefficients):
    coefficients = []
    for k in range(num_coefficients + 1):
        S = 0
        for n in range(period):
            S += signal[n] * exp(-1j * 2 * pi * n * k / period)
        coefficients.append(S / period)
    return coefficients