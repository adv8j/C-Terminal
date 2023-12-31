all: cmdline2.o ls.o ls_a.o ls_A.o ls_i.o ls_g.o ls_l.o grep.o ps.o cp.o mv.o trim.o
	gcc cmdline2.o trim.o ls.o ls_a.o ls_A.o ls_i.o ls_g.o ls_l.o grep.o ps.o cp.o mv.o -lreadline -o cmdline2 && rm *.o

ls.o: ls.c
	gcc -c ls.c

ls_a.o: ls_a.c
	gcc -c ls_a.c

ls_A.o: ls_A.c
	gcc -c ls_A.c

ls_i.o: ls_i.c
	gcc -c ls_i.c

ls_g.o: ls_g.c
	gcc -c ls_g.c

ls_l.o: ls_l.c
	gcc -c ls_l.c

grep.o: grep.c
	gcc -c grep.c

ps.o: ps.c
	gcc -c ps.c

cp.o: cp.c
	gcc -c cp.c

mv.o: mv.c
	gcc -c mv.c

cmdline2.o: cmdline2.c
	gcc -c cmdline2.c

trim.o: trim.c
	gcc -c trim.c
