#!/usr/bin/env python2

#/***********************************************************************
#    filename:   generateCEGUIOpenGLRenderer.py
#    created:    13/8/2010
#    author:     Martin Preisler (with many bits taken from python ogre)
#
#    purpose:    Generates CEGUI OpenGL Renderer binding code
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

PACKAGE_NAME = "PyCEGUIOpenGLRenderer"
PACKAGE_VERSION = common_utils.GLOBAL_PACKAGE_VERSION
MODULE_NAME = PACKAGE_NAME

OUTPUT_DIR = os.path.join(common_utils.OUTPUT_DIR, "CEGUIOpenGLRenderer")

def filterDeclarations(mb):
    # by default we exclude everything and only include what we WANT in the module
    mb.global_ns.exclude()

    CEGUI_ns = mb.global_ns.namespace("CEGUI")

    # RendererModules/OpenGL/RendererBase.h
    rendererBase = CEGUI_ns.class_("OpenGLRendererBase")
    rendererBase.include()
    rendererBase.mem_fun("setViewProjectionMatrix").exclude()
    rendererBase.mem_fun("getViewProjectionMatrix").exclude()
    rendererBase.noncopyable = True

    # RendererModules/OpenGL/GLRenderer.h
    renderer = CEGUI_ns.class_("OpenGLRenderer")
    renderer.include()
    renderer.mem_fun("setViewProjectionMatrix").exclude() # CEGUI::mat4Pimpl
    renderer.noncopyable = True

    # RendererModules/OpenGL/GeometryBufferBase.h
    geometryBufferBase = CEGUI_ns.class_("OpenGLGeometryBufferBase")
    geometryBufferBase.include()
    geometryBufferBase.mem_fun("getMatrix").exclude() # CEGUI::mat4Pimpl
    geometryBufferBase.noncopyable = True

    # RendererModules/OpenGL/OpenGLTextureTarget.h
    textureTarget = CEGUI_ns.class_("OpenGLTextureTarget")
    textureTarget.include()
    textureTarget.noncopyable = True

    # RendererModules/OpenGL/OpenGLViewportTarget.h
    viewportTarget = CEGUI_ns.class_("OpenGLViewportTarget")
    viewportTarget.include()
    viewportTarget.noncopyable = True

    # RendererModules/OpenGL/GL3Renderer.h
    renderer3 = CEGUI_ns.class_("OpenGL3Renderer")
    renderer3.include()
    renderer3.noncopyable = True

    # RendererModules/OpenGL/Shader.h
    shader3 = CEGUI_ns.class_("OpenGL3Shader")
    shader3.include()
    shader3.noncopyable = True

    # RendererModules/OpenGL/StateChangeWrapper.h
    stateChangeWrapper3 = CEGUI_ns.class_("OpenGL3StateChangeWrapper")
    stateChangeWrapper3.include()
    stateChangeWrapper3.noncopyable = True

def generate():
    ### disable unnecessary warnings
    # py++ will create a wrapper
    messages.disable(messages.W1023, messages.W1025, messages.W1026, messages.W1027, messages.W1031)
    # can't be overridden in python
    messages.disable(messages.W1049)

    # "CEGUIBASE_EXPORTS" seems to help with internal compiler error with VS2008SP1 and gccxml 0.9
    mb = common_utils.createModuleBuilder("python_CEGUIOpenGLRenderer.h", ["OPENGL_GUIRENDERER_EXPORTS", "CEGUIBASE_EXPORTS"])
    CEGUI_ns = mb.global_ns.namespace("CEGUI")

    mb.register_module_dependency(cegui_base.OUTPUT_DIR)

    common_utils.addSupportForString(mb)

    filterDeclarations(mb)

    common_utils.setDefaultCallPolicies(CEGUI_ns)

    ## add additional version information to the module to help identify it correctly 
    # todo: this should be done automatically
    common_utils.addVersionInfo(mb, PACKAGE_NAME, PACKAGE_VERSION)

    # Creating code creator. After this step you should not modify/customize declarations.
    mb.build_code_creator(module_name = MODULE_NAME, doc_extractor = common_utils.createDocumentationExtractor())

    common_utils.writeModule(mb, OUTPUT_DIR)

if __name__ == "__main__":
    common_utils.verbose_generate("CEGUIOpenGLRenderer", generate)
