#ifndef _PBDNS_UN_PRIVILEGED_HPP_
#define _PBDNS_UN_PRIVILEGED_HPP_

class UnPrivileged {
    bool mDroppedPriv;
    bool mIsChild;
  public:
    UnPrivileged();
    bool operator()();
    bool is_child();
};

#endif
