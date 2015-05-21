#include <sys/types.h>
#include <pwd.h>
#include <grp.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include "UnPrivileged.hpp"
#include <iostream>

UnPrivileged::UnPrivileged():mDroppedPriv(false),mIsChild(false) {
   //Uncomment the following 3 lines to run in priviledged non daemon mode for debug purposes.
   //mDroppedPriv=true;
   //mIsChild=true;
   //return;
   struct passwd *pbdnsd_user=getpwnam("dnsitsme");
   if (pbdnsd_user) {
      uid_t pbdnsd_user_uid= pbdnsd_user->pw_uid;
      gid_t pbdnsd_user_gid= pbdnsd_user->pw_gid;
      if (setgid(pbdnsd_user_gid) == 0) {
        if (setuid(pbdnsd_user_uid) == 0) {
          mDroppedPriv=true;
          pid_t pid;
          pid = fork();
          if (pid == 0) {
            setsid();
            close(0);
            close(1);
            close(2);
            int f1=open("/dev/zero",O_RDWR);
            int f2=open("/dev/null",O_RDWR);
            int f3=open("/dev/null",O_RDWR);
            //int f2=open("/tmp/pbdns.stdout",O_RDWR|O_CREAT,0700);
            //int f3=open("/tmp/pbdns.stderr",O_RDWR|O_CREAT,0700);
            umask(0000);
            chdir("/");
            mIsChild=true;
          }
        }
      }            
   } else {
      std::cerr << "No 'dnsitsme' user to run under." << std::endl;
   }
}
bool UnPrivileged::operator()() {
  return mDroppedPriv;
}
bool UnPrivileged::is_child(){
  return mIsChild;
}
