
#include <iostream>
#include <memory>
#include <string>

#include <grpc++/grpc++.h>
#include "echo.grpc.pb.h"

using grpc::Channel;
using grpc::ClientContext;
using grpc::Status;

using echo::SendMsg;
using echo::RecieveMsg;
using echo::Message;


class EchoClient {
 public:
  EchoClient(std::shared_ptr<Channel> channel)
      : stub_(Message::NewStub(channel)) {}

  // Assembles the client's payload, sends it and presents the response back
  // from the server.
  std::string SendMessage(const std::string& user) {
    // Data we are sending to the server.
    SendMsg request;
    request.set_message(user);

    // Container for the data we expect from the server.
    RecieveMsg reply;

    // Context for the client. It could be used to convey extra information to
    // the server and/or tweak certain RPC behaviors.
    ClientContext context;

    // The actual RPC.
    Status status = stub_->SendMessage(&context, request, &reply);

    // Act upon its status.
    if (status.ok()) {
      return reply.message();
    } else {
      std::cout << status.error_code() << ": " << status.error_message()
                << std::endl;
      return "RPC failed";
    }
  }

 private:
  std::unique_ptr<Message::Stub> stub_;
};

int main(int argc, char** argv) {
  // Instantiate the client. It requires a channel, out of which the actual RPCs
  // are created. This channel models a connection to an endpoint (in this case,
  // localhost at port 50051). We indicate that the channel isn't authenticated
  // (use of InsecureChannelCredentials()).
  EchoClient echo(grpc::CreateChannel(
      "localhost:50051", grpc::InsecureChannelCredentials()));
  std::string user("Hello!");
  std::string reply = echo.SendMessage(user);
  std::cout << "Message from Server: " << reply << std::endl;

  return 0;
}
