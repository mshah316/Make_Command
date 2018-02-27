
fakemake:  fakemake.o Graph.o hmap.o
	gcc -DIVAL fakemake.o Graph.o hmap.o -o fakemake

fakemake.o: fakemake.c Graph.h hmap.h
	gcc -DIVAL -c fakemake.c

Graph.o: Graph.c Graph.h
	gcc -c Graph.c

hmap.o: hmap.c hmap.h
	gcc -DIVAL -c hmap.c

clean:
	rm -f *.o fakemake
