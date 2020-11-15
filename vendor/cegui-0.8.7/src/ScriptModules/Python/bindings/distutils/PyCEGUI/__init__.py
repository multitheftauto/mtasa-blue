import os
import os.path

# atrocious and unholy!
def get_my_path():
    import fake
    return os.path.dirname(os.path.abspath(fake.__file__))

libpath = get_my_path()
#print "libpath =", libpath
os.environ['PATH'] = libpath + ";" + os.environ['PATH']

from PyCEGUI import *
