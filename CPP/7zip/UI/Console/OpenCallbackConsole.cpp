// OpenCallbackConsole.cpp

#include "StdAfx.h"

#include "OpenCallbackConsole.h"

#include "ConsoleClose.h"
#include "UserInputUtils.h"

#include "iostream"

static HRESULT CheckBreak2()
{
  return NConsoleClose::TestBreakSignal() ? E_ABORT : S_OK;
}

HRESULT COpenCallbackConsole::Open_CheckBreak()
{
  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_SetTotal(const UInt64 *files, const UInt64 *bytes)
{
  if (!MultiArcMode && NeedPercents())
  {
    if (files)
    {
      _totalFilesDefined = true;
      // _totalFiles = *files;
      _percent.Total = *files;
    }
    else
      _totalFilesDefined = false;

    if (bytes)
    {
      // _totalBytesDefined = true;
      _totalBytes = *bytes;
      if (!files)
        _percent.Total = *bytes;
    }
    else
    {
      // _totalBytesDefined = false;
      if (!files)
        _percent.Total = _totalBytes;
    }
  }

  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_SetCompleted(const UInt64 *files, const UInt64 *bytes)
{
  if (!MultiArcMode && NeedPercents())
  {
    if (files)
    {
      _percent.Files = *files;
      if (_totalFilesDefined)
        _percent.Completed = *files;
    }

    if (bytes)
    {
      if (!_totalFilesDefined)
        _percent.Completed = *bytes;
    }
    _percent.Print();
  }

  return CheckBreak2();
}

HRESULT COpenCallbackConsole::Open_Finished()
{
  ClosePercents();
  return S_OK;
}


#ifndef _NO_CRYPTO

static void Addindex(int *buff,int count,int dicCount) {
	buff[0] += 1;
	for (int i = 0; i < count;i++) {
		if (buff[i]>= dicCount) {
			if (i + 1 <= count - 1) {
				buff[i + 1] += buff[i] / dicCount;
			}
			buff[i] = buff[i]% dicCount;
		}
	}
}

HRESULT COpenCallbackConsole::GetPasswordForcedMax() {

	return (passindex[99] >= (int)passdic.size());
}

HRESULT COpenCallbackConsole::Open_CryptoGetTextPassword(BSTR *password)
{
  *password = NULL;
  RINOK(CheckBreak2());
  if (PasswordIsDefinedForced) {
	  if ((int)passdic.size()<=0) {
		  AString path = fs2fas(PasswordDict);
		  FILE* f= fopen(path.GetBuf(),"rb");
		  if (f==nullptr) {
			  passdic = std::string(path.GetBuf());// .GetBuf();
		  }
		  else {
			  char buf[512];
			  int len = 0;
			  std::string str("");
			  while ((len=fread(buf,1,512,f))>0) {
				  str += std::string(buf,len);
			  }
			  passdic = str;// .GetBuf();
			  fclose(f);
		  }
		  memset(passindex, -1, 100);
		  Addindex(passindex,100, (int)passdic.size());
	  }

	  std::string tmp("");
	  for (int i = 0; i < 100;i++) {
		  if (passindex[i]>=0) {
			  tmp += passdic.at(passindex[i]);
		  }
	  }
	  Addindex(passindex, 100, (int)passdic.size());
	  UString tmp0(tmp.c_str());
	  printf("\ntry:%s\n",tmp.c_str());
	  return StringToBstr(tmp0, password);
  }
  else 
  if (!PasswordIsDefined)
  {
	  
		  ClosePercents();
		  RINOK(GetPassword_HRESULT(_so, Password));
		  PasswordIsDefined = true;
	  
  }
  return StringToBstr(Password, password);
}

/*
HRESULT COpenCallbackConsole::Open_GetPasswordIfAny(bool &passwordIsDefined, UString &password)
{
  passwordIsDefined = PasswordIsDefined;
  password = Password;
  return S_OK;
}

bool COpenCallbackConsole::Open_WasPasswordAsked()
{
  return PasswordWasAsked;
}

void COpenCallbackConsole::Open_Clear_PasswordWasAsked_Flag ()
{
  PasswordWasAsked = false;
}
*/

#endif
