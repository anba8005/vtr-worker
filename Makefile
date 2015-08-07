CXXFLAGS =	-O2 -std=c++11 -g -Wall -fmessage-length=0

OBJS =		main.o AWorker.o VTRWorker.o FakeWorker.o BMDTools.o sdk/DeckLinkAPIDispatch.o

LIBS = -ldl -lpthread

TARGET =	vtr-worker

$(TARGET):	$(OBJS)
	$(CXX) -o $(TARGET) $(OBJS) $(LIBS)

all:	$(TARGET)

clean:
	rm -f $(OBJS) $(TARGET)
