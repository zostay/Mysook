CPPFLAGS=-std=c++14 -c -g -Wall -I/usr/local/include -I../lib/SimPanel -I../lib/SimRTC -I../lib/SimLogger -I../lib/Firmware -I../lib/Logger -I../lib/Panel -I../lib/RTC -Isrc -I../lib/Color
LDFLAGS=-Wall -g -L/usr/local/lib
LIBS=-lallegro -lallegro_main -lallegro_primitives

sim-tclock: main.o tclock.o Firmware.o RTC.o
	g++ $(LDFLAGS) $(LIBS) -o sim-tclock main.o tclock.o Firmware.o RTC.o

main.o: src/main.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/main.cpp -o main.o

tclock.o: src/tclock.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/tclock.cpp -o tclock.o

Firmware.o: ../lib/Firmware/Firmware.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Firmware/Firmware.cpp -o Firmware.o

RTC.o: ../lib/RTC/RTC.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/RTC/RTC.cpp -o RTC.o

clean:
	rm -f *.o sim-tclock
