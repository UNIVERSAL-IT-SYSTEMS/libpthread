/* Try to acquire a rwlock for reading.  Generic version.
   Copyright (C) 2002 Free Software Foundation, Inc.
   This file is part of the GNU C Library.

   The GNU C Library is free software; you can redistribute it and/or
   modify it under the terms of the GNU Library General Public License as
   published by the Free Software Foundation; either version 2 of the
   License, or (at your option) any later version.

   The GNU C Library is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
   Library General Public License for more details.

   You should have received a copy of the GNU Library General Public
   License along with the GNU C Library; see the file COPYING.LIB.  If not,
   write to the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
   Boston, MA 02111-1307, USA.  */

#include <pthread.h>
#include <assert.h>

#include <pt-internal.h>

/* Try to acquire RWLOCK.  */
int
pthread_rwlock_tryrdlock (struct __pthread_rwlock *rwlock)
{
  __pthread_spin_lock (&rwlock->__lock);
  if (__pthread_spin_trylock (&rwlock->__held) == 0)
    /* Successfully acquired the lock.  */
    {
      assert (rwlock->readerqueue == 0);
      assert (rwlock->writerqueue == 0);
      assert (rwlock->readers == 0);

      rwlock->readers = 1;
      __pthread_spin_unlock (&rwlock->__lock);
      return 0;
    }
  else
    /* Lock is held, but is held by a reader?  */
    if (rwlock->readers > 0)
      {
	assert (rwlock->readerqueue == 0);
	rwlock->readers ++;
	__pthread_spin_unlock (&rwlock->__lock);
	return 0;
      }

  /* The lock is busy.  */

  __pthread_spin_unlock (&rwlock->__lock);

  return EBUSY;
}
