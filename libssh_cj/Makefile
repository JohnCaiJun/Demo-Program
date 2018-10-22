TARGET = ssh-client-cj

cc = g++ -std=c++11 

LIB = -lssh

all: ${TARGET}

${TARGET}: main.cc
	${cc} -o $@ $^ $(LIB)

.PHONY: clean

clean:
	rm ${TARGET}
