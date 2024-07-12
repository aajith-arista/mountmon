// Copyright (c) 2024 Arista Networks, Inc.  All rights reserved.
// Arista Networks, Inc. Confidential and Proprietary.

#include <assert.h>
#include <errno.h>
#include <libmount/libmount.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include <signal.h>
#include <sys/epoll.h>

#define MAX_EVENTS 32
#define FILENAME_SIZE 1024
#define DUMPSIZE (4096 * 4)

// char mountinfo_dump[ DUMPSIZE ];
static volatile bool quit = false;

void sigint_handler( int x ) {
   quit = true;
}

static void epoll_ctl_add(int epfd, int fd, uint32_t events)
{
   struct epoll_event ev;
   ev.events = events;
   ev.data.fd = fd;
   if (epoll_ctl(epfd, EPOLL_CTL_ADD, fd, &ev) == -1) {
      perror("epoll_ctl()\n");
      exit(1);
   }
}
int main( void ) {
   int nfds, mn_fd, epoll_fd,  r;
   struct epoll_event events[MAX_EVENTS];
   struct libmnt_monitor * mn = NULL;
   const char * filename = NULL;
   int event_count = 0;

   printf( "Initing mount monitor\n" );

   mn = mnt_new_monitor();
   assert( mn );
   r = mnt_monitor_enable_kernel(mn, 1);
   if( r < 0 ) {
      perror( "mnt_monitor_enable_kernel" );
   }
   mn_fd = r = mnt_monitor_get_fd( mn );
   if( r < 0 ) {
      perror( "mnt_monitor_get_fd" );
   }
   epoll_fd = epoll_create( 1 );
   epoll_ctl_add(epoll_fd, mn_fd, EPOLLIN | EPOLLOUT | EPOLLET);

   printf( "Registering Ctrl-C handler\n" );
   signal(SIGINT, sigint_handler );

   printf( "Starting monitor\n" );
   while( !quit ) {
      nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
      for ( int i = 0; i < nfds; i++) {
         int mn_type;
         assert( events[i].data.fd == mn_fd );
         event_count++;
         printf( "Received event %d\n", event_count );
         r = mnt_monitor_next_change( mn, &filename, &mn_type);
         if( r < 0 ){
            perror( "mnt_monitor_next_change" );
         } else if( r ) {
            printf( "No change" );
         } else {
            printf( "Change:" );
            if( filename ) {
               printf( " filename: ", filename );
            }
            printf( " type: %d\n", mn_type );
            printf( "Dumping mountinfo\n" );
            system( "cat /proc/self/mountinfo && echo '-----------------  END OF DUMP -----------------' 2>&1 | tee -a /tmp/mountmon.log" );
         }
      }
   }

   printf( "Quitting!\n" );


   return( 0 );
}
