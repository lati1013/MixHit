/*
Mutex class library for Arduino (tm), version 1.0
Copyright (C) 2013 F1RMB, Daniel Caujolle-Bert <f1rmb.daniel@gmail.com>
This program is free software; you can redistribute it and/or
modify it under the terms of the GNU General Public License
as published by the Free Software Foundation; either version 2
of the License, or (at your option) any later version.
This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.
You should have received a copy of the GNU General Public License
along with this program; if not, write to the Free Software
Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef _MYMUTEX_H_
#define _MYMUTEX_H_

#include <Arduino.h>

void MyMutex_mQueue_lock();
void MyMutex_mLinesToShow_lock();
void MyMutex_gMachineState_lock();
void MyMutex_I2C_lock();
void MyMutex_MenueItems_lock();

void MyMutex_mQueue_unlock();
void MyMutex_mLinesToShow_unlock();
void MyMutex_gMachineState_unlock();
void MyMutex_I2C_unlock();
void MyMutex_MenueItems_unlock();


#endif