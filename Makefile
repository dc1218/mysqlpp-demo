CC:=g++

CCFLAG = -g -w -O0 -std=c2x			

CCFLAG += -I/usr/include/mysql -I/usr/include/mysql++

LDFLAGS = -lmysqlclient \
			-lmysqlpp 
			

SRC=$(wildcard *.cpp)

OBJ=$(patsubst %,%.o,$(SRC))

PROG=$(SRC:%.cpp=%)

all:$(PROG)

$(PROG):%:%.cpp
	$(CC) $(CCFLAG) $< -o $@ $(LDFLAGS)
.PHONY:
clean:
	rm -rf *.o $(PROG)

