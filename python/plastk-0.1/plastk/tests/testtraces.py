"""
Tests for the plastk.misc.traces module.
$Id: testtraces.py 230 2008-03-03 04:53:52Z jprovost $
"""

import unittest
from numpy import zeros
from plastk.misc.traces import InMemoryRecorder

class TestDataRecorder(unittest.TestCase):

    def setUp(self):

        self.recorder = self.recorder_type(*self.recorder_args,
                                           **self.recorder_kw)
        self.recorder.add_variable('testvar',dim = [2,2],
                                   dtype = self.dtype)

        self.testdata = [zeros((2,2),dtype=self.dtype) * i
                         for i in xrange(10)]
        
    def testMain(self):

        assert len(self.recorder['testvar']) == 0

        N = len(self.testdata)
        
        # test sequential storage
        for i,d in enumerate(self.testdata):
            self.recorder['testvar'][i] = d

        for i,d in enumerate(self.recorder['testvar']):
            assert (d==self.testdata[i]).all()


        # test dynamic random access growth
        for i,d in enumerate(self.testdata):
            self.recorder['testvar'][i+1000] = d

        assert (self.recorder['testvar'][1000:1000+N] == self.testdata).all()

        self.recorder['testvar'][100:100+N] = self.testdata

        assert (self.recorder['testvar'][100:100+N] == self.testdata).all()

        


class TestInMemoryRecorder(TestDataRecorder):

    recorder_type = InMemoryRecorder
    recorder_args = []
    recorder_kw = {}

class TestInMemoryInt(TestInMemoryRecorder):
    dtype = int

class TestInMemoryFloat(TestInMemoryRecorder):
    dtype = float

#####################################################

cases = [
    TestInMemoryFloat,
    TestInMemoryInt
    ]

suite = unittest.TestSuite()
suite.addTests(unittest.makeSuite(case) for case in cases)

