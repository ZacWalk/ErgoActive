#pragma once

#define XML_STATIC
#include "..\..\Libraries\expat\lib\expat.h"


class Layout
{
public:

	Frames::Group m_navigation;
	Frames::Group *m_pCurrentPage;
	Frames::Style m_styleDefault;

	typedef std::vector<Frames::Group*> PAGELIST;
	PAGELIST m_pagesList;

	typedef std::map<CAdapt<CStringW>, Frames::Group*> PAGEMAP;
	PAGEMAP m_pages;

	typedef std::map<CAdapt<CStringW>, Frames::Style*> STYLEMAP;
	STYLEMAP m_styles;

	Gdiplus::ARGB m_argbBackground;
	Gdiplus::ARGB m_argbText;	
	Gdiplus::Bitmap m_bullet;
	CSize m_size;
	
	Layout() : 
		m_pCurrentPage(0), 
		m_argbBackground(0xffffffff),
		m_argbText(0xff000000),
		m_navigation(0xFF93bC10, 0), 
		m_styleDefault(L"Arial", 10, true, 0xFFFFFFFF), 
		m_bullet(L"bullet.png"),
		m_size(640, 480)
	{
	}


	Gdiplus::ARGB GetFillColor()
	{
		if (m_pCurrentPage != NULL)
		{
			return m_pCurrentPage->m_argb;
		}

		return m_argbBackground;
	}

	Frames::Style &GetStyle(LPCWSTR szName)
	{
		CStringW strName(szName);
		strName.MakeLower();

		STYLEMAP::iterator it = m_styles.find(strName);

		if (it == m_styles.end())
		{
			return m_styleDefault;
		}

		return *(it->second);
	}

	void SetCurrentPage(Frames::Host *pHost, LPCWSTR szName)
	{
		PAGEMAP::iterator it = m_pages.find(CStringW(szName));

		if (it != m_pages.end())
		{
			m_pCurrentPage = it->second;
			pHost->OnShowPage(m_pCurrentPage);
		}
	}

	static Gdiplus::ARGB ParseColor(const char* szColor)
	{
		Gdiplus::ARGB argb = 0;

		if(*szColor == '#')
		{
			sscanf(szColor + 1, "%X", &argb);
			argb |= 0xff000000;
		}

		return argb;
	}

	static bool ParseBool(const char* sz)
	{
		return stricmp(sz, "True") == 0;
	}

	static int ParseNumber(const char* sz)
	{
		return atol(sz);
	}

	bool Parse(HWND hWnd, const char* fname)
	{
		const int buf_size = 1024;
		char buffer[buf_size];

		XML_Parser p = XML_ParserCreate(NULL);
		if(p == 0) 
		{
			return false;
		}

		XML_SetUserData(p, this);
		XML_SetElementHandler(p, StartElement, EndElement);

		FILE* fd = fopen(fname, "rt");
		if(fd == 0)
		{
			return false;
		}

		bool done = false;
		do
		{
			size_t len = fread(buffer, 1, buf_size, fd);
			done = len < buf_size;

			if(!XML_Parse(p, buffer, len, done))
			{
				CString str;
				str.Format(_T("Error loading config file.\n\n%s\nLine: %d\nColumn: %d"), 
					XML_ErrorString(XML_GetErrorCode(p)),
					XML_GetCurrentLineNumber(p),
					XML_GetCurrentColumnNumber(p));
				::MessageBox(hWnd, str, L"ErgoActiv", MB_OK);
				return false;
			}
		}
		while(!done);

		fclose(fd);
		XML_ParserFree(p);

		

		return true;
	}	


	static void StartElement(void* data, const char* el, const char** attr)
	{
		Layout& self = *(Layout*)data;

		if(stricmp(el, "style") == 0)
		{
			self.ParseStyle(attr);			
		} 
		else if (stricmp(el, "default") == 0)
		{
			self.ParseDefault(attr);
		}
		else if (stricmp(el, "page") == 0)
		{
			self.ParsePage(attr);
		}
		else if(stricmp(el, "text") == 0)
		{
			self.ParseText(attr);
		}
		else if(stricmp(el, "bullet") == 0)
		{
			self.ParseBullet(attr);
		}
		else if(stricmp(el, "image") == 0)
		{
			self.ParseImage(attr);
		}
		else if(stricmp(el, "Graph") == 0)
		{
			if (self.m_pCurrentPage != NULL)
			{
				self.m_pCurrentPage->Add(new Frames::Graph(self.GetStyle(L"Graph")));
				self.m_pCurrentPage->SetControlParent();
			}
		}
		
	} 

	static void EndElement(void* data, const char* el)
	{
		Layout& self = *(Layout*)data;

		if(stricmp(el, "page") == 0)
		{
			self.m_pCurrentPage = 0;			
		}
	}

	void ParseDefault(const char** attr)
	{
		for(int i = 0; attr[i]; i += 2)
		{
			if(stricmp(attr[i], "cx") == 0)
			{
				m_size.cx = ParseNumber(attr[i + 1]);
			}
			else if(stricmp(attr[i], "cy") == 0)
			{
				m_size.cy = ParseNumber(attr[i + 1]);
			}
			else if(stricmp(attr[i], "Background") == 0)
			{
				m_argbBackground = ParseColor(attr[i + 1]);
			}
			else if(stricmp(attr[i], "Text") == 0)
			{
				m_argbText = ParseColor(attr[i + 1]);
			}
		}
	}

	void ParseStyle(const char** attr)
	{
		CStringW strName;
		CStringW strFontName(L"Arial");
		int nFontSize = 10;
		bool bFontBold = false;
		Gdiplus::ARGB argb = 0xffffffff;
		int nPadding = 2;

		for(int i = 0; attr[i]; i += 2)
		{
			if(stricmp(attr[i], "name") == 0)
			{
				strName = attr[i + 1];
			}
			else if(stricmp(attr[i], "fontname") == 0)
			{
				strFontName = attr[i + 1];
			}
			else if(stricmp(attr[i], "size") == 0)
			{
				nFontSize = ParseNumber(attr[i + 1]);
			}
			else if(stricmp(attr[i], "bold") == 0)
			{
				bFontBold = ParseBool(attr[i + 1]);
			}
			else if(stricmp(attr[i], "color") == 0)
			{
				argb = ParseColor(attr[i + 1]);
			}
			else if(stricmp(attr[i], "padding") == 0)
			{
				nPadding = ParseNumber(attr[i + 1]);
			}
				
		}

		strName.MakeLower();
		m_styles[strName] = new Frames::Style(strFontName, nFontSize, bFontBold, argb);			
	}

	static void ConvertChars(CStringW &str)
	{
		str.Replace(L"Ã¶", L"ö");
		str.Replace(L"Ã¤", L"ä");
		str.Replace(L"Ã¥", L"å");
	}

	void ParsePage(const char** attr)
	{
		CStringW strName;
		CStringW strStyle = L"Link";
		CStringW strImage = L"";
		bool bImage = false;
		Gdiplus::ARGB argb = 0xffffffff;
		Frames::Frame *pLinkFrame = 0;

		for(int i = 0; attr[i]; i += 2)
		{
			if(stricmp(attr[i], "style") == 0)
			{
				strStyle = attr[i + 1];
			}
			if(stricmp(attr[i], "image") == 0)
			{
				strImage = attr[i + 1];
				bImage = true;
			}
			else if(stricmp(attr[i], "name") == 0)
			{
				strName = attr[i + 1];
				ConvertChars(strName);
			}
			else if(stricmp(attr[i], "color") == 0)
			{
				argb = ParseColor(attr[i + 1]);
			}
		}

		if (!bImage)
		{
			m_navigation.Add(pLinkFrame = new Frames::Link(strName, argb, GetStyle(strStyle)));
		}
		else 
		{
			m_navigation.Add(pLinkFrame = new Frames::LinkImage(strName, strImage, argb));
		}

		m_pages[strName] = m_pCurrentPage = new Frames::Group(argb, pLinkFrame);
		m_pagesList.push_back(m_pCurrentPage);
	}

	void ParseText(const char** attr)
	{
		CStringW str = L"";
		CStringW strStyle = L"Text";
		Gdiplus::ARGB argb = 0xffffffff;

		for(int i = 0; attr[i]; i += 2)
		{
			if(stricmp(attr[i], "style") == 0)
			{
				strStyle = attr[i + 1];
			}
			else if(stricmp(attr[i], "string") == 0)
			{
				str = attr[i + 1];
				ConvertChars(str);
			}
			else if(stricmp(attr[i], "color") == 0)
			{
				argb = ParseColor(attr[i + 1]);
			}
		}

		m_pCurrentPage->Add(new Frames::Text(str, GetStyle(strStyle)));
	}
	

	void ParseBullet(const char** attr)
	{
		USES_CONVERSION;

		if (m_pCurrentPage != NULL)
		{
			CStringW str = L"";

			for(int i = 0; attr[i]; i += 2)
			{
				if(stricmp(attr[i], "string") == 0)
				{
					str = attr[i + 1];
					ConvertChars(str);
				}			
			}
			
			m_pCurrentPage->Add(new Frames::Bullet(str, GetStyle(L"Bullet"), &m_bullet));
		}
	}

	void ParseImage(const char** attr)
	{
		USES_CONVERSION;

		if (m_pCurrentPage != NULL)
		{
			LPCSTR szFileName = "";

			for(int i = 0; attr[i]; i += 2)
			{
				if(stricmp(attr[i], "filename") == 0)
				{
					szFileName = attr[i + 1];
				}		
			}

			m_pCurrentPage->Add(new Frames::Image(A2W(szFileName)));
		}
	}

	void End(Frames::Host *pHost)
	{
		SetPage(pHost, m_pagesList.back());
	}

	void Home(Frames::Host *pHost)
	{
		SetPage(pHost, *m_pagesList.begin());
	}

	void PreviousPage(Frames::Host *pHost)
	{
		for(PAGELIST::iterator it = m_pagesList.begin(); it != m_pagesList.end(); ++it)
		{
			if (m_pCurrentPage == *it && it != m_pagesList.begin())
			{
				--it;
				SetPage(pHost, *it);
				break;
			}
		}
	}

	void NextPage(Frames::Host *pHost)
	{
		for(PAGELIST::iterator it = m_pagesList.begin(); it != m_pagesList.end(); ++it)
		{
			if (m_pCurrentPage == *it && it != m_pagesList.end())
			{
				++it;
				SetPage(pHost, *it);
				break;
			}
		}		
	}

	void SetPage(Frames::Host *pHost, Frames::Group *pGroup)
	{
		for(PAGEMAP::iterator it = m_pages.begin(); it != m_pages.end(); ++it)
		{
			if (pGroup == it->second)
			{
				m_pCurrentPage = pGroup;
				pHost->OnShowPage(it->second);
			}
		}
	}

};