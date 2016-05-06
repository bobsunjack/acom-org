#include "stdafx.h"
#include "JPublicConfig.h"

//------------------------------------------------------------------------
//	OUTPUT_LOG �����־ 
//------------------------------------------------------------------------   
void OUTPUT_LOG(const char *format, ...)
{
	try
	{
		char tempformat[1024] = {0};
		sprintf(tempformat,"Csdb->%s",format);

		char buf[4096], *p = buf;
		memset(buf,0,4096*sizeof(char));
		va_list args;
		va_start(args, format);
		p += _vsnprintf(p, sizeof buf - 1, tempformat, args);
		va_end(args);

		//buf�������ݣ���pǰһ���ǿո�
		if ( p > buf && isspace(p[-1]) )
		{
			*p = '\0';
			*p++ = '\r';
			*p++ = '\n';
			*p = '\0';
		}

		OutputDebugString(buf);
	}
	catch(...)
	{
		OutputDebugString("OUTPUT_LOG exception!!!\n");
	}
}