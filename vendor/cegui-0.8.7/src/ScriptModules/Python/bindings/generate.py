#!/usr/bin/env python2

#/***********************************************************************
#    filename:   generate.py
#    created:    13/8/2010
#    author:     Martin Preisler (with many bits taken from python ogre)
#
#    purpose:    Generates all CEGUI related python bindings
#*************************************************************************/
#/***************************************************************************
# *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
# *
# *   Thanks to Roman Yakovenko for advices and great work on Py++!
# *   Thanks to Andy Miller for his python-ogre CEGUI bindings!
# *
# *   License: generator is GPL3 (python ogre code generators are also GPL)
# *            generated code is MIT as the rest of CEGUI

import generators.cegui_base

import generators.opengl_renderer
import generators.ogre_renderer
import generators.null_renderer

from generators.common_utils import verbose_generate
import os

if __name__ == "__main__":
    verbose_generate("CEGUIBase", generators.cegui_base.generate)

    verbose_generate("CEGUINullRenderer", generators.null_renderer.generate)
    verbose_generate("CEGUIOgreRenderer", generators.ogre_renderer.generate)
    verbose_generate("CEGUIOpenGLRenderer", generators.opengl_renderer.generate)
