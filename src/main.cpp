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
  
  PacketBuffer p = state.ubit.radio.datagram.recv();
  Packet packet;
  DeserializeResult result = Packet::deserialize(p.getBytes(), p.length(), packet);

  if (result.enumCase() != DeserializeCode::Success) {
    result.doSwitch().ifCase<DeserializeCode::NotEnoughData>([&log](int bytes) {
      log.warn("Not enough data (%d bytes received)", bytes);
    }).ifCase<DeserializeCode::InvalidCommand>([&log](uint8_t command) {
      log.warn("Invalid command (command id: %d)", command);
    });

    log.debug(bthex(p.getBytes(), p.length()).toCharArray());
    log.debug(bttext(p.getBytes(), p.length()).toCharArray());
  } else {  
    log.info("Received packet with command id %d", packet.command_id);
    log.debug(bthex(p.getBytes(), p.length()).toCharArray());
  }
}

int main() {
  // Register the listeners
  state.ubit.messageBus.listen(MICROBIT_ID_RADIO, MICROBIT_RADIO_EVT_DATAGRAM, on_recv, MESSAGE_BUS_LISTENER_QUEUE_IF_BUSY);
  // Initialise the micro:bit runtime.
  state.init();

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
  }

  // If main exits, there may still be other fibers running or registered event handlers etc.
  // Simply release this fiber, which will mean we enter the scheduler. Worse case, we then
  // sit in the idle task forever, in a power efficient sleep.
  release_fiber();
}
