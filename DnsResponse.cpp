#include <boost/array.hpp>
#include <boost/tokenizer.hpp>
#include <boost/lexical_cast.hpp>
#include <boost/regex.hpp>
#include "DnsResponse.hpp" 
DnsResponse::DnsResponse(std::string trueip,std::string falseip){
        //Convert the two ascii IP numbers into arrays we can use when generating responses.
        //FIXME: fix this code to use boost::asio IP address parsing.        
        boost::regex ipregex("^((\\d|\\d\\d|[0-1]\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|\\d\\d|[0-1]\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|\\d\\d|[0-1]\\d\\d|2[0-4]\\d|25[0-5])\\.(\\d|\\d\\d|[0-1]\\d\\d|2[0-4]\\d|25[0-5]))$");
        if (boost::regex_match(trueip,ipregex) == false) {
           throw std::runtime_error(std::string("invalid trueip '") + trueip + "'");
        }
        if (boost::regex_match(falseip,ipregex) == false) {
           throw std::runtime_error(std::string("invalid falseip '") + falseip + "'");
        }
        typedef boost::tokenizer<boost::char_separator<char> > tokenizer;
        boost::char_separator<char> sep(".");
        tokenizer tok(trueip, sep);
        size_t index=0;
        for (tokenizer::iterator it = tok.begin(); it != tok.end(); ++it) {
            mTrueIp[index]=static_cast<char>(boost::lexical_cast<unsigned int>(*it));
            index++;
        }
        tokenizer tok2(falseip, sep);
        index=0;
        for (tokenizer::iterator it = tok2.begin(); it != tok2.end(); ++it) {
            mFalseIp[index]=static_cast<char>(boost::lexical_cast<unsigned int>(*it));
            index++;
        }
}

//This method adds a 14 byte response section to the DNS packet and updates the dns header to
//indicate this.
boost::array<char,65536 > DnsResponse::reply(boost::array<char,65536 > &query ,std::size_t qsize,bool val) {
        boost::array<char,65536 > rval=query;
        //(See RFC1035 section 4.1.1. Header section format)
        //Set the R bit to true to indicate this is no longer a query but a response.
        rval[2] |= 0x080;
        //Clear the RCODE (should not be needed, just to be sure)
        rval[3] &= 0x0f0;
        //Set the answer count to 1 so we can add our own RR record as a response.
        rval[6] = 0;
        rval[7] = 1;
        //Now we start our new answer section RR record (4.1.3. Resource record format)
        //Pointer to name already in the query section
        rval[qsize]=0x0c0;  //indicate that this is a pointer. (4.1.4. Message compression)
        rval[qsize+1]=0x0c; //Point to position 12, just after the dns header the question section should start.
        //Type = A (3.2.2. TYPE values)
        rval[qsize+2]=0;
        rval[qsize+3]=1;
        //Class = IN (3.2.4. CLASS values)
        rval[qsize+4]=0;
        rval[qsize+5]=1;
        //Ttl = 0 (4.1.3. Resource record format)
        rval[qsize+6]=0;
        rval[qsize+7]=0;
        rval[qsize+8]=0;
        rval[qsize+9]=0;
        //Rdlen = (4 3.4.1. A RDATA format)
        rval[qsize+10]=0;
        rval[qsize+11]=4;
        //Fake ip (3.4.1. A RDATA format)
        if (val) {
          rval[qsize+12]=mTrueIp[0];
          rval[qsize+13]=mTrueIp[1];
          rval[qsize+14]=mTrueIp[2];
          rval[qsize+15]=mTrueIp[3];
        } else {
          rval[qsize+12]=mFalseIp[0];
          rval[qsize+13]=mFalseIp[1];
          rval[qsize+14]=mFalseIp[2];
          rval[qsize+15]=mFalseIp[3];
        }
        return rval;
} 
