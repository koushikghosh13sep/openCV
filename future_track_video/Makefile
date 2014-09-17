CC= g++
LDFLAGS = -lopencv_core -lopencv_highgui -lopencv_imgproc -lopencv_video

main: main.cpp processor.o future_tracked.o

processor.o: processor.h processor.cpp

future_tracked.o: future_tracked.cpp future_tracked.h

clean:
	rm *.o main
