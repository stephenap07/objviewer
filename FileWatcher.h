#pragma once

#ifdef __APPLE__
#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <stdio.h>
#include <sys/event.h>
#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#endif

#include <iostream>
#include <list>
#include <memory>
#include <string>
#include <thread>

class Buffer
{
public:
   void add(int value)
   {
      while (true)
      {
         std::unique_lock<std::mutex> locker(mu);
         data.push_back(value);
         locker.unlock();
         return;
      }
   }

   bool hasItem()
   {
      std::unique_lock<std::mutex> locker(mu);
      return !data.empty();
   }

   void removeAll()
   {
      std::unique_lock<std::mutex> locker(mu);
      data.clear();
   }

private:
   std::mutex mu;
   std::condition_variable cond;
   std::list<int> data;
};

class FileWatcher
{
public:
   FileWatcher(const std::string& apath)
      : quit(false)
      , path(apath)
   {
   }

   void run()
   {
#ifdef __APPLE__
      if ((kq = kqueue()) < 0)
      {
         std::cerr << "Could not open kernel queue. Error was " << strerror(errno)
                   << '\n';
      }

      eventFd = open(path.c_str(), O_EVTONLY);
      if (eventFd <= 0)
      {
         std::cerr << "The file " << path
                   << " could not be opened for monitoring. Error was "
                   << strerror(errno) << '\n';
         exit(-1);
      }

      userData = const_cast<char*>(path.c_str());

      timeout.tv_sec = 0;
      timeout.tv_nsec = 500000000;

      vnodeEvents = NOTE_DELETE | NOTE_WRITE | NOTE_EXTEND | NOTE_ATTRIB | NOTE_LINK | NOTE_RENAME | NOTE_REVOKE;
      EV_SET(&eventsToMonitor[0], eventFd, EVFILT_VNODE, EV_ADD | EV_CLEAR,
         vnodeEvents, 0, userData);

      numFiles = 1;

      while (!quit)
      {
         int eventCount{ kevent(kq, eventsToMonitor, 1, eventData, numFiles, &timeout) };

         if ((eventCount < 0) || (eventData[0].flags == EV_ERROR))
         {
            std::cerr << "An error occurred (event count " << eventCount
                      << "). The error was " << strerror(errno) << '\n';
            break;
         }

         if (eventCount != 0)
         {
            if (eventData[0].fflags & NOTE_DELETE)
            {
               //std::cout << "File deleted\n";
               break;
            }

            if (eventData[0].fflags & NOTE_EXTEND || eventData[0].fflags & NOTE_WRITE)
            {
               //std::cout << "File modified\n";
               data.add(1);
            }

            if (eventData[0].fflags & NOTE_ATTRIB)
            {
               //std::cout << "File attributes modified\n";
            }
         }

         timeout.tv_sec = 0;
         timeout.tv_nsec = 500000000;
      }

      close(eventFd);
#endif
   }

   Buffer& buffer() { return data; }

   void doQuit()
   {
      std::unique_lock<std::mutex> locker(mu);
      quit = true;
   }

private:
   std::mutex mu;

   bool quit;
   std::string path;

   int kq;
   int eventFd;
   int numFiles;
   unsigned int vnodeEvents;
   void* userData;
   struct timespec timeout;
#ifdef __APPLE__
   struct kevent eventsToMonitor[1];
   struct kevent eventData[1];
#endif
   Buffer data;
};
