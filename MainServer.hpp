#ifndef _PBDNS_MAIN_SERVER_HPP
#define _PBDNS_MAIN_SERVER_HPP
#include <boost/asio.hpp>
#include <boost/regex.hpp>
#include "DnsResponse.hpp"
class MainServer {
    boost::asio::io_service& mIoService;
    boost::asio::ip::udp::socket &mServerSocket;
    boost::array<char,65536 >    mRecvBuffer;
    boost::asio::ip::udp::endpoint mRemoteClient;
    std::string queryString(size_t psize);
    std::string dnsid(size_t psize);
    DnsResponse &mResponseHelper;
    boost::regex mCommandRegex;
    boost::regex mCommand2Regex;
    boost::regex mMagicDomainRegex;
    void server_start_receive();
    void handle_receive_from_client(const boost::system::error_code& error,std::size_t insize);   
    void handle_send(const boost::system::error_code& err,std::size_t s){}
  public:
    MainServer(boost::asio::io_service& io_service,boost::asio::ip::udp::socket &serversocket, DnsResponse &responsehelper); 
    ~MainServer();
};
#endif
