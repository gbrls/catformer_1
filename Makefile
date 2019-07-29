src = $(wildcard *.c)
obj = $(src:.c=.o)

#LDFLAGS = -lGL -lglut -lpng -lz -lm -lSDL2
LDFLAGS = -lSDL2 -lSDL2_mixer

catformer: $(obj)
	$(CC) -o $@ $^ $(LDFLAGS) ./src/main.c

.PHONY: clean
clean:
	rm -f $(obj) ./catformer 
