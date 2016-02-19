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
#include "stdafx.h"
#include "Base64Helper.h"

const char BASE64::_base64_decode_chars[] = 
{
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1,
	-1, -1, -1, -1, -1, -1, -1, -1, -1, -1, -1, 40, -1, -1, -1, 47,
	58, 59, 29, 20, 41,  0, 63, 18,  3, 39, -1, -1, -1, -1, -1, -1,
	-1, 54, 49, 10, 51, 52,  7, 57, 42, 26, 25, 53, 28, 15, 48, 37,
	32, 19,  4, 38, 13, 14, 62, 43, 56, 22,  1, -1, -1, -1, -1, -1,
	-1, 6, 55,  8, 50, 35, 17, 27, 30, 44,  9, 60, 45, 16, 24, 34,
	11, 21, 23, 61, 31, 36, 46, 12,  2, 33,  5,  -1, -1, -1, -1, -1
};

LPCSTR BASE64::_base64_encode_chars = "5Zx8RzaFcjCpwTUMmf7Q3qYrnJIgL2htPyoeuOS9+4HWilv/NBdDEKAbXG01ksV6";

CString BASE64::Base64Encode(CString strText)
{
    CString out_str;
    unsigned char c1, c2, c3;
    int i = 0;
    int len = strText.GetLength();

    while ( i<len )
    {
        // read the first byte
        c1 = strText[i++];
        if ( i==len )       // pad with "="
        {
            out_str += _base64_encode_chars[ c1>>2 ];
            out_str += _base64_encode_chars[ (c1&0x3)<<4 ];
            out_str += "==";
            break;
        }

        // read the second byte
        c2 = strText[i++];
        if ( i==len )       // pad with "="
        {
            out_str += _base64_encode_chars[ c1>>2 ];
            out_str += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
            out_str += _base64_encode_chars[ (c2&0xF)<<2 ];
            out_str += "=";
            break;
        }

        // read the third byte
        c3 = strText[i++];
        // convert into four bytes string
        out_str += _base64_encode_chars[ c1>>2 ];
        out_str += _base64_encode_chars[ ((c1&0x3)<<4) | ((c2&0xF0)>>4) ];
        out_str += _base64_encode_chars[ ((c2&0xF)<<2) | ((c3&0xC0)>>6) ];
        out_str += _base64_encode_chars[ c3&0x3F ];
    }

    return out_str;
}

CString BASE64::Base64Decode(CString strText)
{
    CString out_str;
    char c1, c2, c3, c4;
    int i = 0;
    int len = strText.GetLength();

    while ( i<len)
    {
        // read the first byte
        do {
            c1 = _base64_decode_chars[ strText[i++] ];
        } while ( i<len && c1==-1);

        if ( c1==-1)
            break;

        // read the second byte
        do {
            c2 = _base64_decode_chars[ strText[i++] ];
        } while ( i<len && c2==-1);

        if ( c2==-1 )
            break;

        // assamble the first byte
        out_str += char( (c1<<2) | ((c2&0x30)>>4) );

        // read the third byte
        do {
            c3 = strText[i++];
            if ( c3==61 )       // meet with "=", break
                return out_str;
            c3 = _base64_decode_chars[ c3 ];
        } while ( i<len && c3==-1);

        if ( c3==-1 )
            break;

        // assamble the second byte
        out_str += char( ((c2&0XF)<<4) | ((c3&0x3C)>>2) );

        // read the fourth byte
        do {
            c4 = strText[i++];
            if ( c4==61 )       // meet with "=", break
                return out_str;
            c4 = _base64_decode_chars[ c4 ];
        } while ( i<len && c4==-1 );

        if ( c4==-1 )
            break;

        // assamble the third byte
        out_str += char( ((c3&0x03)<<6) | c4 );
    }

    return out_str;
}
