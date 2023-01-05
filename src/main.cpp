#include "MicroBit.h"
#include "base64.hpp"
#include "pfp/protocol.hpp"
#include "pfp/log.hpp"
#include "pfp/crypto.hpp"
#include "pfp/test_data.hpp"

MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);

void on_log();
Logger logger(on_log);
void on_log()  {
  ManagedString log_msg(logger.buffer);
  serial.send(log_msg);
}

int8_t recv_buffer[PROTOCOL_MAX_PACKET_SIZE];

void on_recv(MicroBitEvent) {
  Logger log = logger.enter_span("RADIO RECV");
  
  PacketBuffer p = uBit.radio.datagram.recv();
  Packet packet;
  if (Packet::deserialize(p.getBytes(), p.length(), packet) < 0) {
    log.warn("Failed to deserialize packet");
    log.debug(bthex(p.getBytes(), p.length()).toCharArray());
    log.debug(bttext(p.getBytes(), p.length()).toCharArray());
  } else {  
    log.info("Received packet");
    log.debug(bthex(p.getBytes(), p.length()).toCharArray());
    log.debug(ManagedString(packet.command_id).toCharArray());
  }
}

int main() {
  // Initialise the micro:bit runtime.
  uBit.init();
  uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, on_recv, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
  uBit.seedRandom();
  uBit.radio.enable();

  ManagedString device_id = uBit.getSerial();

  while (true) {
    uBit.radio.datagram.send((uint8_t*)DISCOVER_PACKET, 13);
    uBit.sleep(1000);
  }

  // If main exits, there may still be other fibers running or registered event handlers etc.
  // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
  // sit in the idle task forever, in a power efficient sleep.
  release_fiber();
}
