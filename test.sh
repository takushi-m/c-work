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
echo OK
