CC = g++
CFLAGS = -std=c++11 -g

all:archive

dataStructure.o:
	$(CC) $(CFLAGS) -c ./dataStructure/b+tree.cpp -o dataStructure.o

manage.o:
	$(CC) $(CFLAGS) -c ./manage/blockManage.cpp -o manage.o
	
file.o:
	$(CC) $(CFLAGS) -c ./manage/file.cpp -o file.o

database.o:
	$(CC) $(CFLAGS) -c ./main/database.cpp -o database.o

archive: dataStructure.o manage.o file.o database.o
	ar rcs database.a dataStructure.o manage.o file.o database.o

database: archive
	$(CC) $(CFLAGS) -o database ./main/main.cpp database.a

performance: archive
	$(CC) $(CFLAGS) -o performance ./test/performance.cpp database.a

validity: archive
	$(CC) $(CFLAGS) -o validity ./test/validity.cpp database.a

clean:
	rm -f *.o *.out *.a database performance validity
