import sys

from cheese_opencv import CheeseOpenCV
antiShake = CheeseOpenCV()
antiShake.openCV(str(sys.argv[1]), str(sys.argv[2]))