breakit: breakit.cpp
		g++ -std=c++11 breakit.cpp -o breakit -lm -lncurses
		
clean:
		rm -f breakit