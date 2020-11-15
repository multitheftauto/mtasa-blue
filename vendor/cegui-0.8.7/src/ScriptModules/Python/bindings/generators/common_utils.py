#/***********************************************************************
#    filename:   common_utils.py
#    created:    13/8/2010
#    author:     Martin Preisler (with many bits taken from python ogre)
#
#    purpose:    Groups some otherwise repeated code
#*************************************************************************/
#/***************************************************************************
# *   Copyright (C) 2004 - 2013 Paul D Turner & The CEGUI Development Team
# *
# *   Thanks to Roman Yakovenko for advices and great work on Py++!
# *   Thanks to Andy Miller for his python-ogre CEGUI bindings!
# *
# *   License: generator is GPL3 (python ogre code generators are also GPL)
# *            generated code is MIT as the rest of CEGUI

import os, sys, time

from pyplusplus import module_builder
from pyplusplus.module_builder import call_policies
from pygccxml import declarations

import extract_documentation as exdoc

def get_root_base_path():
    """Returns directory the source tarball was extracted to.
    """

    def ancestor_dir(start_dir, distance = 1):
        cur_dir = start_dir
        for _ in range(distance):
            cur_dir = os.path.dirname(cur_dir)

        return cur_dir

    file_dir = os.path.dirname(__file__)
    return ancestor_dir(file_dir, 6)

GLOBAL_PACKAGE_VERSION = "0.8.0"
GCCXML_PATH = "C:\\Users\\Martin Preisler\\Devel\\PythonPackages\\gccxml_bin\\v09\\win32\\bin\\"
ROOT_BASE_PATH = get_root_base_path()
OUTPUT_DIR = os.path.join(ROOT_BASE_PATH, "cegui", "src", "ScriptModules", "Python", "bindings", "output")
INCLUDE_PATHS = [
    os.path.join(ROOT_BASE_PATH, "cegui", "include"),
    os.path.join(ROOT_BASE_PATH, "build", "cegui", "include") # the usual build include dir
]
BINDINGS_HEADER_PATH = os.path.relpath(os.path.join(ROOT_BASE_PATH, "cegui", "src", "ScriptModules", "Python", "bindings", "generators", "include"))

def createModuleBuilder(input_file, defined_symbols):
    ret = module_builder.module_builder_t(
        files = [
            os.path.join(BINDINGS_HEADER_PATH, input_file)
        ],
        gccxml_path = GCCXML_PATH,
        include_paths = INCLUDE_PATHS,
        define_symbols = defined_symbols,
        indexing_suite_version = 2
    )

    ret.BOOST_PYTHON_MAX_ARITY = 25
    ret.classes().always_expose_using_scope = True

    return ret

def addConstants(mb, constants):
    tmpl = 'boost::python::scope().attr("%(name)s") = %(value)s;'
    for name, value in constants.items():
        mb.add_registration_code(tmpl % dict(name = name, value = str(value)))

def addVersionInfo(mb, name, version):
    import datetime

    t = datetime.datetime.now().isoformat(' ').strip()

    docstring = "%s - version %s" % (name, version)

    addConstants(mb, {
                    'CompileTime__' : '__TIME__',
                    'CompileDate__' : '__DATE__',
                    'Version__' : '"%s"' % version.replace("\n", "\\\n" ),
                    '__doc__' : '"%s"' % docstring.replace("\n", "\\\n" )
                    })

def setDefaultCallPolicies(ns):
    # Set the default policy to deal with pointer/reference return types to reference_existing object
    # as this is the CEGUI Default.
    mem_funs = ns.calldefs()
    mem_funs.create_with_signature = True #Generated code will not compile on
    #MSVC 7.1 if function has throw modifier.
    for mem_fun in mem_funs:
        if mem_fun.call_policies:
            continue
        if declarations.is_pointer (mem_fun.return_type) or declarations.is_reference (mem_fun.return_type):
            mem_fun.call_policies = call_policies.return_value_policy(call_policies.reference_existing_object)

def createDocumentationExtractor():
    return exdoc.doc_extractor("")

def writeModule(mb, output_dir):
    mb.code_creator.user_defined_directories.append(output_dir)
    mb.split_module(output_dir)

def excludeAllPrivate(cls):
    cls.decls(declarations.matchers.access_type_matcher_t("private")).exclude()

    # include back pure virtual private functions
    query = declarations.matchers.access_type_matcher_t("private") \
            & declarations.virtuality_type_matcher_t(declarations.VIRTUALITY_TYPES.PURE_VIRTUAL)

    cls.mem_funs(query, allow_empty = True).include()

# this adds string converters - should only be used in CEGUI package!
def addStringConverters(mb):
    mb.add_declaration_code(
"""
struct CEGUI_String_to_python : public boost::python::converter::expected_from_python_type<CEGUI::String>
{
    static PyObject* convert(const CEGUI::String& s)
    {
        // use native byteorder
        int byteorder = 0;

        // "replace" replaces invalid utf32 chars with "?"
        
        // python wants the size of the buffer, not length of the string,
        // this is the reason for the sizeof
        return boost::python::incref(
            PyUnicode_DecodeUTF32((const char*)(s.ptr()), s.length() * sizeof(CEGUI::utf32), "replace", &byteorder)
        );
    }
};

struct CEGUI_String_from_python
{
    CEGUI_String_from_python()
    {
      boost::python::converter::registry::push_back(
        &convertible,
        &construct,
        boost::python::type_id<CEGUI::String>(),
        &boost::python::converter::expected_from_python_type<CEGUI::String>::get_pytype);
    }
    
    static void* convertible(PyObject* obj_ptr)
    {
        // we allow str() and unicode() objects to be converted to CEGUI::String
        // NOTE: Python3 only has unicode strings
        if (PyUnicode_Check(obj_ptr)
#if PY_MAJOR_VERSION < 3
            || PyString_Check(obj_ptr)
#endif
        )
        {
            return obj_ptr;
        }
        return 0;
    }
    
    static void construct(
      PyObject* obj_ptr,
      boost::python::converter::rvalue_from_python_stage1_data* data)
    {
#if PY_MAJOR_VERSION < 3
        if (!PyUnicode_Check(obj_ptr))
        {
            // no need for unicode, this is just plain str()
            const char* value = PyString_AsString(obj_ptr);
            if (value == 0)
                boost::python::throw_error_already_set();

            void* storage = ((boost::python::converter::rvalue_from_python_storage<CEGUI::String>*)data)->storage.bytes;
            new (storage) CEGUI::String(value);
            data->convertible = storage;
        }
        else
#endif
        {
            // we have to employ a bit more machinery since this is utf16 coded string
            // we encode given unicode object (ucs2 - utf16) to utf8
            PyObject* utf8 = PyUnicode_AsUTF8String(obj_ptr);
            // then we get the raw bytes of the utf8 python object
#if PY_MAJOR_VERSION < 3
            const CEGUI::utf8* value = (CEGUI::utf8*)PyString_AsString(utf8);
#else
            const CEGUI::utf8* value = (CEGUI::utf8*)PyBytes_AsString(utf8);
#endif
            if (value == 0)
                boost::python::throw_error_already_set();

            void* storage = ((boost::python::converter::rvalue_from_python_storage<CEGUI::String>*)data)->storage.bytes;
            new (storage) CEGUI::String(value);
            data->convertible = storage;
            // now we don't need utf8 anymore, CEGUI::String holds the data for us now, by decreasing
            // refcount to utf8, we are practically deallocating it
            boost::python::decref(utf8);
        }
    }
};
"""
    )

    mb.add_registration_code(
"""
boost::python::to_python_converter<
    CEGUI::String,
    CEGUI_String_to_python,
    true>();

CEGUI_String_from_python();
""", tail = False # force converters to be there first
    )

def addSupportForString(mb):
    # set String as exposed, because we provide converters
    mb.global_ns.namespace("CEGUI").class_("String").already_exposed = True

    # we have to set use_make_functions to True for all variables that have CEGUI::String type
    # converters are only used when using setter / getter functions

    for var in mb.global_ns.variables():
        # we don't check the type directly, because a const could be at the end
        # this could be replaced with var.type == "CEGUI::String" or var.type == "CEGUI::String const"
        if str(var.type).startswith("CEGUI::String"):
            var.use_make_functions = True
            # we have to use return_by_value to make converters work
            var.set_getter_call_policies(call_policies.return_value_policy(call_policies.return_by_value))

def verbose_generate(name, generator):
    print("Generating %s bindings..." % (name))
    start_time = time.clock()
    generator()
    print("Finished %s bindings. (took %g seconds)\n" % (name, (time.clock() - start_time)))
