#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <cstring>
#include <ctime>
#include <chrono>

#include "google/protobuf/message.h"
#include "cpp_pbtest.pb.h"

using namespace std;

int main()
{
    lab::NetPacket packet;
    packet.set_ack(1);

    packet.set_timestamp(time(0));

    packet.set_payload("packet content", sizeof("packet content"));

    uint8_t buffer[256];
    packet.SerializeToArray(buffer, packet.ByteSizeLong());

    lab::NetPacket packet_recv;
    packet_recv.ParseFromArray(buffer, sizeof(buffer));

    cout << packet_recv.ack() << " " << packet_recv.timestamp() << " " << packet_recv.payload() << endl;

    return 0;
}