
#include "peer_router.h"
#include <iostream>
#include <sodium.h>

using namespace std;
using namespace zmqpp;

unsigned char key[32] = "Esta es mi llave";
uint64_t nonce = 0;

void putEncryptedMessage(message& m, const string& plain) {
  const char* text = plain.c_str();
  uint64_t mynonce = nonce++;
  unsigned char enc[plain.size()];
  crypto_stream_salsa20_xor(enc, (unsigned char*)text, plain.size(), (unsigned char*)&mynonce, key);
  string encrypted((const char*)enc, plain.size());
  m << mynonce << encrypted;
}

string getDecryptedMessage(message &m) {
  uint64_t mynonce;
  string encrypted;
  m >> mynonce >> encrypted;
  const char* enc = encrypted.c_str();
  unsigned char plain[encrypted.size()];
  crypto_stream_salsa20_xor(plain, (unsigned char*)enc, encrypted.size(), (unsigned char*)&mynonce, key);
  return string((const char*)plain, encrypted.size());
}

class ChatClient : public Listener {
  string myname;
  public:
    void recv_message(zmqpp::message& msg, zmqpp::socket& skt) {
      string sender, text;
      //msg >> sender >> text;
      sender = getDecryptedMessage(msg);
      text = getDecryptedMessage(msg);
      cout << sender << ": " << text << endl;
    }

    void other_input(int fd, zmqpp::socket& skt) {
      string text;
      if (fd == 0) {
        //read from stdin
        getline(cin, text);
        message msg;
        if (text == "exit") {
          msg << static_cast<int>(PRCommand::Unregister);
          skt.send(msg);
          finish();
        } else {
          //msg << myname << text;
          putEncryptedMessage(msg, myname);
          putEncryptedMessage(msg, text);
          cout << "Sending: " << text << endl;
          broadcast_peer(skt,msg);
        }
      }
    }

    void setName(const string& s) {
      myname = s;
    }

    string getName() const {
      return myname;
    }
};

int main() {
  if (sodium_init() == -1) {
    return 1;
  }
  ChatClient chat;
  string myname;
  cout << "Write your name here: ";
  getline(cin, myname);
  chat.setName(myname);
  vector<int> filedesc {0};
  start_peer("tcp://localhost:3030",filedesc,chat);
}
