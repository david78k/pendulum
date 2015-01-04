"""
Unit tests for PLASTK

$Id: __init__.py 229 2008-03-03 04:38:00Z jprovost $
"""

import unittest,doctest,os,re,fnmatch

# Automatically discover all test*.py files in this directory
__all__ = [re.sub('\.py$','',f)
           for f in fnmatch.filter(os.listdir(__path__[0]),'test*.py')]

all_doctest = sorted(fnmatch.filter(os.listdir(__path__[0]),'test*.txt'))

suite = unittest.TestSuite()

def all(verbosity=1):
    return unittest.TextTestRunner(verbosity=verbosity).run(suite)
def debug():
    suite.debug()
    

for test_name in __all__:
    exec 'import '+test_name
    test_module = locals()[test_name]
    try:
        print 'Checking module %s for test suite...' % test_name,
        suite.addTest(getattr(test_module,'suite'))
        print 'found.'
    except AttributeError,err:
        print err

for filename in all_doctest:
    print 'Loading doctest file', filename,'...'
    suite.addTest(doctest.DocFileSuite(filename))
    print 'done.'
