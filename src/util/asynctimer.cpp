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

#include "asynctimer.h"
#include "misc.h"

CAsyncTimer::~CAsyncTimer()
{
  StopTimer();
}

void CAsyncTimer::StartTimer(int64_t usecs /*= -1*/)
{
  if (usecs > 0) 
    m_interval = usecs;
  
  StartThread();
}

void CAsyncTimer::Process()
{
  int64_t target = m_clock.GetTime();
  int64_t now;
  int64_t sleeptime;

  while(!m_stop)
  {
    target += m_interval * m_clock.GetFreq() / 1000000;

    now = m_clock.GetTime();
    sleeptime = (target - now) * 1000000 / m_clock.GetFreq();
    if (sleeptime > m_interval * 2) //failsafe
    {
      sleeptime = m_interval * 2;
      target = now;
    }
    if (sleeptime > 0)
      usleep(sleeptime);

    m_signal.Broadcast();
  }
}

void CAsyncTimer::Wait()
{
  m_signal.Lock();
  m_signal.Wait(m_interval * 2);
  m_signal.Unlock();
}

void CAsyncTimer::StopTimer()
{
  StopThread();
}
