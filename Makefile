CC = g++
CCA = arm-linux-gnueabihf-g++
OPTIONS = -Wall -Wextra
LIB = -ldbus-1 -lpthread -lrt
INCLUDE = -I/usr/include/dbus-1.0 -I/usr/lib/x86_64-linux-gnu/dbus-1.0/include -L/usr/lib/x86_64-linux-gnu
INCLUDEC = -I/home/dbus-arm -I/home/dbus-arm/dbus-1.0 -L/home/dbus-arm/lib

all : clean myclient

#dbuscli:
#       $(CCA) -o dbuscli.bin mydbus.cpp $(INCLUDEC) $(LIB) $(OPTIONS)

#clientcli:
#       $(CCA) -o clientcli.bin myclient.cpp $(INCLUDEC) $(LIB) $(OPTIONS)

myclient:
		$(CCA) -o myclient.bin myclient.cpp mydbus.cpp $(INCLUDEC) $(LIB) $(OPTIONS)
clean:
		rm -f myclient