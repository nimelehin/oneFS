CPP_SOURCES = $(wildcard *.cpp */*.cpp */*/*.cpp */*/*/*.cpp */*/*/*/*.cpp)
HEADERS = $(wildcard include/*.h)
CPP_OBJ = ${CPP_SOURCES:.cpp=.o}
DEBUG_OPTIONS = 

all: ext2fs.exec

ext2fs.exec: main.o ${CPP_OBJ}
	g++ -v -o $@ $^

%.o: %.cpp ${HEADERS}
	g++ -c $< -o $@ ${DEBUG_OPTIONS}

main.o: main.cpp ${HEADERS}
	g++ -c $< -o $@ ${DEBUG_OPTIONS}

run: ext2fs.exec
	./ext2fs.exec

drive:
	qemu-img create -f raw hd.img 20M

clean:
	rm -rf *.exec
	rm -rf *.bin *.o *.dis
	rm -rf */*.bin */*.o
	rm -rf */*/*.bin */*/*.o
	rm -rf */*/*/*.bin */*/*/*.o
	rm -rf */*/*/*/*.bin */*/*/*/*.o