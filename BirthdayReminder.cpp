//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <memory>
#include <mmsystem.h>
#include "Aqq.h"
#include "SettingsFrm.h"
//---------------------------------------------------------------------------

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

TSettingsForm *handle; //tworzenie uchwytu do formy

//utworzenie obiektow do struktur
TPluginShowInfo PluginShowInfo;
TPluginLink PluginLink;
TPluginInfo PluginInfo;

//Program
extern "C"  __declspec(dllexport) PPluginInfo __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  PluginInfo.cbSize = sizeof(TPluginInfo);
  PluginInfo.ShortName = (wchar_t *)L"Birthday Reminder";
  PluginInfo.Version = PLUGIN_MAKE_VERSION(2,0,0,0);
  PluginInfo.Description = (wchar_t *)L"Przypominanie o urodzinach kontaktów";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  if(handle==NULL)
  {
	Application->Handle = SettingsForm;
	handle = new TSettingsForm(Application);
  }

  handle->Timer->Enabled=true;

  return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
bool SaveResourceToFile(char *FileName, char *res)
{
  HRSRC hrsrc = FindResource(HInstance, res, RT_RCDATA);
  if(hrsrc == NULL) return false;
  DWORD size = SizeofResource(HInstance, hrsrc);
  HGLOBAL hglob = LoadResource(HInstance, hrsrc);
  LPVOID rdata = LockResource(hglob);
  HANDLE hFile = CreateFile(FileName, GENERIC_WRITE, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
  DWORD writ;
  WriteFile(hFile, rdata, size, &writ, NULL);
  CloseHandle(hFile);
  return true;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  UnicodeString PluginPath = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  //Tworzenie katalogu
  if(!DirectoryExists(PluginPath + "\\\\BirthdayReminder"))
   CreateDir(PluginPath + "\\\\BirthdayReminder");
  //Wypakowanie ikony
  if(!FileExists(PluginPath + "\\\\BirthdayReminder\\\\cake.png"))
   SaveResourceToFile((PluginPath+ "\\\\BirthdayReminder\\\\cake.png").c_str(),"ID_PNG");
  //Wypakowanie ikony
  if(!FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
   SaveResourceToFile((PluginPath+ "\\\\BirthdayReminder\\\\birthday.wav").c_str(),"ID_SONG");

  PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);
  if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0)==1)
  {
	//Uruchomienie timer'a
	Application->Handle = SettingsForm;
	handle = new TSettingsForm(Application);
	handle->Timer->Enabled=true;
  }

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if(handle==NULL)
  {
	Application->Handle = SettingsForm;
	handle = new TSettingsForm(Application);
  }
  handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

void TestChmurki(int TimeOutTest, bool ShowAgeTest, bool PlaySoundTest)
{
  UnicodeString PluginPath;
  PluginPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  wchar_t* Text;

  if(ShowAgeTest==0)
   Text = L"Jan Kowalski obchodzi dziœ urodziny!";
  else
   Text = L"Jan Kowalski obchodzi dziœ urodziny! (36)";

  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
  PluginShowInfo.Event = tmeInfo;
  PluginShowInfo.Text = Text;
  PluginShowInfo.ImagePath = (PluginPath + "\\\\BirthdayReminder\\\\cake.png").w_str();
  PluginShowInfo.TimeOut = 1000 * TimeOutTest;

  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));

  if(PlaySoundTest==1)
  {
	if(FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
	 PlaySound((PluginPath + "\\\\BirthdayReminder\\\\birthday.wav").c_str(), NULL, SND_ASYNC | SND_FILENAME);
  }
}
//---------------------------------------------------------------------------

UnicodeString GetPluginPath()
{
  UnicodeString Dir;
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

UnicodeString GetContactsPath()
{
  UnicodeString Dir;
  Dir = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(HInstance),0));
  Dir = Dir + "\\Data\\Contacts\\\\";
  return Dir;
}
//---------------------------------------------------------------------------

UnicodeString GetContactNick(UnicodeString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = JID.w_str();
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  JID = (wchar_t*)(PluginContactSimpleInfo.Nick);
  return JID;
}
//---------------------------------------------------------------------------

void ShowBirthdayInfo(UnicodeString CText, int CTimeOut, bool CSoundPlay)
{
  UnicodeString PluginPath;
  PluginPath = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(HInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
  PluginShowInfo.Event = tmeInfo;
  PluginShowInfo.Text = CText.w_str();
  PluginShowInfo.ImagePath = (PluginPath + "\\\\BirthdayReminder\\\\cake.png").w_str();
  PluginShowInfo.TimeOut = CTimeOut;
  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));

  if(CSoundPlay==1)
  {
	if(FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
	 PlaySound((PluginPath + "\\\\BirthdayReminder\\\\birthday.wav").c_str(), NULL, SND_ASYNC | SND_FILENAME);
  }
}
//---------------------------------------------------------------------------

