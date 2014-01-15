
CC = gcc
CFLAGS = -Wall -Os
LDLIBS = -ljpeg
OBJECTS = main.o picture.o ve.o veisp.o veavc.o vejpeg.o
TARGET = jepoc

$(TARGET): $(OBJECTS)
	gcc -o $@ $^ $(LDLIBS)

clean:
	rm $(TARGET) $(OBJECTS)

