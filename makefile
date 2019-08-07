CPP_SOURCES = $(wildcard */*.cpp */*/*.cpp */*/*/*.cpp)
HEADERS = $(wildcard include/*.h)
CPP_OBJ = ${CPP_SOURCES:.cpp=.o} 

all: run

fs.exec: main.o ${CPP_OBJ}
	g++ -v -o $@ $^

%.o: %.cpp ${HEADERS}
	g++ -c $< -o $@ -I./include

main.o: main.cpp ${HEADERS}
	g++ -c $< -o $@ -I./include

run: fs.exec hd.img
	./fs.exec

clean:
	rm -rf *.exec
	rm -rf *.bin *.o *.dis
	rm -rf */*.bin */*.o
	rm -rf */*/*.bin */*/*.o
	rm -rf */*/*/*.bin */*/*/*.o

hd.img:
	qemu-img create -f raw hd.img 1M