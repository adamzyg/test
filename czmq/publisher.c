#include <czmq.h>

char text[] = "{\"timestamp\":\"2019-03-03 08:45:57\", \"value\": 1}";
char *text1 = NULL;
int main(int argc, char ** argv) {
  zsock_t *socket = zsock_new_pub("ipc://example.sock");
  assert(socket);

  while(!zsys_interrupted) {
    zsys_info("Publishing");
    zsock_send(socket, "sss", "TOPIC", text, text);
    zclock_sleep(1000);
  }

  zsock_destroy(&socket);
  return 0;
}
