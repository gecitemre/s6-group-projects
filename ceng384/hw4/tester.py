from numpy.fft import fft, ifft
from numpy import flip, concatenate, int16
from scipy.io import wavfile

def test(encoded, decoded, rate):
    N = len(encoded)
    reference = fft(encoded)
    flipped_reference = flip(reference)
    decoded_reference = ifft(concatenate([flipped_reference[N//2 + 1:], flipped_reference[:N//2 + 1]]))[1:]
    diff = decoded_reference - decoded[:-1]
    condition = diff < 1e-7
    print("Test passed:", condition.all())
