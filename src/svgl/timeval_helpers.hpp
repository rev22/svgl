/*
 * $Id$
 *
Copyright (c) 2001 Stephane Conversy, Jean-Daniel Fekete and Ecole des
Mines de Nantes.
All rights reserved.
 
This software is proprietary information of Stephane Conversy,
Jean-Daniel Fekete and Ecole des Mines de Nantes.  You shall use it
only in accordance with the terms of the license agreement you
accepted when downloading this software.  The license is available in
the file licence.txt and at the following URL:
http://www.emn.fr/info/image/Themes/Indigo/licence.html

 */
#ifndef __timeval_util__
#define __timeval_util__

#include <svgl/config.hpp>
#if defined(WIN32) && ! defined(__CYGWIN__)
#include <windows.h>
#else
#include <sys/time.h>
#endif
#include <iostream>

inline
timeval
operator-=(timeval& t1, const timeval& t2)
{
	t1.tv_usec -= t2.tv_usec;
	if(t1.tv_usec < 0) {
		t1.tv_usec+=1000000;
		--t1.tv_sec;
	}
	t1.tv_sec -= t2.tv_sec;
	return t1;
}


inline
timeval
operator-(const timeval& t1, const timeval& t2)
{
	timeval res = t1;
	res-=t2;
	return res;
	
}

inline
timeval
operator-(const timeval& t)
{
	timeval res = t;
	res.tv_sec = - res.tv_sec - 1;
	res.tv_usec = 1000000-res.tv_usec; 
	return res;
}


inline
timeval
operator+=(timeval& t1, const timeval& t2)
{
	t1.tv_usec += t2.tv_usec;
	if(t1.tv_usec > 1000000) {
		t1.tv_usec-=1000000;
		++t1.tv_sec;
	}
	t1.tv_sec += t2.tv_sec;
	return t1;
}


inline
timeval
operator+(const timeval& t1, const timeval& t2)
{
	timeval res = t1;
	res+=t2;
	return res;
	
}


inline
std::ostream&
operator<<(std::ostream& out, const timeval& t)
{
	out << "s: " << t.tv_sec << " us: " << t.tv_usec;
	return out;
}



inline
bool
is_neg(const timeval& t)
{
	return (t.tv_sec < 0);
}


inline
bool
operator<(const timeval& t1, const timeval& t2)
{
	if(t1.tv_sec == t2.tv_sec) {
		return t1.tv_usec < t2.tv_usec;
	}
	else
		return (t1.tv_sec < t2.tv_sec);

}


inline
bool
operator<=(const timeval& t1, const timeval& t2)
{
	if(t1.tv_sec == t2.tv_sec) {
		return t1.tv_usec <= t2.tv_usec;
	}
	else
		return (t1.tv_sec <= t2.tv_sec);

}


inline
bool
operator>(const timeval& t1, const timeval& t2)
{
	return !(t1<=t2);
}

inline
bool
operator>=(const timeval& t1, const timeval& t2)
{
	return !(t1<t2);
}


inline
int
timeval_to_ms(const timeval& t)
{
	return t.tv_sec * 1000 + t.tv_usec/1000;
}


inline
int
timeval_to_us(const timeval& t)
{
	return t.tv_sec * 1000000 + t.tv_usec;
}


inline
timeval
ms_to_timeval(int ms)
{
	timeval t;

	if(ms>0) {
		t.tv_sec = ms / 1000;
		t.tv_usec = (ms % 1000) * 1000;
	}
	else {
		ms = -ms;
		t.tv_sec = ms / 1000;
		t.tv_usec = (ms % 1000) * 1000;
		t = -t;
	}

	return t;
}

inline
timeval
us_to_timeval(int us)
{
	timeval t;

	if(us>=0) {
		t.tv_sec = us / 1000000;
		t.tv_usec = (us % 1000000);
	}
	else {
		us = -us;
		t.tv_sec = us / 1000000;
		t.tv_usec = (us % 1000000);
		t = -t;
	}

	return t;
}

#endif //__timeval_util__
