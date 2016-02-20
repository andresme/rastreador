CC		=	gcc
CFLAGS	=	-std=gnu99 -g -lm
LIBS	=	-lm
CHECK	=	-Wall -Wextra

all: main.o 
	$(CC) $(CHECK) $(CFLAGS) -o main main.o $(LIBS)

clean:
	rm -f *.o main
