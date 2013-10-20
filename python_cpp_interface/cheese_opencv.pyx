
cdef extern from "cpp_cheese.h":
    ctypedef struct c_CheeseOpenCV "CheeseOpenCV":
        void openCV(char* path1, char* path2)
        void normalizeBright(char* path1, char* path2)
    c_CheeseOpenCV *new_CheeseOpenCV "new CheeseOpenCV" ()
    void del_CheeseOpenCV "delete" (c_CheeseOpenCV *rect)

cdef class CheeseOpenCV:
    cdef c_CheeseOpenCV *thisptr      # hold a C++ instance which we're wrapping
    def __cinit__(self):
        self.thisptr = new_CheeseOpenCV()
    def __dealloc__(self):
        del_CheeseOpenCV(self.thisptr)
    def openCV(self, path1, path2):
        self.thisptr.openCV(path1, path2)
    def normalizeBright(self, path1, path2):
        self.thisptr.normalizeBright(path1, path2)
                
    def __repr__(self):
        return "CheeseOpenCV" ()

"""
cdef Rectangle r = Rectangle(1, 2, 3, 4)
print r
print "Original area:", r.getArea()
r.move(1,2)
print r
print "Area is invariante under rigid motions:", r.getArea()
r += Rectangle(0,0,1,1)
print r
print "Now the aread is:", r.getArea()
"""
