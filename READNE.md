# C++ reference model for simple RTL-style pipeline/retiming experiments.

## The program compares:
- combinational model: output at cycle N
- pipelined model: output at cycle N + 1

## Supported signals:
- reset
- valid
- a, b, c inputs
- y output

## Expression:
y = (a + b) * c

## Make
~~~bash
make
~~~
## Test
~~~bash
make test
~~~
## Clean
~~~bash
make clean
~~~