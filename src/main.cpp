#include "MicroBit.h"
#include "base64.hpp"
#include "pfp/protocol.hpp"
#include "pfp/log.hpp"
#include "pfp/crypto.hpp"
#include "pfp/test_data.hpp"
#include "pfp/state.hpp"

State state;

void on_log(char * msg);
Logger logger(on_log);
void on_log(char * msg)  {
  ManagedString log_msg(msg);
  (*state.serial).send(log_msg);
}

void on_recv(MicroBitEvent) {
  Logger log = logger.enter_span("RADIO RECV");
  
  log.info("Received packet");
  PacketBuffer p = state.ubit.radio.datagram.recv();
  Packet packet;
  uint8_t *bytes = p.getBytes();
  int len = p.length();
  DeserializeResult result = Packet::deserialize(bytes, len, packet);

  if (result.enumCase() != DeserializeCode::Success) {
    result.doSwitch().ifCase<DeserializeCode::NotEnoughData>([&log](int bytes) {
      log.warn("Not enough data (%d bytes received)", bytes);
    }).ifCase<DeserializeCode::InvalidCommand>([&log](uint8_t command) {
      log.warn("Invalid command (command id: %d)", command);
    });
  } else {  
    log.info("Received packet with command id %d", packet.command_id);

    switch (packet.command_id) {
    case Command::HELOP:
      log.debug("Received HELOP");
      state.handle_helop(packet);
      break;
    case Command::OLEHL:
      log.debug("Received OLEHL");
      if (packet.dest_addr == state.current_device->get_id()) {
        state.handle_olehl(packet);
      }
      break;
    case Command::ALIVE:
      log.debug("Received ALIVE");
      state.handle_alive(packet);
      break;
    case Command::IDENT:
      log.debug("Received IDENT");
      state.handle_ident(packet);
      // TODO: Implement giving logical id if manager
      break;
    case Command::TNEDI:
      log.debug("Received TNEDI");
      // TODO: Implement received logical id
      break;
    case Command::PUSH:
      log.debug("Received PUSH");
      // TODO: Handle push data
      break;
    case Command::ADD:
      log.debug("Receive ADD");
      // TODO: New device added to the mesh
      break;
    case Command::DEL:
      log.debug("Received DEL");
      // TODO: Device lost on mesh
      break;
    default:
      if (packet.dest_addr != 0 && packet.forwarded_by_addr == state.current_device->get_id()) {
        state.forward(packet);
      }
      break;
    }
  }
}

int main() {
  // Register the listeners
  state.ubit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, on_recv, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
  // Initialise the micro:bit runtime.
  state.init(&logger);

  while (true) {
    Logger log = logger.enter_span("MAIN LOOP");

    if (state.current_device->get_device_state() == DeviceState::Alone) {
      state.discover_network();
      log.debug("Discovering network...");
    } else {
      state.ubit.display.scroll("UwU");
      log.debug("I'm not alone! :D");
    }

    state.ubit.sleep(1000);

    state.check_ttl();
  }

  // If main exits, there may still be other fibers running or registered event handlers etc.
  // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
  // sit in the idle task forever, in a power efficient sleep.
  release_fiber();
}
