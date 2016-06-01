TARGET=main
SRCS=$(wildcard *.cpp)
OBJS=$(patsubst %.cpp, %.o, ${SRCS})
LINKFLAG=-lpthread


${TARGET}:${OBJS}
	g++ -o $@ $^ ${LINKFLAG}

.cpp.o:
	g++ -g -c $< -o $@

clean:
	-$(RM) ${OBJS} ${TARGET}


