#include "MicroBit.h"
#include "base64.hpp"
#include "pfp/protocol.hpp"
#include "pfp/log.hpp"
#include "pfp/crypto.hpp"
#include "pfp/test_data.hpp"

MicroBit uBit;
MicroBitSerial serial(USBTX, USBRX);
Logger logger(uBit.messageBus);

int8_t recv_buffer[PROTOCOL_MAX_PACKET_SIZE];

void on_recv(MicroBitEvent) {
  logger.enter_span("RADIO RECV");
  
  PacketBuffer p = uBit.radio.datagram.recv();
  Packet packet;
  logger.info("Received packet");
  if (Packet::deserialize(p.getBytes(), packet) < 0) {
    logger.warn("Failed to deserialize packet");
    logger.debug(bthex(p.getBytes(), p.length()).toCharArray());
    logger.debug(bttext(p.getBytes(), p.length()).toCharArray());
  } else {
    logger.info("Received packet");
    logger.debug(ManagedString(packet.command_id).toCharArray());
  }
}

void on_log(MicroBitEvent) {
  ManagedString log_msg(logger.buffer);
  serial.send(log_msg);
}

int main() {
  // Initialise the micro:bit runtime.
  uBit.init();
  uBit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, on_recv, MESSAGE_BUS_LISTENER_REENTRANT);
  uBit.messageBus.listen(PFP_ID_EVT_LOG, PFP_LOG_EVT_SEND, on_log, MESSAGE_BUS_LISTENER_IMMEDIATE);
  uBit.seedRandom();
  uBit.radio.enable();

  uBit.display.scroll("INIT");
  ManagedString device_id = uBit.getSerial();

  int x = 0;

  while (true) {
    logger.enter_span("MAIN LOOP");
    if (x == 0) {
      uBit.radio.datagram.send(ManagedString("Hello World!"));
      logger.info("Sent Hello World!");
      x = 1;
    } else {
      uBit.radio.datagram.send((uint8_t*)DISCOVER_PACKET, PROTOCOL_MAX_PACKET_SIZE);
      logger.info("Sent HELOP!");
      x = 0;
    }

    uBit.sleep(5000);
  }

  // If main exits, there may still be other fibers running or registered event handlers etc.
  // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
  // sit in the idle task forever, in a power efficient sleep.
  release_fiber();
}
