#! /usr/bin/env python

from distutils.core import setup

import RLearning

setup(name="RLearning",
      version=RLearning.version,
      description="Simple Reinforcement Learning Algorithms Library",
      author="Jose Antonio Martin H",
      author_email="jamartinh@fdi.ucm.es",
      url="http://www.dia.fi.upm.es/~jamartin/download.htm",
      packages=['RLearning'],
      package_data={'RLearning': ['Examples/*.*']}
      ,
     )

