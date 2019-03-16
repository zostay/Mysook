CPPFLAGS=-std=c++14 -c -g -Wall -I/usr/local/include -I../lib/SimPanel -I../lib/SimRTC -I../lib/SimLogger -I../lib/Firmware -I../lib/Logger -I../lib/Panel -I../lib/RTC -Isrc -I../lib/Color -I../lib/Canvas32 -I../lib/Sim_Adafruit_GFX -I../lib/Simduino
LDFLAGS=-Wall -g -L/usr/local/lib
LIBS=-lallegro -lallegro_main -lallegro_primitives

sim-tclock: main.o tclock.o Canvas32.o Sim_Adafruit_GFX.o Logger.o Firmware.o RTC.o Simduino.o
	g++ $(LDFLAGS) $(LIBS) -o sim-tclock main.o tclock.o Canvas32.o Sim_Adafruit_GFX.o Logger.o Firmware.o RTC.o Simduino.o

main.o: src/main.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/main.cpp -o main.o

tclock.o: src/tclock.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/tclock.cpp -o tclock.o

Canvas32.o: ../lib/Canvas32/Canvas32.o ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Canvas32/Canvas32.cpp -o Canvas32.o

Sim_Adafruit_GFX.o: ../lib/Sim_Adafruit_GFX/Sim_Adafruit_GFX.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Sim_Adafruit_GFX/Sim_Adafruit_GFX.cpp -o Sim_Adafruit_GFX.o

Logger.o: ../lib/Logger/Logger.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Logger/Logger.cpp -o Logger.o

Firmware.o: ../lib/Firmware/Firmware.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Firmware/Firmware.cpp -o Firmware.o

RTC.o: ../lib/RTC/RTC.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/RTC/RTC.cpp -o RTC.o

Simduino.o: ../lib/Simduino/Simduino.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Simduino/Simduino.cpp -o Simduino.o

clean:
	rm -f *.o sim-tclock
