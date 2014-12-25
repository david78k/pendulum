from numpy import array
from math import *

class InvertedPendulum:
    def __init__(self, rotational=array([pi, 0]), translational=array([0,0])):
        self.time = 0.0
        self.rotational = rotational
        self.translational = array([0.0, 0.0])
        self.control = 0.0

        self.LENGTH = 0.5
        self.GRAVITY = 9.81
        self.MASS = 0.4
        self.DT = 0.01

    def systemEquation(self, y):
        theta = y[0]
        omega = y[1]
        dOmega = 1 / self.LENGTH * \
            ( \
              -self.GRAVITY * sin(theta) \
              - (self.control / self.MASS) * cos(theta) - 0.01 * omega \
            )

        return array([omega, dOmega])

    def rungeKutta(self, y):
        k1 = self.systemEquation(y)
        k2 = self.systemEquation(y + self.DT / 2.0 * k1)
        k3 = self.systemEquation(y + self.DT / 2.0 * k2)
        k4 = self.systemEquation(y + self.DT * k3)

        return y + (self.DT / 6.0) * (k1 + 2.0 * k2 + 2.0 * k3 + k4)

    def update(self, controlInput):
        self.time += self.DT
        # Bound control input
        self.control = clip(-20.0, controlInput, 20.0)

        # Prevent overflow (when training, some MLPs really don't work at all)
        self.translational[0] = clip(-1.0e300, self.translational[0], 1.0e300)
        self.translational[1] = clip(-1.0e300, self.translational[1], 1.0e300)

        self.translational[1] += (self.control / self.MASS) * self.DT
        self.translational[0] += self.translational[1] * self.DT

        self.rotational = self.rungeKutta(self.rotational)

def clip(lo, x, hi):
    return lo if x <= lo else hi if x >= hi else x
