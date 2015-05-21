#include <boost/bind.hpp>
#include <boost/lexical_cast.hpp>
#include <string>
#include "MainServer.hpp"
#include <boost/regex.hpp>
#include <syslog.h>

void MainServer::server_start_receive() {
  mServerSocket.async_receive_from( boost::asio::buffer(mRecvBuffer),
                                     mRemoteClient,
                                     boost::bind(&MainServer::handle_receive_from_client, this,
                                                 boost::asio::placeholders::error,
                                                 boost::asio::placeholders::bytes_transferred
                                                )
                                   );
}

void MainServer::handle_receive_from_client(const boost::system::error_code& error,std::size_t insize/*bytes_transferred*/) {
  if (!error || error == boost::asio::error::message_size) {
     u_int32_t clientip=mRemoteClient.address().to_v4().to_ulong(); 
     std::string queryname=queryString(insize);
     if (insize > 1) {
       mServerSocket.async_send_to(boost::asio::buffer(mResponseHelper.reply(mRecvBuffer,insize,true),
                                                       insize+16),
                                   mRemoteClient,
                                   boost::bind(&MainServer::handle_send,
                                               this,
                                               boost::asio::placeholders::error,
                                               boost::asio::placeholders::bytes_transferred
                                              )
                                   );
     } else {
        syslog(LOG_WARNING, "Tiny packet, unable to get uniqueu id, ignoring.");
     }
  }
  server_start_receive();
}

std::string MainServer::queryString(std::size_t packetsize){
  //The DNS header is 12 bytes long before we get to the question section.
  //A one byte question takes up two bytes. 
  //If the whole packet is to small to contain a matching question we return an empty string.
  if (packetsize < 14) {
     return "";
  }
  std::string rval="";
  for (size_t index=12;(index<(packetsize-2)) && (mRecvBuffer[index] != 0 ) ;index=index+mRecvBuffer[index]+1) {
     size_t tokenlen=mRecvBuffer[index] < packetsize? mRecvBuffer[index] : packetsize;
     if (tokenlen > 0) {
       if (index !=12) {
         rval = rval +  ".";
       }
       for (size_t index2=index+1;index2< index+tokenlen+1;index2++) {
          rval.push_back((char) mRecvBuffer[index2]);
       }
     }
  } 
  return rval;
}

MainServer::MainServer(boost::asio::io_service& io_service,
                       boost::asio::ip::udp::socket &serversocket, 
                       DnsResponse &responsehelper):mIoService(io_service),
                                                    mServerSocket(serversocket),
                                                    mResponseHelper(responsehelper),
                                                    mCommandRegex("^ws([1-9][0-9]{0,7})-gw([1-9][0-9]{0,4})\\.magicdomain\\.internal$"), 
                                                    mCommand2Regex("^ws([1-9][0-9]{0,7})-clear\\.magicdomain\\.internal$"),
					            mMagicDomainRegex("(.*\\.)?magicdomain\\.internal$")
{
  openlog("dnsitsme",LOG_PID,LOG_USER);
  server_start_receive();
}

MainServer::~MainServer() {
  closelog();
}
