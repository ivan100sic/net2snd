CCC=g++ -pthread -O2 -std=c++14 -Wall
all: test transmit receive pulse-t

include: sound_playback_device.hpp sound_recording_device.hpp \
	bytes.hpp oscillator.hpp common-p.hpp

test: test.cpp include
	$(CCC) test.cpp -lasound -o test

transmit: transmit.cpp include
	$(CCC) transmit.cpp -lasound -o transmit

receive: receive.cpp include
	$(CCC) receive.cpp -lasound -o receive

pulse-t: pulse-t.cpp include
	$(CCC) pulse-t.cpp -lasound -o pulse-t

transmit-p: transmit-p.cpp include
	$(CCC) transmit-p.cpp -lasound -o transmit-p

receive-p: receive-p.cpp include
	$(CCC) receive-p.cpp -lasound -o receive-p

pair: transmit-p receive-p
