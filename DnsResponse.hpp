#ifndef _PBDNS_DNS_RESPONSE_HPP
#define _PBDNS_DNS_RESPONSE_HPP
#include <boost/array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
//Helper class for generating responses for the command processor.
class DnsResponse {
     boost::array<char,4> mTrueIp;   //Response IP to use as truth value
      boost::array<char,4> mFalseIp; //response IP to use as false value
   public: 
     DnsResponse(std::string trueip, std::string falseip);
     boost::array<char,65536 > reply(boost::array<char,65536 > &query ,std::size_t qsize,bool val);//Construct a response from a dns query.
};
#endif
