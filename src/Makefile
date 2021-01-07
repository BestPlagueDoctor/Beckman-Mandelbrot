calcgcc:
	g++ -g -o calc -Ofast objcalc_simd.cpp -march=native 
debug:
	g++ -o calc -g objcalc_simd.cpp -march=native -fsanitize=address -fsanitize=undefined -lasan -lubsan -pg
clean:
	rm calc
new:
	g++ -o calc -g fullvector.cpp -Ofast -march=native -std=c++20
oldmath:
	g++ -o calc -g oldmath.cpp -Ofast -march=native
