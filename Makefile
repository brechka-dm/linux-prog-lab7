all: task1-1 task1-2 task2 task3
task1-1: task1-1.cpp
	g++ task1-1.cpp -o task1-1
task1-2: task1-2.cpp
	g++ task1-2.cpp -o task1-2
task2: task2.cpp
	g++ task2.cpp -o task2
task3: task3.cpp
	g++ task3.cpp -o task3 -std=c++1z

