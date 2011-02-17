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

#include <string.h>

#include "util/log.h"
#include "util/misc.h"
#include "devicers232.h"

CDeviceRS232::CDeviceRS232(CClientsHandler& clients) : m_timer(&m_stop), CDevice(clients)
{
  m_type = -1;
  m_buff = NULL;
  m_bits = 8;
}

void CDeviceRS232::SetType(int type)
{
  m_type = type;
  if (type == ATMO) //atmo devices have two bytes for startchannel and one byte for number of channels
  {
    //it doesn't say anywhere if the startchannel is big endian or little endian, so I'm just starting at 0
    m_prefix.push_back(0xFF);
    m_prefix.push_back(0);
    m_prefix.push_back(0);
    m_prefix.push_back(m_channels.size());
  }
  else if (type == KARATE)
  {
    m_prefix.push_back(0xAA);
    m_prefix.push_back(0x12);
    m_prefix.push_back(0);
    m_prefix.push_back(m_channels.size());
  }
}

void CDeviceRS232::Sync()
{
  if (m_allowsync)
    m_timer.Signal();
}

bool CDeviceRS232::SetupDevice()
{
  m_timer.SetInterval(m_interval);

  //try to open the serial port
  if (!m_serialport.Open(m_output, m_rate))
  {
    logerror("%s: %s", m_name.c_str(), m_serialport.GetError().c_str());
    return false;
  }
  m_serialport.PrintToStdOut(m_debug); //print serial data to stdout when debug mode is on

  //bytes per channel
  m_bytes = m_bits / 8 + ((m_bits % 8) ? 1 : 0);

  //allocate a buffer, that can hold the prefix, and the number of bytes per channel
  m_buff = new unsigned char[m_prefix.size() + m_channels.size() * m_bytes];

  //copy in the prefix
  if (m_prefix.size() > 0)
    memcpy(m_buff, &m_prefix[0], m_prefix.size());

  return true;
}

bool CDeviceRS232::WriteOutput()
{
  //get the channel values from the clienshandler
  int64_t now = m_clock.GetTime();
  m_clients.FillChannels(m_channels, now, this);

  int64_t maxvalue = (1 << m_bits) - 1;

  //put the values in 1 byte unsigned in the buffer
  for (int i = 0; i < m_channels.size(); i++)
  {
    int64_t output = Round<int64_t>((double)m_channels[i].GetValue(now) * maxvalue);
    output = Clamp(output, 0, maxvalue);

    for (int j = 0; j < m_bytes; j++)
      m_buff[m_prefix.size() + i * m_bytes + j] = (output >> ((m_bytes - j - 1) * 8)) & 0xFF;
  }

  //write the channel values out the serial port
  if (m_serialport.Write(m_buff, m_prefix.size() + m_channels.size() * m_bytes) == -1)
  {
    logerror("%s: %s", m_name.c_str(), m_serialport.GetError().c_str());
    return false;
  }

  m_timer.Wait();
  
  return true;
}

void CDeviceRS232::CloseDevice()
{
  //if opened, set all channels to 0 before closing
  if (m_buff)
  {
    memset(m_buff + m_prefix.size(), 0, m_channels.size() * m_bytes);
    m_serialport.Write(m_buff, m_prefix.size() + m_channels.size() * m_bytes);

    delete m_buff;
    m_buff = NULL;
  }

  m_serialport.Close();
}

