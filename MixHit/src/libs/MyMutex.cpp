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
#include "MyMutex.h"

volatile bool mIsLocked_1 = false;
volatile bool mIsLocked_2 = false;
volatile bool mIsLocked_3 = false;
volatile bool mIsLocked_4 = false;
volatile bool mIsLocked_5 = false;

bool interlockedExchange_1(bool v)
{
	bool ov = mIsLocked_1;
	mIsLocked_1 = v;
	return ov;
}
void MyMutex_mQueue_lock()
{
	bool prev;
	while (1)
	{
		while (mIsLocked_1) {};

		prev = interlockedExchange_1(true);

		if (!prev)
			break;
	}
}
void MyMutex_mQueue_unlock()
{
	interlockedExchange_1(false);
}

bool interlockedExchange_2(bool v)
{
	bool ov = mIsLocked_2;
	mIsLocked_2 = v;
	return ov;
}
void MyMutex_mLinesToShow_lock()
{
	bool prev;
	while (1)
	{
		while (mIsLocked_2) {};

		prev = interlockedExchange_2(true);

		if (!prev)
			break;
	}
}
void MyMutex_mLinesToShow_unlock()
{
	interlockedExchange_2(false);
}

bool interlockedExchange_3(bool v)
{
	bool ov = mIsLocked_3;
	mIsLocked_3 = v;
	return ov;
}
void MyMutex_gMachineState_lock()
{
	bool prev;
	while (1)
	{
		while (mIsLocked_3) {};

		prev = interlockedExchange_3(true);

		if (!prev)
			break;
	}
}
void MyMutex_gMachineState_unlock()
{
	interlockedExchange_3(false);
}

bool interlockedExchange_4(bool v)
{
	bool ov = mIsLocked_4;
	mIsLocked_4 = v;
	return ov;
}
void MyMutex_I2C_lock()
{
	bool prev;
	while (1)
	{
		while (mIsLocked_4) {};

		prev = interlockedExchange_4(true);

		if (!prev)
			break;
	}
}
void MyMutex_I2C_unlock()
{
	interlockedExchange_4(false);
}

bool interlockedExchange_5(bool v)
{
	bool ov = mIsLocked_5;
	mIsLocked_5 = v;
	return ov;
}
void MyMutex_MenueItems_lock()
{
	bool prev;
	while (1)
	{
		while (mIsLocked_5) {};

		prev = interlockedExchange_5(true);

		if (!prev)
			break;
	}
}
void MyMutex_MenueItems_unlock()
{
	interlockedExchange_5(false);
}

//void MyMutex_mLinesToShow_lock()
//{
//	while (mIsLocked_2)
//	{
//
//	}
//	mIsLocked_2 = true;
//}
//void MyMutex_mLinesToShow_unlock()
//{
//	mIsLocked_2 = false;
//}
//
//void MyMutex_gMachineState_lock()
//{
//	while (mIsLocked_3)
//	{
//
//	}
//	mIsLocked_3 = true;
//}
//void MyMutex_gMachineState_unlock()
//{
//	mIsLocked_3 = false;
//}
//
//void MyMutex_I2C_lock()
//{
//	while (mIsLocked_4)
//	{
//
//	}
//	mIsLocked_4 = true;
//}
//void MyMutex_I2C_unlock()
//{
//	mIsLocked_4 = false;
//}
//
//void MyMutex_MenueItems_lock()
//{
//	while (mIsLocked_5)
//	{
//
//	}
//	mIsLocked_5 = true;
//}
//void MyMutex_MenueItems_unlock()
//{
//	mIsLocked_5 = false;
//}

//
//MyMutex::MyMutex()
//{
//	//ctor
//	m_count = 1;
//	isLocked = false;
//}
//
//MyMutex::~MyMutex()
//{
//	//dtor
//}
//
//uint8_t MyMutex::_interlockedExchange(uint8_t v)
//{
//	uint8_t ov = m_count;
//	m_count = v;
//
//	return (ov);
//}
//
//void MyMutex::lock()
//{
//	while (isLocked)
//	{
//		;
//	}
//	isLocked = true;
//	/*Serial.println("lock");
//	uint8_t prev;
//
//	while (1)
//	{
//		while (m_count == 0) {};
//
//		prev = _interlockedExchange(0);
//
//		if (prev == 1)
//			break;
//	}*/
//}
//
//void MyMutex::unlock()
//{
//	Serial.println("unlock");
//	isLocked = false;
//	//(void)_interlockedExchange(1);
//}


//bool MyMutex::isLocked()
//{
//	if (m_count == 0)
//		return (true);
//
//	return (false);
//}