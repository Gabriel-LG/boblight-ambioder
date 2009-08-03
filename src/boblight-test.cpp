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

#include <iostream>

#include "util/tcpsocket.h"
#include "util/misc.h"
#include "util/clock.h"
#include "util/timer.h"

#define BOBLIGHT_DLOPEN
#include "lib/libboblight.h"

using namespace std;

const char* test(void* vpboblight);

int main (int argc, char *argv[])
{
  char* error = boblight_loadlibrary(NULL);

  if (error)
  {
    cout << error << "\n";
    return 1;
  }

  void* boblight = boblight_init();
  if (!boblight_connect(boblight, NULL, -1, 5000))
  {
    cout << test(boblight) << "\n";
  }
  
}