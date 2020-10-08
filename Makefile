all:
	g++ -g ./src/test.cpp -I./include -L./lib -lseal -o test
