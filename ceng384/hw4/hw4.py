import numpy
from scipy.io import wavfile

def fft(x):
    N = len(x)
    if N <= 1:
        return x
    odd = fft(x[::2]) # check indexing NOTE
    even = fft(x[1::2]) # check indexing NOTE

    return numpy.concatenate([odd + even * numpy.exp(-2j * numpy.pi * numpy.arange(N // 2) / N),
                              odd - even * numpy.exp(-2j * numpy.pi * numpy.arange(N // 2) / N)])

rate, message = wavfile.read("encoded.wav")
message = fft(message)
first_half = message[:len(message) // 2 + 1]
second_half = message[len(message) // 2 + 1:]
message2 = numpy.concatenate([numpy.flip(first_half), numpy.flip(second_half)])
message3 = numpy.fft.ifft(message2)

wavfile.write("decoded.wav", rate, message3.astype(numpy.int16))
