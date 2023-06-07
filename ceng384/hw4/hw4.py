import numpy
from scipy.io import wavfile
from tester import test # This is to verify the implementation, uses numpy.fft

def fft(x: numpy.ndarray) -> numpy.ndarray:
    N = len(x)
    if N <= 1:
        return x

    # Even and odd with mathematical indexing
    odd = fft(x[::2])
    even = fft(x[1::2]) * numpy.exp(-2j * numpy.pi * numpy.arange(N // 2) / N)
    return numpy.concatenate([odd + even,
                              odd - even])

def ifft(X: numpy.ndarray) -> numpy.ndarray:
    return fft(X)[::-1] / len(X)


rate, encoded = wavfile.read("encoded.wav")
N = len(encoded)
dft_coefficients = fft(encoded)
flipped = numpy.flip(dft_coefficients)
decoded = ifft(numpy.concatenate([flipped[N//2 + 1:], flipped[:N//2 + 1]]))
test(encoded, decoded)
wavfile.write("decoded.wav", rate, decoded.real.astype(numpy.int16))
