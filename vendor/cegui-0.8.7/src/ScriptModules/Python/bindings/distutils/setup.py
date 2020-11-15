import sys

CEGUI_BASEDIR = "C:\\Users\\Admin\\Devel\\CEGUI\\cegui_mk2"
is_64bits = sys.maxsize > 2**32
BOOST_BASEDIR = "C:\\Program Files (x86)\\boost\\boost_1_46_1" if is_64bits else "C:\\Program Files\\boost\\boost_1_46_1"

from distutils.core import setup, Extension
from distutils.sysconfig import get_python_inc

import os
from glob import *

global_defines = []# [("CEGUI_STATIC", "1")]
# turn on exceptions for MSVC
global_extra_compile_args = ["/EHsc"]
global_include_dirs = [get_python_inc(plat_specific = True), "../", "../../../../../include", "../../../../../../build/cegui/include", BOOST_BASEDIR]
global_library_dirs = [CEGUI_BASEDIR + "/build/lib", BOOST_BASEDIR + "/lib"]
# Windows is special and picks the libraries magically!
global_libraries = []

PyCEGUI_sources = glob(os.path.join("..", "output", "CEGUI", "*.cpp"))
PyCEGUIOpenGLRenderer_sources = glob(os.path.join("..", "output", "CEGUIOpenGLRenderer", "*.cpp"))
PyCEGUIOgreRenderer_sources = glob(os.path.join("..", "output", "CEGUIOgreRenderer", "*.cpp"))
PyCEGUINullRenderer_sources = glob(os.path.join("..", "output", "CEGUINullRenderer", "*.cpp"))

setup(
    name = "PyCEGUI",
    version = "0.8",
    description = "Python bindings for CEGUI library",
    long_description =
"""Crazy Eddie's GUI System is a free library providing windowing
and widgets for graphics APIs / engines where such functionality
is not natively available, or severely lacking. The library is
object orientated, written in C++, and targeted at games developers
who should be spending their time creating great games, not building GUI sub-systems.

note: For Linux and MacOSX packages, see http://www.cegui.org.uk, we provide them
      in SDKs. Distutils package is only provided for Windows since it's hard to
      install the binding there as it involves lots of wrappers and nasty tricks.
      Shame on you Windows!""",
    author = "CEGUI team",
    author_email = "team@cegui.org.uk",
    #maintainer = "Martin Preisler", # authors get shadowed by this
    #maintainer_email = "preisler.m@gmail.com",
    url = "http://www.cegui.org.uk",
    license = "MIT",
    platforms = ["Windows", "Linux", "MacOSX"],
    
    classifiers = [
        "Development Status :: 4 - Beta",
        "Environment :: MacOS X",
        "Environment :: Win32 (MS Windows)",
        "Environment :: X11 Applications",
        "Intended Audience :: Developers",
        "License :: OSI Approved :: MIT License",
        "Natural Language :: English",
        "Operating System :: MacOS :: MacOS X",
        "Operating System :: Microsoft :: Windows",
        "Operating System :: POSIX",
        "Programming Language :: Python",
        "Programming Language :: C++"
        "Topic :: Games/Entertainment",
        "Topic :: Multimedia :: Graphics",
        "Topic :: Software Development :: Libraries :: Python Modules",
        "Topic :: Software Development :: User Interfaces",
        "Topic :: Software Development :: Widget Sets",
    ],
    
    packages = [
        "PyCEGUI" # nasty nasty wrapper for a nasty nasty OS
    ],
    
    # this doesn't work as I expected so I am doing data files further down
    #package_data = { "PyCEGUI/datafiles": [CEGUI_BASEDIR + "/datafiles/*"] },
        
    ext_modules = [
        Extension(
            "PyCEGUI.PyCEGUI",
            PyCEGUI_sources,
            define_macros = global_defines,
            extra_compile_args = global_extra_compile_args,
            include_dirs = global_include_dirs + ["../output/CEGUI"],
            library_dirs = global_library_dirs,
            libraries = ["CEGUIBase"] + global_libraries,
        ),
        Extension(
            "PyCEGUIOpenGLRenderer",
            PyCEGUIOpenGLRenderer_sources,
            define_macros = global_defines,
            extra_compile_args = global_extra_compile_args,
            include_dirs = global_include_dirs + ["../output/CEGUIOpenGLRenderer"],
            library_dirs = global_library_dirs,
            libraries = ["CEGUIBase", "CEGUIOpenGLRenderer"] + global_libraries,
        ),
        # no ogre for now, I will use SDK binaries later for this
        #Extension(
        #    "PyCEGUIOgreRenderer",
        #    PyCEGUIOgreRenderer_sources,
        #    define_macros = global_defines,
        #    extra_compile_args = global_extra_compile_args,
        #    include_dirs = global_include_dirs + ["../output/CEGUIOgreRenderer"],
        #    library_dirs = global_library_dirs,
        #    libraries = ["CEGUIBase", "CEGUIOgreRenderer"] + global_libraries,
        #),
        Extension(
            "PyCEGUINullRenderer",
            PyCEGUINullRenderer_sources,
            define_macros = global_defines,
            extra_compile_args = global_extra_compile_args,
            include_dirs = global_include_dirs + ["../output/CEGUINullRenderer"],
            library_dirs = global_library_dirs,
            libraries = ["CEGUIBase", "CEGUINullRenderer"] + global_libraries,
        ),
    ],
    
    # the first string is directory where the files should go
    # - leave empty for C:/Python26 for example
    data_files = [
        # we have to bundle CEGUIBase.dll, CEGUIOpenGLRenderer.dll, etc...
        ("Lib/site-packages/PyCEGUI",
        # this is obviously a workaround, I would be happy to hear what the clean
        # solution should look like
            [
                BOOST_BASEDIR + "/lib/boost_python-vc90-mt-1_46_1.dll",
                
                CEGUI_BASEDIR + "/build/bin/CEGUIBase.dll",
                CEGUI_BASEDIR + "/build/bin/CEGUIOpenGLRenderer.dll",
                CEGUI_BASEDIR + "/build/bin/CEGUINullRenderer.dll",
                CEGUI_BASEDIR + "/build/lib/CEGUICoreWindowRendererSet.dll",
                CEGUI_BASEDIR + "/build/lib/CEGUIFreeImageImageCodec.dll",
                CEGUI_BASEDIR + "/dependencies/bin/freeimage.dll",
                CEGUI_BASEDIR + "/build/lib/CEGUIExpatParser.dll",
            ]
        ),
        
        # distutils doesn't allow to bundle folders (or to be precise: I have no idea how to do that)
        # therefore I do this the ugly way!
        ("Lib/site-packages/PyCEGUI/datafiles/animations",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/animations", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/configs",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/configs", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/fonts",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/fonts", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/imagesets",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/imagesets", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/layouts",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/layouts", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/looknfeel",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/looknfeel", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/lua_scripts",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/lua_scripts", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/schemes",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/schemes", "*")),
        ),
        ("Lib/site-packages/PyCEGUI/datafiles/xml_schemas",
            glob(os.path.join(CEGUI_BASEDIR + "/datafiles/xml_schemas", "*")),
        )
    ]
)
