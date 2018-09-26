war.exe: war.c graph.c graph.h map.c map.h
	gcc -c *.c
	gcc -g *.o -o war -lallegro-5.0.10-monolith-md
clean:
	rm *.o
	rm *.exe