#pragma once

namespace dxbc
{
	BOOL CalculateDXBCChecksum(BYTE* pData, DWORD dwSize, DWORD dwHash[4]);
}
