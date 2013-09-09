import sys

from rectangle import Rectangle
r = Rectangle(1, 2, 3, 4)
print r
print "Original area:", r.getArea()
r.move(1,2)
# r.normalizeBright(str(sys.argv[1]), str(sys.argv[2]))
r.openCV(str(sys.argv[1]), str(sys.argv[2]))
# r.openCV("/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot0.jpeg", "/Users/marcelosalloum/Projects/CppOpenCV/CppOpenCV/Debug/aux/foot1.jpeg")
print r
print "Area is invariante under rigid motions:", r.getArea()
r += Rectangle(0,0,1,1)
print r
print "Now the aread is:", r.getArea()
