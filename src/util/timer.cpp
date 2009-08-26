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

#include "timer.h"
#include "misc.h"

#include <iostream>
using namespace std;

CTimer::CTimer()
{
  m_interval = -1;
}

void CTimer::SetInterval(int64_t usecs)
{
  m_interval = usecs;
  Reset();
}

int64_t CTimer::GetInterval()
{
  return m_interval;
}

void CTimer::Reset()
{
  m_time = m_clock.GetTime();
}

void CTimer::Wait()
{
  int64_t sleeptime;

  //keep looping until we have a timestamp in the future we can wait for
  do
  {
    m_time += m_interval * m_clock.GetFreq() / 1000000;
    sleeptime = (m_time - m_clock.GetTime()) * 1000000 / m_clock.GetFreq();
  }
  while(sleeptime <= 0);
  
  if (sleeptime > m_interval * 2) //failsafe, m_time must be bork if we get here
  {
    sleeptime = m_interval * 2;
    Reset();
  }
  
  usleep(sleeptime);
}
