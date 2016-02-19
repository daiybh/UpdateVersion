////////////////////////////////////////////////////////////////////////////////
// General utilities : Base64 encode/decode helper class
//
// Copyright (c) 2003 by Morning
// http://morningspace.51.net
// mailto:moyingzz@etang.com
//
// Permission to use, copy, modify, distribute and sell this program for any 
// purpose is hereby granted without fee, provided that the above copyright 
// notice appear in all copies and that both that copyright notice and this 
// permission notice appear in supporting documentation.
//
// It is provided "as is" without express or implied warranty.
////////////////////////////////////////////////////////////////////////////////

#ifndef _BASE64HELPER_H_
#define _BASE64HELPER_H_

class BASE64
{
public:
	BASE64();
	virtual ~BASE64();
	static CString Base64Encode(CString strText);
	static CString Base64Decode(CString strText);
protected:
private:
	static LPCSTR _base64_encode_chars;
	static const char _base64_decode_chars[128];
};

#endif  // _BASE64HELPER_H_