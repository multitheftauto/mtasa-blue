#!/usr/bin/env python2

#/***********************************************************************
#    filename:   generateCEGUINullRenderer.py
#    created:    12/11/2010
#    author:     Martin Preisler (with many bits taken from python ogre)
#
#    purpose:    Generates CEGUI Null Renderer binding code
#*************************************************************************/
#/***************************************************************************
# *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
# *
# *   Thanks to Roman Yakovenko for advices and great work on Py++!
# *   Thanks to Andy Miller for his python-ogre CEGUI bindings!
# *
# *   License: generator is GPL3 (python ogre code generators are also GPL)
# *            generated code is MIT as the rest of CEGUI

import os

from pyplusplus import function_transformers as ft
from pyplusplus import messages
from pyplusplus.module_builder import call_policies
from pygccxml import declarations

import common_utils
import cegui_base

PACKAGE_NAME = "PyCEGUINullRenderer"
PACKAGE_VERSION = common_utils.GLOBAL_PACKAGE_VERSION
MODULE_NAME = PACKAGE_NAME

OUTPUT_DIR = os.path.join(common_utils.OUTPUT_DIR, "CEGUINullRenderer")

def filterDeclarations(mb):
    # by default we exclude everything and only include what we WANT in the module
    mb.global_ns.exclude()

    CEGUI_ns = mb.global_ns.namespace("CEGUI")

    # RendererModules/Null/CEGUINullRenderer.h
    renderer = CEGUI_ns.class_("NullRenderer")
    renderer.include()
    renderer.noncopyable = True

def generate():
    ### disable unnecessary warnings
    # py++ will create a wrapper
    messages.disable(messages.W1023, messages.W1025, messages.W1026, messages.W1027, messages.W1031)
    # can't be overridden in python
    messages.disable(messages.W1049)

    # "CEGUIBASE_EXPORTS" seems to help with internal compiler error with VS2008SP1 and gccxml 0.9
    mb = common_utils.createModuleBuilder("python_CEGUINullRenderer.h", ["NULL_GUIRENDERER_EXPORTS", "CEGUIBASE_EXPORTS"])
    CEGUI_ns = mb.global_ns.namespace("CEGUI")

    mb.register_module_dependency(cegui_base.OUTPUT_DIR)

    common_utils.addSupportForString(mb)

    filterDeclarations(mb)

    common_utils.setDefaultCallPolicies(CEGUI_ns)

    ## add additional version information to the module to help identify it correctly 
    common_utils.addVersionInfo(mb, PACKAGE_NAME, PACKAGE_VERSION)

    # Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator(module_name = MODULE_NAME, doc_extractor = common_utils.createDocumentationExtractor())

    common_utils.writeModule(mb, OUTPUT_DIR)

if __name__ == "__main__":
    common_utils.verbose_generate("CEGUINullRenderer", generate)
