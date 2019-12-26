#include <stdio.h>
#include <iostream>
#include <ctype.h>
#include <stddef.h>
#include <stdint.h>
#include <string>
#include <stdlib.h>

using namespace std;

#pragma pack(4)
struct Packet {
	enum PacketTypeEnum { PACKET_A, PACKET_B, PACKET_C };
	
	uint8_t packetType;

	union Body {
		struct PacketA {
			int x, y, z;
		} a;
		
		struct PacketB {
			int numElements;
			int elements[100];
		} b;

		struct PacketC {
			bool x;
			short y;
			int z;
		} c;

	};
};
#pragma pack()

int main() {
	Packet p;
	std::cout << sizeof(p) << std::endl;
}
