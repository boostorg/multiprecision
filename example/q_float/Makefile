###############################################################
#
# Intel architecture Pentium IV flags.
# Enable if desired
#
###############################################################
I86_FLAGS  =
#I86_FLAGS := -march=pentium4 -mfpmath=sse

CPP        = g++
CC         = gcc
CFLAGS     = ${I86_FLAGS}                          \
             -march=native                         \
             -mtune=native                         \
             -Wall                                 \
             -Wextra                               \
             -O3                                   \
             -finline-functions                    \
             -finline-limit=32

CXXFLAGS   = ${CFLAGS}                             \
             -I.                                   \
             -I/mnt/c/boost/boost_1_76_0

###############################################################
#
# lists of header files for dependencies
#
###############################################################

QF_HEADERS     = q_float/q_float.h                    \
                 q_float/q_float_limits.h             \
                 q_float/q_float_exception.h          \
                 q_float/q_float_types.h              \
                 q_float/q_float_z.h                  \
                 q_float/q_float_qf.h

all: unix/q_float.exe

###############################################################
#
# make output directory if it is not present
#
###############################################################

unix :
	mkdir -p unix
	
###############################################################
#
# compile q_float files
#
###############################################################

unix/test.o: unix test.cpp ${QF_HEADERS}
	${CPP} ${CXXFLAGS} -c test.cpp -o unix/test.o

unix/q_float.o: q_float/q_float.cpp ${QF_HEADERS}
	${CPP} ${CXXFLAGS} -c q_float/q_float.cpp -o unix/q_float.o

unix/q_float_math.o: q_float/q_float_math.cpp ${QF_HEADERS}
	${CPP} ${CXXFLAGS} -c q_float/q_float_math.cpp -o unix/q_float_math.o

unix/q_float_gamma.o: q_float/q_float_gamma.cpp ${QF_HEADERS}
	${CPP} ${CXXFLAGS} -c q_float/q_float_gamma.cpp -o unix/q_float_gamma.o

unix/q_float_z_math.o: q_float/q_float_z_math.cpp ${QF_HEADERS}
	${CPP} ${CXXFLAGS} -c q_float/q_float_z_math.cpp -o unix/q_float_z_math.o

###############################################################
#
# link all files
#
###############################################################

TEST_OBJECTS   = unix/test.o
QF_OBJECTS     = unix/q_float.o unix/q_float_math.o unix/q_float_gamma.o unix/q_float_z_math.o

unix/q_float.exe : ${TEST_OBJECTS} ${QF_OBJECTS}
	${CPP} ${TEST_OBJECTS} ${QF_OBJECTS} ${LDFLAGS} -o unix/q_float.exe

clean : preclean
	rmdir unix

preclean :
	rm unix/*.*

touch :
	touch ./*.cpp ./*.c ./q_float/*.cpp ./q_float/*.h
