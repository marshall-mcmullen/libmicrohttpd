/*
  This file is part of libmicrohttpd
  Copyright (C) 2016 Karlson2k (Evgeny Grin)

  This library is free software; you can redistribute it and/or
  modify it under the terms of the GNU Lesser General Public
  License as published by the Free Software Foundation; either
  version 2.1 of the License, or (at your option) any later version.

  This library is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
  Lesser General Public License for more details.

  You should have received a copy of the GNU Lesser General Public
  License along with this library; if not, write to the Free Software
  Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA

*/

/**
 * @file microhttpd/mhd_itc.c
 * @brief  Implementation of inter-thread communication functions
 * @author Karlson2k (Evgeny Grin)
 * @author Christian Grothoff
 */

#include "mhd_itc.h"
#ifdef HAVE_UNISTD_H
#include <unistd.h>
#endif /* HAVE_UNISTD_H */
#include <fcntl.h>
#include "internal.h"


#ifdef HAVE_SYS_EVENTFD_H

int
MHD_pipe_write_ (struct MHD_Pipe pip,
                 const void *ptr,
                 size_t sz)
{
  uint64_t val = 1;
  if (sizeof (val) !=
      write (pip.event_fd,
             &val,
             sizeof (val)))
    MHD_PANIC (_("Failed to write to eventfd\n"));
  return sz;
}

#else

#if defined(_MHD_ITC_EVENTFD) || defined(_MHD_ITC_PIPE)
#if !defined(_WIN32) || defined(__CYGWIN__)


/**
 * Change itc FD options to be non-blocking.
 *
 * @param pip the pipe to manipulate
 * @return non-zero if succeeded, zero otherwise
 */
int
MHD_itc_nonblocking_ (struct MHD_Pipe pip)
{
  unsigned int i;

  for (i=0;i<2;i++)
  {
    int flags;

    flags = fcntl (pip.fd[i],
                   F_GETFL);
    if (-1 == flags)
      return 0;

    if ( ((flags | O_NONBLOCK) != flags) &&
         (0 != fcntl (pip.fd[i],
                      F_SETFL,
                      flags | O_NONBLOCK)) )
      return 0;
  }
  return !0;
}
#endif /* !_WIN32 || __CYGWIN__ */
#endif /* _MHD_ITC_EVENTFD ||  _MHD_ITC_PIPE */
#endif /* ! HAVE_SYS_EVENTFD_H */
