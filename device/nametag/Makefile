CPPFLAGS=-std=c++14 -c -g -Wall -I/usr/local/include -I../lib/SimPanel -I../lib/SimLogger -I../lib/Firmware -I../lib/Logger -I../lib/Panel -Isrc -I../lib/Color -I../lib/Canvas32 -I../lib/Sim_Adafruit_GFX/ -I../lib/Simduino/ -I../lib/Network/ -I../lib/WebServer/
LDFLAGS=-Wall -g -L/usr/local/lib
LIBS=-lallegro -lallegro_main -lallegro_primitives

sim-nametag: main.o nametag.o default-prog.o Firmware.o Canvas32.o Logger.o Sim_Adafruit_GFX.o Simduino.o  TomThumb.o
	g++ $(LDFLAGS) $(LIBS) -o sim-nametag main.o nametag.o default-prog.o Firmware.o Canvas32.o Logger.o Sim_Adafruit_GFX.o Simduino.o TomThumb.o

main.o: src/main.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/main.cpp -o main.o

nametag.o: src/nametag.cpp src/*.h ../lib/*/*.h
	g++ $(CPPFLAGS) src/nametag.cpp -o nametag.o

default-prog.o: src/default-prog.cpp
	g++ $(CPPFLAGS) src/default-prog.cpp -o default-prog.o

Firmware.o: ../lib/Firmware/Firmware.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Firmware/Firmware.cpp -o Firmware.o

Canvas32.o: ../lib/Canvas32/Canvas32.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Canvas32/Canvas32.cpp -o Canvas32.o

Logger.o: ../lib/Logger/Logger.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Logger/Logger.cpp -o Logger.o

Sim_Adafruit_GFX.o: ../lib/Sim_Adafruit_GFX/Sim_Adafruit_GFX.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Sim_Adafruit_GFX/Sim_Adafruit_GFX.cpp -o Sim_Adafruit_GFX.o

Simduino.o: ../lib/Simduino/Simduino.cpp ../lib/*/*.h
	g++ $(CPPFLAGS) ../lib/Simduino/Simduino.cpp -o Simduino.o

TomThumb.o: src/TomThumb.cpp src/*.h
	g++ $(CPPFLAGS) src/TomThumb.cpp -o TomThumb.o

clean:
	rm -f *.o sim-nametag
