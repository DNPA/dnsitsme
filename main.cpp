#include <boost/asio.hpp>
#include <iostream>
#include <boost/lexical_cast.hpp>
#include "MainServer.hpp"
#include "UnPrivileged.hpp"
#include "DnsResponse.hpp"

int core_loop_unpriv(boost::asio::io_service &io_service,boost::asio::ip::udp::socket &serversocket,std::string yesip) {
  UnPrivileged unpriv; //This drops privilegdes, forks and daemonizes.
  if (unpriv()) { //Dropping priviledges succeeded. Should always succees unless we are not root to start with.
    if (unpriv.is_child()) {
        //We are the daemonized unpriviledged process now.
        DnsResponse responsehelper(yesip,"127.0.0.1"); //Create a helper for building responses to command queries.
        MainServer server(io_service,serversocket,responsehelper); //Create and register our main server.
        try {
            io_service.run(); //Run the asynchonous server by starting the reactor.
        } catch (std::exception& e) {
          std::cerr << "ERROR: unexpected exception: " << e.what() << std::endl;
          return 1;
        } catch (...) {
          std::cerr << "ERROR: unexpected non-exception: "<< std::endl;
          return 1;
        }
        return 0;
    } else {
      //We are the non daemon parent process.
      std::cout << "Forked unpriviledged server daemon process." << std::endl;
      return 0;
    }
  } else {
    std::cerr << "ERROR: Problem dropping priviledges" << std::endl;
    return 1;
  }
  return 0;
}

int main (int argc, char ** argv) {
  if (argc < 2) {
    std::cerr << "Error: Need to know what IP to run on." << std::endl;
    return 1;
  }
  try {
    //The first argument is our listen IP. We could have gotten that out of the config ourself, 
    //but we need it before dropping priviledges so we don't.
    std::string listenip=argv[1];
    //Create io_service, we need this when creating the socket for the priviledged low (53) udp port.
    boost::asio::io_service io_service;
    //Create our priviledged port 53 socket before dropping priviledges.
    boost::asio::ip::udp::endpoint ep(boost::asio::ip::address_v4::from_string(listenip.c_str()),53);
    boost::asio::ip::udp::socket serversocket(io_service, ep);
    //Run the main loop unpriviledged or croak.
    return core_loop_unpriv(io_service,serversocket,listenip);
  } catch (std::exception& e) {
     std::cerr << "ERROR: unexpected exception: " << e.what() << std::endl;
     return 1;
  }
}
