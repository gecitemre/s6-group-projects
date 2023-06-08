import numpy
from scipy.io import wavfile
from tester import test # This is to verify the implementation, uses numpy.fft
import matplotlib
from matplotlib import pyplot

matplotlib.use("pgf")

def fft(x: numpy.ndarray) -> numpy.ndarray:
    N = len(x)
    if N <= 1:
        return x

    even = fft(x[::2])
    odd = fft(x[1::2]) * numpy.exp(-2j * numpy.pi * numpy.arange(N // 2) / N)
    return numpy.concatenate([even + odd,
                              even - odd])

def ifft(X: numpy.ndarray) -> numpy.ndarray:
    return numpy.flip(fft(X)) / len(X)

def plot(signal, name):
    pyplot.plot(signal)
    pyplot.savefig("figures/" + name + ".pgf")
    pyplot.clf()

rate, encoded = wavfile.read("encoded.wav")
plot(encoded, "encoded")
N = len(encoded)
dft_encoded = fft(encoded)
plot(dft_encoded.real, "dft_encoded")
flipped = numpy.flip(dft_encoded)
dft_decoded = numpy.concatenate([flipped[N//2:], flipped[:N//2]])
plot(dft_decoded.real, "dft_decoded")
decoded = ifft(dft_decoded)
plot(decoded.real, "decoded")
assert test(encoded, decoded)
wavfile.write("decoded.wav", rate, decoded.real.astype(numpy.int16))
