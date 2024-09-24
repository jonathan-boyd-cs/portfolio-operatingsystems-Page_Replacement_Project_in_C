page_manager: page_manager_simulation.o paging.o
	gcc page_manager_simulation.o paging.o -o page_manager_sim.exe
main.o: page_manager_simulation.c
	gcc -c page_manager_simulation.c paging.c
paging.o: paging.c paging.h
	gcc -c paging.c
clean:
	rm *.o page_manager_sim.exe
