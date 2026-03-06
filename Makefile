CC = gcc
CFLAGS = -Wall -g -O0 -I./include
LDFLAGS = -L./lib -l:libraylib.a -lm

APP = edger

SRC := $(wildcard ./src/*.c)
OBJS := $(patsubst ./src/%.c,./obj/%.o,$(SRC))

all: $(APP)

$(APP): $(OBJS)
	$(CC) $(OBJS) -o $(APP) $(LDFLAGS)

./obj/%.o: ./src/%.c | ./obj
	$(CC) $(CFLAGS) -c $< -o $@

./obj:
	mkdir -p obj

clean:
	rm -f $(OBJS) $(APP)
