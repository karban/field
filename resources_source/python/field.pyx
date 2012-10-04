from libcpp.vector cimport vector
from libcpp.map cimport map
from libcpp.pair cimport pair
from libcpp cimport bool
from cython.operator cimport preincrement as incr, dereference as deref

cdef extern from "<string>" namespace "std":
    cdef cppclass string:
        string()
        string(char *)
        char * c_str()

cdef extern from "limits.h":
    int c_INT_MIN "INT_MIN"
    int c_INT_MAX "INT_MAX"
    int c_DOUBLE_MIN "DOUBLE_MIN"
    int c_DOUBLE_MAX "DOUBLE_MAX"

cdef extern from "../../src/pythonlabagros.h":
    # PyProblem
    cdef cppclass PyProblem:
        PyProblem(int clear)

        void clear()
        void refresh()

        char *getName()
        void setName(char *name)

        void solve()


    char *pyVersion()
    void pyQuit()

    char *pyInput(char *str)
    void pyMessage(char *str)

    void pyOpenDocument(char *str) except +
    void pySaveDocument(char *str) except +
    void pyCloseDocument()

    void pySaveImage(char *str, int w, int h) except +

# Problem
cdef class __Problem__:
    cdef PyProblem *thisptr

    # Problem(clear)
    def __cinit__(self, int clear = 0):
        self.thisptr = new PyProblem(clear)

    def __dealloc__(self):
        del self.thisptr

    # clear
    def clear(self):
        self.thisptr.clear()

    # refresh
    def refresh(self):
        self.thisptr.refresh()

    # name
    property name:
        def __get__(self):
            return self.thisptr.getName()
        def __set__(self, name):
            self.thisptr.setName(name)

    # solve
    def solve(self):
        self.thisptr.solve()

# problem
__problem__ = __Problem__()
def problem(int clear = False):
    if (clear):
        __problem__.clear()
    return __problem__

# version()
def version():
    return pyVersion()

# quit()
def quit():
    pyQuit()

# input()
def input(char *str):
    return pyInput(str)

# message()
def message(char *str):
    pyMessage(str)

def open_document(char *str):
    pyOpenDocument(str)

def save_document(char *str):
    pySaveDocument(str)

def close_document():
    pyCloseDocument()

def save_image(char *str, int w = 0, int h = 0):
    pySaveImage(str, w, h)
