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

#include <math.h>

#include "device.h"
#include "util/log.h"
#include "util/sleep.h"

CChannel::CChannel()
{
  m_color = -1;
  m_light = -1;

  m_isused = false;
  
  m_speed = 100.0;
  m_wantedvalue = 0.0;
  m_currentvalue = 0.0;
  m_fallback = 0.0;
  m_lastupdate = -1;

  m_gamma = 1.0;
  m_adjust = 1.0;
  m_blacklevel = 0.0;
}

float CChannel::GetValue(int64_t time)
{
  //we need two calls for the speed
  if (m_lastupdate == -1)
  {
    m_lastupdate = time;
    return m_currentvalue;
  }

  if (m_speed == 100.0) //speed of 100.0 means max
  {
    m_currentvalue = m_wantedvalue;
  }
  else
  {
    float diff = m_wantedvalue - m_currentvalue; //difference between where we want to be, and where we are
    float timediff = time - m_lastupdate; //difference in time in microseconds between now and the last update
    float speedadjust = 1.0 - pow(1.0 - (m_speed / 100.0), timediff / 20000.0); //speed adjustment value, corrected for time

    //move the current value closer to the wanted value
    m_currentvalue += diff * speedadjust;
  }

  m_lastupdate = time;

  float outputvalue = m_currentvalue;
  //gamma correction
  if (m_gamma != 1.0)
    outputvalue = pow(outputvalue, m_gamma);
  //adjust correction
  if (m_adjust != 1.0)
    outputvalue *= m_adjust;
  //blacklevel correction
  if (m_blacklevel != 1.0)
    outputvalue = (outputvalue * (1.0 - m_blacklevel)) + m_blacklevel;
  
  return outputvalue;
}

CDevice::CDevice(CClientsHandler& clients) : m_clients(clients)
{
  m_type = NOTHING;
}

void CDevice::Process()
{
  log("Starting device %s on \"%s\"", m_name.c_str(), m_output.c_str());
      
  while(!m_stop)
  {
    //keep trying to set up the device every 10 seconds
    while(!m_stop)
    {
      log("Setting up device %s", m_name.c_str());
      if (!SetupDevice())
      {
        CloseDevice();
	log("Setup failed, retrying in 10 seconds");
        USleep(10000000LL, &m_stop);
      }
      else
      {
	log("Device %s opened", m_name.c_str());
        break;
      }
    }


    //keep calling writeoutput until we're asked to stop or writeoutput fails
    while(!m_stop)
    {
      if (!WriteOutput())
        break;
    }

    CloseDevice();

    log("Device %s closed", m_name.c_str());
  }

  log("Device %s stopped", m_name.c_str());
}
  
