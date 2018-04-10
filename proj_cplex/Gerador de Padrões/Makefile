CXX	=	g++
FLAGS_1	=	-O2	-ansi	-DIL_STD#	-Wall	-pedantic
DEBUG	=	-pg	-g	-ansi	-DIL_STD#	-Wall	-pedantic		

GC_I		= /usr/local/include/
GC_1		= /usr/local/lib/
GC		= -lgecodeflatzinc -lgecodedriver	-lgecodegist	-lgecodesearch	-lgecodeminimodel	-lgecodeset	-lgecodefloat	-lgecodeint	-lgecodekernel	-lgecodesupport	-lpthread

CPLEX_FLAGS	=	-I$(CG_I)  -L$(CG_1) $(GC)

OBJ	=	main.o

main:	$(OBJ)
	$(CXX)	$(FLAGS_1)	-o	main	$(OBJ)	$(CPLEX_FLAGS)

main.o:	main.cpp	
	$(CXX)	-std=c++14	$(FLAGS_1)	$(CPLEX_FLAGS)	-c	main.cpp	#$<	-o	$@

clean:
	rm	-rf	*.o	main
