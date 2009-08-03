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

#include "string.h"

#include "util/log.h"
#include "util/misc.h"
#include "devicers232.h"

CDeviceRS232::CDeviceRS232(CClientsHandler& clients, CAsyncTimer& timer) : m_timer(timer), CDevice(clients)
{
  m_type = -1;
  m_buff = NULL;
}

void CDeviceRS232::SetType(int type)
{
  m_type = type;
  if (type == ATMO)
  {
    m_prefix.push_back(0);
    m_prefix.push_back(0);
    m_prefix.push_back(m_channels.size());
  }
}

bool CDeviceRS232::SetupDevice()
{
  if (!m_serialport.Open(m_output, m_rate))
  {
    log("%s %s: %s", m_name.c_str(), m_output.c_str(), m_serialport.GetError().c_str());
    return false;
  }
  m_buff = new unsigned char[m_channels.size()];

  return true;
}

bool CDeviceRS232::WriteOutput()
{
  int64_t now = m_clock.GetTime();
  m_clients.FillChannels(m_channels, now);

  for (int i = 0; i < m_channels.size(); i++)
  {
    int output = Round<int>(m_channels[i].GetValue(now) * 255.0);
    output = Clamp(output, 0, 255);
    m_buff[i] = output;
  }

  if (m_serialport.Write(&m_prefix[0], m_prefix.size()) == -1 || m_serialport.Write(m_buff, m_channels.size()) == -1)
  {
    log("%s %s: %s", m_name.c_str(), m_output.c_str(), GetErrno().c_str());
    return false;
  }

  m_timer.Wait();
  
  return true;
}

void CDeviceRS232::CloseDevice()
{
  if (m_buff)
  {
    if (m_prefix.size() > 0)
      m_serialport.Write(&m_prefix[0], m_prefix.size());

    memset(m_buff, 0, m_channels.size());
    m_serialport.Write(m_buff, m_channels.size());
    m_serialport.Close();

    delete m_buff;
    m_buff = NULL;
  }
}