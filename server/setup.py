import os
from distutils.core import setup

def read(fname):
    return open(os.path.join(os.path.dirname(__file__), fname)).read()

setup(
    name='snotstream',
    version='1.0',
    description='Online monitoring tools for the SNO+ experiment',
    author='Richie Bonventre (and maybe some Andy Mastbaum)',
    author_email='rbonv@hep.upenn.com',
    url='http://github.com/mastbaum/snotstream',
    long_description=read('README.md'),
    scripts=['bin/snotstream'],
    packages=['snotstream'],
)

