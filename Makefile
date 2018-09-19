all: mapred 

eighth: mapred .c

	gcc -Wall -Werror -fsanitize=address mapred .c -o mapred 

clean:
	rm -rf mapred 
