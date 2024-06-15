objects := main.o libpix.o libpixnet.o
CC := cc
CLAGS := -Wall
INCLUDES := 
BUILD := build

pixcel: $(BUILD)/$(objects)
	cc $(CFLAGS) -o pixcel $(objects) 

$(BUILD)/main.o: main.c
	cc $(CFLAGS) -c main.c $(CLAGS)

clean :
	rm pixcel $(objects)
