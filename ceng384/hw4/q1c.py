import numpy
from matplotlib import pyplot

t = numpy.linspace(-100, 100, 1000)
y = 3 * numpy.exp(-2 * t) - 3 * numpy.exp(-t) * numpy.heaviside(t, 1)
pyplot.plot(t, y)
pyplot.savefig("figures/q1c.pgf")
