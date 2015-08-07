CXXFLAGS =	-O2 -g -Wall -fmessage-length=0

OBJS =		main.o VTRWorker.o sdk/DeckLinkAPIDispatch.o

LIBS = -ldl -lpthread

TARGET =	vtr-worker

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
