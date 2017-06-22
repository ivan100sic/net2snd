CCC=g++ -pthread -lasound -O2 -std=c++14 -Wall
all: test transmit receive pulse-t

include: sound_playback_device.hpp sound_recording_device.hpp \
	bytes.hpp oscillator.hpp

test: test.cpp include
	$(CCC) test.cpp -o test

transmit: transmit.cpp include
	$(CCC) transmit.cpp -o transmit

receive: receive.cpp include
	$(CCC) receive.cpp -o receive

pulse-t: pulse-t.cpp include
	$(CCC) pulse-t.cpp -o pulse-t