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


try 0 0
try 42 42
try 21 '5+20-4'
try 10 '5+6-1'
try 2 '1 + 1'
try 4 '2*2'
try 10 '3*2+2*2'
try 2 '2*10/10'
echo OK
