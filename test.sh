#! /bin/bash
try() {
	expected="$1"
	input="$2"

	./mcc "$input" > tmp.s
	gcc tmp.s
	./a.out
	actual="$?"

	if [ "$actual" = "$expected" ]; then
		echo "$input => $actual"
	else
		echo "$expected expected, but got $actual"
		exit 1
	fi
}


try 0 '0;'
try 42 '42;'
try 21 '5+20-4;'
try 10 '5+6-1;'
try 2 '1 + 1;'
try 4 '2*2;'
try 10 '3*2+2*2;'
try 2 '2*10/10;'
try 5 '(1+3)*10/8;'
try 4 '(6-4)*(1+1);'
try 1 '+1;'
try 4 '-1*(0-4);'
try 1 '1==1;'
try 0 '1!=1;'
try 1 '2>1;'
try 0 '1>2;'
try 1 '2>=1;'
try 1 '2>=2;'
try 0 '1>=2;'
try 1 '1<2;'
try 0 '2<1;'
try 1 '1<=2;'
try 1 '2<=2;'
try 0 '2<=1;'
try 10 '1+1+1+1+1+1+1+1+1+1;'
try 10 'a=5;b=2;a+b+3;'
try 10 'a=1;b=10;return a*b;'
try 42 'hoge=10;fuga=30;return hoge+fuga+2;'
try 1 'if (1>0) return 1;return 0;'
try 1 'if (1==0) return 0;return 1;'
try 1 'if (1>0) return 1; else return 0;'
try 1 'if (1==0) return 0; else return 1;'
try 10 'i=0;while (i<10) i=i+1; return i;'

echo OK
