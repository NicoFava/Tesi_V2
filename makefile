LIBS := `root-config --libs`
INCS := `root-config --cflags`

main: main.o funzioni.o
	g++ -o main main.o funzioni.o ${LIBS} 

main.o: main.cpp funzioni.h
	g++ -c -o main.o main.cpp ${INCS} 

funzioni.o: funzioni.cpp funzioni.h
	g++ -c -o funzioni.o funzioni.cpp ${INCS} 

clean:
	rm -f *.o main