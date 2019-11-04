CC = g++
CCA = arm-linux-gnueabihf-g++
OPTIONS = -Wall -Wextra
LIB = -ldbus-1 -lpthread -lrt
INCLUDE = -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -L/usr/lib/x86_64-linux-gnu
INCLUDEC = -I/home/dbus-arm -I/home/dbus-arm/dbus-1.0 -L/home/dbus-arm/lib

all : clean test-mydbus myclient 

test-mydbus:
		$(CCA) -o test-mydbus.bin test-mydbus.cpp $(INCLUDEC) $(LIB) $(OPTIONS)

#clientcli:
#       $(CCA) -o clientcli.bin myclient.cpp $(INCLUDEC) $(LIB) $(OPTIONS)

myclient:
		$(CCA) -o myclient.bin myclient.cpp mydbus.cpp $(INCLUDEC) $(LIB) $(OPTIONS)
clean:
		rm -f test-mydbus myclient