CXX = g++
CXXFLAGS = -O2 -Wall `pkg-config --cflags opencv4` 
LDFLAGS = `pkg-config --libs opencv4`
EXTRAFLAGS =

TARGET = captureImage
OBJS = captureImage.o estimateLandingTarget.o estimateGateCenter.o goTarget.o



all: $(TARGET)

$(TARGET): $(OBJS)
	$(CXX) -o $@ $^ $(LDFLAGS) $(EXTRAFLAGS)

%.o: %.cpp
	$(CXX) $(CXXFLAGS) $(EXTRAFLAGS) -c $< -o $@

clean:
	rm -f $(TARGET) $(OBJS)
