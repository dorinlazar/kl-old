#include "kl/klnet.h"
using namespace kl;

int main() {
  try {
    FileStream output("output.mp3", FileOpenMode::WriteOnly);
    SslClient client("podcastdeistorie.ro"_t, 443);
    std::string req(
        "GET /episoade/Istorie116.mp3 HTTP/1.1\r\nHost: podcastdeistorie.ro:443\r\nConnection: close\r\n\r\n");

    client.write({(uint8_t*)req.c_str(), (uint8_t*)req.c_str() + req.size()});
    StreamReader reader(&client);
    std::array<uint8_t, 1024> buffer;
    std::span<uint8_t> bufspan(buffer.data(), buffer.size());
    while (true) {
      auto line = reader.read_line();
      log("{} -- {}", line[0], line);
      if (line.trim().size() == 0) {
        break;
      }
    }
    while (true) {
      auto size = reader.read(bufspan);
      if (size == 0) {
        break;
      }
      output.write(bufspan.subspan(0, size));
    }
  } catch (IOException& ioe) {
    kl::err("Exception received: ", ioe.what());
  }
  return 0;
}
