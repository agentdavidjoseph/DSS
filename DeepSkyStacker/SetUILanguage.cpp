#include <stdafx.h>
#include "SetUILanguage.h"

#include "DSS-VersionHelpers.h"
#include <QCoreApplication>
#include <QLocale>
#include <QSettings>

/* ------------------------------------------------------------------- */


void	SetUILanguage(LANGID LangID)
{
	if (IsWindowsVistaOrGreater())
	{
		// Vista specific
		typedef LANGID (WINAPI * SetThreadUILanguageProc)(LANGID);
		typedef LANGID (WINAPI * SetThreadPreferredUILanguagesProc)(DWORD, PCWSTR, PULONG);

		SetThreadUILanguageProc				SetThreadUILanguage;
		SetThreadPreferredUILanguagesProc	SetThreadPreferredUILanguages;

#ifndef MUI_LANGUAGE_NAME
		const UINT MUI_LANGUAGE_NAME	= 0x8; // Use ISO language (culture) name convention
#endif
//		const UINT MUI_THREAD_LANGUAGES = 0x40; // GetThreadPreferredUILanguages merges in thread preferred languages

		SetThreadPreferredUILanguages =
			(SetThreadPreferredUILanguagesProc)GetProcAddress(
				GetModuleHandle(_T("kernel32.dll")),
				"SetThreadPreferredUILanguages");
		if (SetThreadPreferredUILanguages)
		{
			// Set the preferred languages
			WCHAR			szPrefLanguages[2000];
			const WCHAR *			szEnglish = L"en-US";
			const WCHAR *			szFrench  = L"fr-FR";
			const WCHAR *			szCzech   = L"cs-CZ";
			const WCHAR *			szSpanish = L"es-ES";
			const WCHAR *			szItalian = L"it-IT";
			const WCHAR *			szCatalan = L"ca-ES";	// Catalan - Espana
			const WCHAR *			szGerman  = L"de-DE";
			const WCHAR *			szDutch   = L"nl-NL";
			const WCHAR *			szChinese = L"zh-TW";	// Traditional Chinese or zh-Hant
			//const WCHAR *			szChinese = L"zh-CN";	// Simplified Chinese or zh-Hans
			const WCHAR *			szPortuguese = L"pt-BR";	// Portuguese/Brazilian
			const WCHAR *			szRomanian = L"ro-RO";	// Romanian
			const WCHAR *			szRussian = L"ru-RU";	// Russian
			const WCHAR *			szTurkish = L"tr-TR";	// Turkish
			ULONG			lNumLang = 5;

			const WCHAR *			szLang1;
			const WCHAR *			szLang2;
			const WCHAR *			szLang3;
			const WCHAR *			szLang4;
			const WCHAR *			szLang5;

			switch (PRIMARYLANGID(LangID))
			{
				case LANG_FRENCH :
					szLang1 = szFrench;
					szLang2 = szEnglish;
					szLang3 = szSpanish;
					szLang4 = szItalian;
					szLang5 = szCzech;
					break;
				case LANG_SPANISH :
					szLang1 = szSpanish;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szItalian;
					szLang5 = szCzech;
					break;
				case LANG_ENGLISH :
                default:
					szLang1 = szEnglish;
					szLang2 = szFrench;
					szLang3 = szSpanish;
					szLang4 = szItalian;
					szLang5 = szCzech;
					break;
				case LANG_GERMAN :
					szLang1 = szGerman;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_DUTCH :
					szLang1 = szDutch;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szGerman;
					szLang5 = szItalian;
					break;
				case LANG_CZECH :
					szLang1 = szCzech;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_ITALIAN :
					szLang1 = szItalian;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szCzech;
					break;
				case LANG_CATALAN :
					szLang1 = szCatalan;
					szLang2 = szSpanish;
					szLang3 = szFrench;
					szLang4 = szEnglish;
					szLang5 = szItalian;
					break;
				case LANG_CHINESE :
					szLang1 = szChinese;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_PORTUGUESE :
					szLang1 = szPortuguese;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_ROMANIAN :
					szLang1 = szRomanian;
					szLang2 = szFrench;
					szLang3 = szEnglish;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_RUSSIAN :
					szLang1 = szRussian;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
				case LANG_TURKISH :
					szLang1 = szTurkish;
					szLang2 = szEnglish;
					szLang3 = szFrench;
					szLang4 = szSpanish;
					szLang5 = szItalian;
					break;
			};

			WCHAR *			szPtr = szPrefLanguages;
			memcpy(szPtr, szLang1, sizeof(WCHAR)*6);
			szPtr += 6;
			memcpy(szPtr, szLang2, sizeof(WCHAR)*6);
			szPtr += 6;
			memcpy(szPtr, szLang3, sizeof(WCHAR)*6);
			szPtr += 6;
			memcpy(szPtr, szLang4, sizeof(WCHAR)*6);
			szPtr += 6;
			memcpy(szPtr, szLang5, sizeof(WCHAR)*6);
			szPtr += 6;
			*szPtr = 0;

			SetThreadPreferredUILanguages(MUI_LANGUAGE_NAME/* | MUI_THREAD_LANGUAGES*/, szPrefLanguages, &lNumLang);
		};
		SetThreadUILanguage = (SetThreadUILanguageProc)GetProcAddress(
			GetModuleHandle(_T("kernel32.dll")),
			"SetThreadUILanguage");
		if (SetThreadUILanguage)
			SetThreadUILanguage(LangID);
	}
	else
		SetThreadLocale(MAKELCID(LangID,SORT_DEFAULT));
};

/* ------------------------------------------------------------------- */

typedef enum tagDSSLANGUAGE
{
	DSSL_DEFAULT		= 0,
	DSSL_FRENCH			= 1,
	DSSL_ENGLISH		= 2,
	DSSL_SPANISH		= 3,
	DSSL_CZECH			= 4,
	DSSL_ITALIAN		= 5,
	DSSL_DUTCH			= 6,
	DSSL_CATALAN		= 7,
	DSSL_GERMAN			= 8,
	DSSL_CHINESE		= 9,
	DSSL_PORTUGUESE		= 10,
	DSSL_ROMANIAN		= 11,
	DSSL_RUSSIAN		= 12,
	DSSL_TURKISH		= 13
}DSSLANGUAGE;

/* ------------------------------------------------------------------- */

void	SetUILanguage()
{
	DSSLANGUAGE			DSSLanguage = DSSL_DEFAULT;
	QSettings settings;
	QString				language;

	//
	// Retrieve the Qt language name (e.g.) en_GB
	//
	language = settings.value("Language").toString();

	//
	// Language was not defined in our preferences, so select the system default
	//
	if (language == "")
	{
		language = QLocale::system().name();
	}

	//
	// What follows is for the older MFC stuff which will go one day!
	//
	// Convert the Qt language name to the DSSLANGUAGE enum values
	//
	if (language.length())
	{
		if (!language.compare("fr_FR", Qt::CaseInsensitive))
			DSSLanguage = DSSL_FRENCH;
		else if (!language.compare("en_GB", Qt::CaseInsensitive))
			DSSLanguage = DSSL_ENGLISH;
		else if (!language.compare("es_ES", Qt::CaseInsensitive))
			DSSLanguage = DSSL_SPANISH;
		else if (!language.compare("cs_CZ", Qt::CaseInsensitive))
			DSSLanguage = DSSL_CZECH;
		else if (!language.compare("it_IT", Qt::CaseInsensitive))
			DSSLanguage = DSSL_ITALIAN;
		else if (!language.compare("ca_ES", Qt::CaseInsensitive))
			DSSLanguage = DSSL_CATALAN;
		else if (!language.compare("de_DE", Qt::CaseInsensitive))
			DSSLanguage = DSSL_GERMAN;
		else if (!language.compare("nl_NL", Qt::CaseInsensitive))
			DSSLanguage = DSSL_DUTCH;
		else if (!language.compare("zh_TW", Qt::CaseInsensitive))
			DSSLanguage = DSSL_CHINESE;
		else if (!language.compare("pt_BR", Qt::CaseInsensitive))
			DSSLanguage = DSSL_PORTUGUESE;
		else if (!language.compare("ro_RO", Qt::CaseInsensitive))
			DSSLanguage = DSSL_ROMANIAN;
		else if (!language.compare("ru_RU", Qt::CaseInsensitive))
			DSSLanguage = DSSL_RUSSIAN;
		else if (!language.compare("tr_TR", Qt::CaseInsensitive))
			DSSLanguage = DSSL_TURKISH;
	};

	if (DSSLanguage == DSSL_DEFAULT)
	{
		// Get the main current language and adapth it
		LANGID			LangID;

		LangID = GetUserDefaultLangID();
		//LangID = LANGIDFROMLCID(GetThreadLocale());
		switch (PRIMARYLANGID(LangID))
		{
		case LANG_FRENCH :
			DSSLanguage = DSSL_FRENCH;
			break;
		case LANG_SPANISH :
			DSSLanguage = DSSL_SPANISH;
			break;
		case LANG_ENGLISH :
			DSSLanguage = DSSL_ENGLISH;
			break;
		case LANG_ITALIAN :
			DSSLanguage = DSSL_ITALIAN;
			break;
		case LANG_CZECH :
			DSSLanguage = DSSL_CZECH;
			break;
		case LANG_CATALAN :
			DSSLanguage = DSSL_CATALAN;
			break;
		case LANG_GERMAN :
			DSSLanguage = DSSL_GERMAN;
			break;
		case LANG_DUTCH :
			DSSLanguage = DSSL_DUTCH;
			break;
		case LANG_CHINESE :
			DSSLanguage = DSSL_CHINESE;
			break;
		case LANG_PORTUGUESE :
			DSSLanguage = DSSL_PORTUGUESE;
			break;
		case LANG_ROMANIAN :
			DSSLanguage = DSSL_ROMANIAN;
			break;
		case LANG_RUSSIAN :
			DSSLanguage = DSSL_RUSSIAN;
			break;
		case LANG_TURKISH :
			DSSLanguage = DSSL_TURKISH;
			break;
		};
	};

	switch (DSSLanguage)
	{
	case DSSL_FRENCH :
		SetUILanguage(MAKELANGID(LANG_FRENCH,SUBLANG_DEFAULT));
		break;
	case DSSL_SPANISH:
		SetUILanguage(MAKELANGID(LANG_SPANISH,SUBLANG_SPANISH_MODERN));
		break;
	case DSSL_ITALIAN:
		SetUILanguage(MAKELANGID(LANG_ITALIAN,SUBLANG_DEFAULT));
		break;
	case DSSL_ENGLISH :
		SetUILanguage(MAKELANGID(LANG_ENGLISH,SUBLANG_ENGLISH_US));
		break;
	case DSSL_CZECH :
		SetUILanguage(MAKELANGID(LANG_CZECH,SUBLANG_DEFAULT));
		break;
	case DSSL_CATALAN :
		SetUILanguage(MAKELANGID(LANG_CATALAN,SUBLANG_DEFAULT));
		break;
	case DSSL_GERMAN :
		SetUILanguage(MAKELANGID(LANG_GERMAN,SUBLANG_DEFAULT));
		break;
	case DSSL_DUTCH :
		SetUILanguage(MAKELANGID(LANG_DUTCH,SUBLANG_DEFAULT));
		break;
	case DSSL_CHINESE :
		SetUILanguage(MAKELANGID(LANG_CHINESE,SUBLANG_CHINESE_TRADITIONAL));
		break;
	case DSSL_PORTUGUESE :
		SetUILanguage(MAKELANGID(LANG_PORTUGUESE,SUBLANG_PORTUGUESE_BRAZILIAN));
		break;
	case DSSL_ROMANIAN :
		SetUILanguage(MAKELANGID(LANG_ROMANIAN,SUBLANG_DEFAULT));
		break;
	case DSSL_RUSSIAN :
		SetUILanguage(MAKELANGID(LANG_RUSSIAN,SUBLANG_DEFAULT));
		break;
	case DSSL_TURKISH :
		SetUILanguage(MAKELANGID(LANG_TURKISH,SUBLANG_DEFAULT));
		break;
	}
};

/* ------------------------------------------------------------------- */
