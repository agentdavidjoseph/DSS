#include <stdafx.h>

#include "BitmapExt.h"
#include "DSSTools.h"
#include "DSSProgress.h"
#include "FrameInfo.h"


/* ------------------------------------------------------------------- */

bool	CFrameInfo::InitFromFile(const fs::path& file, PICTURETYPE Type)
{
	bool				bResult = false;

	if (Type == PICTURETYPE_REFLIGHTFRAME)
		m_PictureType  = PICTURETYPE_LIGHTFRAME;
	else
		m_PictureType  = Type;
	filePath = file;

	/*
	FILETIME		FileTime;
	TCHAR			szTime[200];
	TCHAR			szDate[200];
	CString			strDateTime;

	GetFileCreationDateTime(szFile, FileTime);
	FileTimeToSystemTime(&FileTime, &m_FileTime);
	SystemTimeToTzSpecificLocalTime(nullptr, &m_FileTime, &m_FileTime);

	GetDateFormat(LOCALE_USER_DEFAULT, 0, &m_FileTime, nullptr, szDate, sizeof(szDate));
	GetTimeFormat(LOCALE_USER_DEFAULT, 0, &m_FileTime, nullptr, szTime, sizeof(szTime));

	strDateTime.Format("%s %s", szDate, szTime);
	m_strDateTime = strDateTime;
	*/

	CBitmapInfo			bmpInfo;

	bResult = GetPictureInfo(file.c_str() ,bmpInfo);

	if (bResult)
	{
		m_lWidth			= bmpInfo.m_lWidth;
		m_lHeight			= bmpInfo.m_lHeight;
		m_lBitPerChannels	= bmpInfo.m_lBitPerChannel;
		m_lNrChannels		= bmpInfo.m_lNrChannels;
		m_CFAType			= bmpInfo.m_CFAType;
		m_bFITS16bit		= bmpInfo.m_bFITS16bit;
		m_DateTime			= bmpInfo.m_DateTime;
		m_strDateTime		= bmpInfo.m_strDateTime;
		m_ExtraInfo			= bmpInfo.m_ExtraInfo;

		bmpInfo.GetDescription(m_strInfos);

		m_lISOSpeed			= bmpInfo.m_lISOSpeed;
		m_lGain				= bmpInfo.m_lGain;
		m_fExposure			= bmpInfo.m_fExposure;
		m_fAperture			= bmpInfo.m_fAperture;

		m_bMaster			= bmpInfo.IsMaster();
		m_filterName		= bmpInfo.m_filterName;

		RefreshSuperPixel();
	};

	return bResult;
};

/* ------------------------------------------------------------------- */
