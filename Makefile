mcc: mcc.c

test: mcc
	./mcc -test
	./test.sh

clean:
	rm -f mcc *.o tmp.s a.out
