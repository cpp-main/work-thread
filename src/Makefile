TARGET=demo

OBJECTS=main.o work_thread.o

all: $(TARGET)

$(TARGET) : $(OBJECTS)
	g++ -o $@ $^ -pthread

clean:
	rm -f $(OBJECTS)
