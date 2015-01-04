"""
Unit tests for vector databases.

$Id: testvectordb.py 229 2008-03-03 04:38:00Z jprovost $
"""
import unittest
import pickle
import plastk.base.rand as rand
from plastk.misc.vectordb import *
from plastk.misc.utils import *
from numpy import array,arange

class TestVectorDB(unittest.TestCase):

    dim = 2
    N = 1000
    probes = [array((x,y)) for x in arange(0,1.25,0.25) for y in arange(0,1.25,0.25)]

    
    def setUp(self):
        rand.seed(0)        
        self.data = [(rand.uniform(0,1,(self.dim,)),None) for i in range(self.N)]
        for x,y in self.data:
            self.db.add(x,y)
            
    def testKNearest(self):
        k = 10
        
        def probe_cmp(a,b,probe):
            ax,ay = a
            bx,by = b
            diff = norm(ax-probe)-norm(bx-probe)
            return int(diff/abs(diff))


        for p in self.probes:
            k_nearest,dists = self.db.k_nearest(p,k)
            self.data.sort(lambda a,b:probe_cmp(a,b,p))        
            for (k1,v1),(k2,v2) in zip(k_nearest,self.data[:k]):                
                assert (k1==k2).all() and v1==v2
                                  

    def testFindInRadius(self):
        r = 0.1
        for p in self.probes:
            found,dists = self.db.find_in_radius(p,r)
            dists = [norm(p-x) for x,y in self.data]
            truth = [(x,y) for d,(x,y) in zip(dists,self.data) if d <= r]
            for k1,v1 in truth:
                match_found = False
                for k2,v2 in found:
                    if (k1==k2).all() and v1==v2:
                        match_found = True
                        break
                self.failUnless(match_found)
            for k1,v1 in found:
                match_found = False
                for k2,v2 in truth:
                    if (k1==k2).all() and v1==v2:
                        match_found = True
                        break
                self.failUnless(match_found)

             
class TestFlatVectorDB(TestVectorDB):
    def setUp(self):
        self.db = FlatVectorDB()
        TestVectorDB.setUp(self)

class TestVectorTree(TestVectorDB):
    def setUp(self):
        self.db = VectorTree()
        TestVectorDB.setUp(self)

        
class TestPickleVectorTree(TestVectorTree):
    def setUp(self):
        TestVectorTree.setUp(self)
        s = pickle.dumps(self.db)
        self.orig_db = self.db
        self.db = pickle.loads(s)

    def testContents(self):
        for xo,vo in self.orig_db.all():
            (x,v),d = self.db.nearest(xo)
            assert (x==xo).all()
        
cases = [TestFlatVectorDB,
         TestVectorTree,
         TestPickleVectorTree,
         ]
suite = unittest.TestSuite()
suite.addTests([unittest.makeSuite(case) for case in cases])
