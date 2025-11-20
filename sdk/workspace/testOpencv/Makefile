CXX = g++
CXXFLAGS = -O2 -Wall `pkg-config --cflags opencv4`
LDFLAGS = `pkg-config --libs opencv4`

TARGET = tuneBinarization
SRC = tuneBinarization.cpp

CAM_TARGET = captureImage
CAM_SRC = captureImage.cpp

CAP_ETERNAL = cap_eternal
CAP_SRC = cap_eternal.cpp

LAB_TARGET = tuneGrayExtractionLab
LAB_SRC = tuneGrayExtractionLab.cpp

LAB1_TARGET = tuneBlueExtraction
LAB1_SRC = tuneBlueExtraction.cpp

LAB2_TARGET = testLandingTarget
LAB2_SRC = testLandingTarget.cpp

LAB3_TARGET = testGlay
LAB3_SRC = testGlay.cpp

all: $(TARGET) $(CAM_TARGET) $(CAP_ETERNAL) $(LAB_TARGET) $(LAB1_TARGET) $(LAB2_TARGET) $(LAB3_TARGET)

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(CAM_TARGET): $(CAM_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(CAP_ETERNAL): $(CAP_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LAB_TARGET): $(LAB_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LAB1_TARGET): $(LAB1_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LAB2_TARGET): $(LAB2_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

$(LAB3_TARGET): $(LAB3_SRC)
	$(CXX) $(CXXFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(TARGET) $(CAM_TARGET) $(CAP_ETERNAL) $(LAB_TARGET) $(LAB1_TARGET) $(LAB2_TARGET) $(LAB3_TARGET)