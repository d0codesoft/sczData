
#include "stdafx.h"

#ifdef __linux__
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#endif

#include <wchar.h>
#include <string>
#include <memory>
#include "AddInNative.h"

#ifdef WIN32
#pragma setlocale("ru-RU" )
#endif
#include <clocale>

static const wchar_t g_kClassNames[] = L"AddInSCZ"; //|OtherClass1|OtherClass2";

size_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len = 0);
size_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, size_t len = 0);
size_t getLenShortWcharStr(const WCHAR_T* Source);

//---------------------------------------------------------------------------//
uintptr_t GetClassObject(const wchar_t* wsName, IComponentBase** pInterface)
{
    if(!*pInterface)
    {
        wchar_t* name = nullptr;
        ::convToShortWchar(&name, wsName);
        if (!wcscmp(name, g_kClassNames))
            *pInterface = new CAddInSCZ();
        delete[] name;
        return reinterpret_cast<uintptr_t>(*pInterface);
    }
    return 0;
}
//---------------------------------------------------------------------------//
uintptr_t DestroyObject(IComponentBase** pIntf)
{
   if(!*pIntf)
      return -1;

   delete *pIntf;
   *pIntf = 0;
   return 0;
}
//---------------------------------------------------------------------------//
const WCHAR_T* GetClassNames()
{
    static WCHAR_T* names = 0;
    if (!names)
        ::convToShortWchar(&names, g_kClassNames);
    return names;
}
//---------------------------------------------------------------------------//
//CAddInNative
CAddInSCZ::CAddInSCZ()
{
    m_iMemory = 0;
    m_iConnect = 0;
}
//---------------------------------------------------------------------------//
CAddInSCZ::~CAddInSCZ()
{
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::Init(void* pConnection)
{ 
    m_iConnect = static_cast<IAddInDefBase*>(pConnection);
    return m_iConnect != NULL;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::GetInfo()
{ 
    return 2000; 
}
//---------------------------------------------------------------------------//
void CAddInSCZ::Done()
{
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::RegisterExtensionAs(WCHAR_T** wsLanguageExt)
{ 
    const wchar_t* wsExtension = L"AddInSCZData";
    const size_t iActualSize = ::wcslen(wsExtension) + 1;
    WCHAR_T* dest = 0;

    if (m_iMemory)
    {
        if (m_iMemory->AllocMemory(reinterpret_cast<void**>(wsLanguageExt), iActualSize * sizeof(WCHAR_T)))
            ::convToShortWchar(wsLanguageExt, wsExtension, iActualSize);
        return true;
    }

    return false;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::GetNProps()
{ 
    return eLastProp;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::FindProp(const WCHAR_T* wsPropName)
{ 
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInSCZ::GetPropName(long lPropNum, long lPropAlias)
{ 
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::GetPropVal(const long lPropNum, tVariant* pvarPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::SetPropVal(const long lPropNum, tVariant* varPropVal)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::IsPropReadable(const long lPropNum)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::IsPropWritable(const long lPropNum)
{
    return false;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::GetNMethods()
{ 
    return eLastMethod;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::FindMethod(const WCHAR_T* wsMethodName)
{ 
    return -1;
}
//---------------------------------------------------------------------------//
const WCHAR_T* CAddInSCZ::GetMethodName(const long lMethodNum,
                            const long lMethodAlias)
{ 
    return 0;
}
//---------------------------------------------------------------------------//
long CAddInSCZ::GetNParams(const long lMethodNum)
{ 
    return 0;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::GetParamDefValue(const long lMethodNum, const long lParamNum,
                          tVariant *pvarParamDefValue)
{ 
    return false;
} 
//---------------------------------------------------------------------------//
bool CAddInSCZ::HasRetVal(const long lMethodNum)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::CallAsProc(const long lMethodNum,
                    tVariant* paParams, const long lSizeArray)
{ 
    return false;
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::CallAsFunc(const long lMethodNum,
                tVariant* pvarRetValue, tVariant* paParams, const long lSizeArray)
{ 
    return false; 
}
//---------------------------------------------------------------------------//
void CAddInSCZ::SetLocale(const WCHAR_T* loc)
{
#ifndef __linux__
    _wsetlocale(LC_ALL, loc);
#else
    int size = 0;
    char *mbstr = 0;
    wchar_t *tmpLoc = 0;
    convFromShortWchar(&tmpLoc, loc);
    size = wcstombs(0, tmpLoc, 0)+1;
    mbstr = new char[size];

    if (!mbstr)
    {
        delete[] tmpLoc;
        return;
    }

    memset(mbstr, 0, size);
    size = wcstombs(mbstr, tmpLoc, wcslen(tmpLoc));
    setlocale(LC_ALL, mbstr);
    delete[] tmpLoc;
    delete[] mbstr;
#endif
}

void CAddInSCZ::addError(uint32_t wcode, const wchar_t* source, const wchar_t* descriptor, long code)
{
    if (m_iConnect)
    {
        WCHAR_T* err = 0;
        WCHAR_T* descr = 0;

        ::convToShortWchar(&err, source);
        ::convToShortWchar(&descr, descriptor);

        m_iConnect->AddError(wcode, err, descr, code);
        delete[] err;
        delete[] descr;
    }
}
//---------------------------------------------------------------------------//
bool CAddInSCZ::setMemManager(void* mem)
{
    return false;
}
//---------------------------------------------------------------------------//
size_t convToShortWchar(WCHAR_T** Dest, const wchar_t* Source, size_t len)
{
    if (!len)
        len = ::wcslen(Source)+1;

    if (!*Dest)
        *Dest = new WCHAR_T[len];

    WCHAR_T* tmpShort = *Dest;
    wchar_t* tmpWChar = (wchar_t*) Source;
    size_t res = 0;

    ::memset(*Dest, 0, len*sizeof(WCHAR_T));
    do
    {
        *tmpShort++ = (WCHAR_T)*tmpWChar++;
        ++res;
    }
    while (len-- && *tmpWChar);

    return res;
}
//---------------------------------------------------------------------------//
size_t convFromShortWchar(wchar_t** Dest, const WCHAR_T* Source, size_t len)
{
    if (!len)
        len = getLenShortWcharStr(Source)+1;

    if (!*Dest)
        *Dest = new wchar_t[len];

    wchar_t* tmpWChar = *Dest;
    auto tmpShort = const_cast<WCHAR_T*>(Source);
    size_t res = 0;

    ::memset(*Dest, 0, len*sizeof(wchar_t));
    do
    {
        *tmpWChar++ = static_cast<wchar_t>(*tmpShort++);
        ++res;
    }
    while (len-- && *tmpShort);

    return res;
}
//---------------------------------------------------------------------------//
size_t getLenShortWcharStr(const WCHAR_T* Source)
{
    size_t res = 0;
    auto tmpShort = const_cast<WCHAR_T*>(Source);

    while (*tmpShort++)
        ++res;

    return res;
}
//---------------------------------------------------------------------------//
#ifdef LINUX_OR_MACOS
WcharWrapper::WcharWrapper(const WCHAR_T* str) : m_str_WCHAR(NULL),
m_str_wchar(NULL)
{
    if (str)
    {
        int len = getLenShortWcharStr(str);
        m_str_WCHAR = new WCHAR_T[len + 1];
        memset(m_str_WCHAR, 0, sizeof(WCHAR_T) * (len + 1));
        memcpy(m_str_WCHAR, str, sizeof(WCHAR_T) * len);
        ::convFromShortWchar(&m_str_wchar, m_str_WCHAR);
    }
}
#endif
//---------------------------------------------------------------------------//
WcharWrapper::WcharWrapper(const wchar_t* str) :
#ifdef LINUX_OR_MACOS
    m_str_WCHAR(NULL),
#endif 
    m_str_wchar(NULL)
{
    if (str)
    {
	    const auto len = wcslen(str);
        m_str_wchar = new wchar_t[len + 1];
        memset(m_str_wchar, 0, sizeof(wchar_t) * (len + 1));
        memcpy(m_str_wchar, str, sizeof(wchar_t) * len);
#ifdef LINUX_OR_MACOS
        ::convToShortWchar(&m_str_WCHAR, m_str_wchar);
#endif
    }

}
//---------------------------------------------------------------------------//
WcharWrapper::~WcharWrapper()
{
#ifdef LINUX_OR_MACOS
    if (m_str_WCHAR)
    {
        delete[] m_str_WCHAR;
        m_str_WCHAR = nullptr;
    }
#endif
    if (m_str_wchar)
    {
        delete[] m_str_wchar;
        m_str_wchar = nullptr;
    }
}
//---------------------------------------------------------------------------//