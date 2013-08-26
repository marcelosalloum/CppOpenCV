from distutils.core import setup
from distutils.extension import Extension
from Cython.Distutils import build_ext

setup(
    name = 'DyCppInterface',
    version = '1.0',
    author = 'Marcelo Salloum dos Santos',
    # The ext modules interface the cpp code with the python one:
    ext_modules=[
        Extension("rectangle",
            sources=["rectangle.pyx", "cpp_rect.cpp"], # Note, you can link against a c++ library instead of including the source
            include_dirs=[".","source", "/opt/local/include/opencv", "/opt/local/include"],
            language="c++",
            # extra_link_args = ['-arch x86_64'],
            # extra_link_args = ['-arch i386', '-arch x86_64'],
            library_dirs=['/opt/local/lib', 'source', '/Users/marcelosalloum/Projects/CppOpenCV/python_cpp_interface/source'],
            runtime_library_dirs=['source'],
            libraries=['LibCppOpenCV'])
    ],
    cmdclass = {'build_ext': build_ext},
)

# gcc-4.2 -fno-strict-aliasing -fno-common -dynamic -arch x86_64 -g -O2 -DNDEBUG -g -O3 -I. -Isource -I/opt/local/include/opencv -I/opt/local/include -I/Library/Frameworks/Python.framework/Versions/2.7/include/python2.7 -c rectangle.cpp -o build/temp.macosx-10.6-intel-2.7/rectangle.o
# g++-4.2 -bundle -undefined dynamic_lookup -arch x86_64 -g build/temp.macosx-10.6-intel-2.7/rectangle.o build/temp.macosx-10.6-intel-2.7/cpp_rect.o -L/opt/local/lib -Lsource -lopencv_core -lLibCppOpenCV -o /Users/marcelosalloum/Desktop/python_cpp_interface/rectangle.so
# g++-4.2 -bundle -undefined dynamic_lookup -arch x86_64 -g build/temp.macosx-10.6-intel-2.7/rectangle.o build/temp.macosx-10.6-intel-2.7/cpp_rect.o -L/opt/local/lib -L/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/src/lib -lopencv_core -lLibCppOpenCV -o /Users/marcelosalloum/Desktop/python_cpp_interface/rectangle.so