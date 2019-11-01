TARGET= testmycall
HEADER= testmycall.h
CC= gcc
FLAGS= -pthread

all: $(TARGET)

$(TARGET): $(TARGET).c $(HEADER)
	$(CC) $(FLAGS) -o $(TARGET) $(TARGET).c

clean:
	$(RM) $(TARGET)
