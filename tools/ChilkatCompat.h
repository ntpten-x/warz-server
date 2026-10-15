#pragma once

// Compatibility helpers for Chilkat C++ 11.x (UnlockBundle / BinData body APIs).
#include "CkGlobal.h"
#include "CkHttp.h"
#include "CkHttpRequest.h"
#include "CkHttpResponse.h"
#include "CkGzip.h"
#include "CkByteData.h"
#include "CkBinData.h"

inline bool Chilkat_UnlockOnce()
{
	static int s_status = -1; // -1 unknown, 0 fail, 1 ok
	if (s_status < 0)
	{
		CkGlobal glob;
		// Fully-functional 30-day trial unlock (any non-empty string).
		s_status = glob.UnlockBundle("Hello") ? 1 : 0;
	}
	return s_status == 1;
}

inline bool Chilkat_UnlockHttp(CkHttp& /*http*/)
{
	return Chilkat_UnlockOnce();
}

inline bool Chilkat_UnlockGzip(CkGzip& /*gzip*/)
{
	return Chilkat_UnlockOnce();
}

inline void Chilkat_UsePost(CkHttpRequest& req)
{
	req.put_HttpVerb("POST");
}

inline bool Chilkat_GetBody(CkHttpResponse* resp, CkByteData& outData)
{
	if (!resp)
		return false;

	CkBinData bd;
	if (!resp->GetBodyBd(bd))
		return false;

	return bd.GetBinary(outData);
}
