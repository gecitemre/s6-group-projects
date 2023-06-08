import numpy
from matplotlib import pyplot

t = numpy.linspace(-100, 100, 1000)
y = 3 * numpy.exp(-2 * t)* numpy.heaviside(t, 1) - 3 * numpy.exp(-t) 
pyplot.plot(t, y)
pyplot.savefig("figures/q1c.pgf")
