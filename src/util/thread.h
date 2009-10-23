/*
 * boblight
 * Copyright (C) Bob  2009 
 * 
 * boblight is free software: you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 * 
 * boblight is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.
 * See the GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License along
 * with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#ifndef CTHREAD
#define CTHREAD

#include <pthread.h>
#include <stdint.h>
#include <unistd.h>
#include "condition.h"

class CThread
{
  public:
    CThread();
    ~CThread();
    void StartThread();
    void StopThread();
    void AsyncStopThread();
    void JoinThread();

  protected:
    pthread_t     m_thread;
    volatile bool m_stop;

    static void* ThreadFunction(void* args);
    void         Thread();
    virtual void Process();
};

#endif //CTHREAD

