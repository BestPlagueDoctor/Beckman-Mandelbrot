calcgcc:
	gcc -g -o calc -Ofast objcalc_simd.cpp -march=native 
debug:
	gcc -o calc -g objcalc_simd.cpp -march=native -fsanitize=address -fsanitize=undefined -lasan -lubsan -pg
clean:
	rm calc
new:
	gcc -o calc -g bitwise.cpp -Ofast -march=native 
oldmath:
	gcc -o calc -g oldmath.cpp -Ofast -march=native 


