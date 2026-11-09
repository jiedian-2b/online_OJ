TARGET := test_run
SRC := /root/my_oj/test/runner_test.cpp

CXX := g++
CXXFLAGS := -std=c++11 -Wall

$(TARGET): $(SRC)
	$(CXX) $(CXXFLAGS) $(SRC) -o $(TARGET)


clean:
	rm -f $(TARGET)

.PHONY: clean