//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <memory> //do RES
#include "ikonka.rh" //do png
#include <mmsystem.h> //do wav
#include "Aqq.h"
#include "SettingsFrm.h"
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}
//---------------------------------------------------------------------------

//zamiana AnsiString->wchar_t*
wchar_t* AnsiTowchar_t(AnsiString Str)
{                                 
  const char* Text = Str.c_str();
  int size = MultiByteToWideChar(GetACP(), 0, Text, -1, 0,0);
  wchar_t* wbuffer = new wchar_t[size+1];

  MultiByteToWideChar(GetACP(), 0, Text, -1, wbuffer, size+1);

  return wbuffer;
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,6,2);
  PluginInfo.Description = (wchar_t *)L"Wtyczka przypomina o urodzinach kontaktów";
  PluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  PluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = (wchar_t *)L"";

  return &PluginInfo;
}
//---------------------------------------------------------------------------

//Do wypakowywania RES
void ExtractExe(unsigned short ID, AnsiString FileName)
{
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;

  TMemoryStream *Ms = new TMemoryStream;
  Ms->Position = 0;
  Ms->LoadFromStream(stream.get());
  Ms->Position = 0;
  Ms->SaveToFile(FileName);
  Ms->Free();
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  PluginLink = *Link;

  AnsiString PluginPath = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  //Wypakowanie ikony
  if(!DirectoryExists(PluginPath + "\\\\BirthdayReminder"))
   CreateDir(PluginPath + "\\\\BirthdayReminder");
  if(!FileExists(PluginPath + "\\\\BirthdayReminder\\\\cake.png"))
   ExtractExe(ID_PNG,PluginPath+ "\\\\BirthdayReminder\\\\cake.png");
  //Wypakowanie ikony - Koniec

  //Uruchomienie timer'a
  Application->Handle = SettingsForm;
  handle = new TSettingsForm(Application);
  handle->Tajmer->Enabled=true;
  handle->Close();

  return 0;
}
//---------------------------------------------------------------------------

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if (handle==NULL)
  {
    Application->Handle = SettingsForm;
    handle = new TSettingsForm(Application);
    handle->Show();
  }
  else
   handle->Show();

  return 0;
}
//---------------------------------------------------------------------------

void TestChmurki(AnsiString PluginPath, int TimeOutTest, int ShowAgeTest, int PlaySoundTest)
{
  wchar_t* Text;
  
  if(ShowAgeTest==0)
   Text = L"Jan Kowalski obchodzi dziœ urodziny!";
  else
   Text = L"Jan Kowalski obchodzi dziœ urodziny! (36)";

  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
  PluginShowInfo.Event = tmeInfo;
  PluginShowInfo.Text = Text;
  PluginShowInfo.ImagePath = AnsiTowchar_t(PluginPath + "\\\\BirthdayReminder\\\\cake.png");
  PluginShowInfo.TimeOut = 1000 * TimeOutTest;

  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));

  if(PlaySoundTest==1)
  {
    if(FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
    {
      AnsiString SoundPatch = PluginPath + "\\\\BirthdayReminder\\\\birthday.wav";
      sndPlaySound(SoundPatch.c_str(), SND_SYNC);
    }
    else
    {
      PlaySound("ID_SONG1", HInstance, SND_ASYNC | SND_RESOURCE);
    }
  }
}
//---------------------------------------------------------------------------

AnsiString GetPluginPath(AnsiString Dir)
{
  Dir = (wchar_t*)(PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  Dir = StringReplace(Dir, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  return Dir;
}
//---------------------------------------------------------------------------

AnsiString GetContactsPath(AnsiString Dir)
{
  Dir = (wchar_t *)(PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(hInstance),0));
  Dir = Dir + "\\Data\\Contacts\\\\";
  return Dir;
}
//---------------------------------------------------------------------------

AnsiString GetContactNick(AnsiString JID)
{
  TPluginContactSimpleInfo PluginContactSimpleInfo;
  PluginContactSimpleInfo.cbSize = sizeof(TPluginContactSimpleInfo);
  PluginContactSimpleInfo.JID = AnsiTowchar_t(JID);
  PluginLink.CallService(AQQ_CONTACTS_FILLSIMPLEINFO,0,(LPARAM)(&PluginContactSimpleInfo));
  JID = (wchar_t*)(PluginContactSimpleInfo.Nick);
  return JID;
}
//---------------------------------------------------------------------------

void ShowBirthdayInfo(AnsiString Text, int TimeOut, AnsiString PluginPath, bool SoundPlay)
{
  PluginShowInfo.cbSize = sizeof(TPluginShowInfo);
  PluginShowInfo.Event = tmeInfo;
  PluginShowInfo.Text = AnsiTowchar_t(Text);
  PluginShowInfo.ImagePath = AnsiTowchar_t(PluginPath + "\\\\BirthdayReminder\\\\cake.png");
  PluginShowInfo.TimeOut = TimeOut;
  PluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&PluginShowInfo));

  if(SoundPlay==1)
  {
    if(FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
    {
      AnsiString SoundPatch = PluginPath + "\\\\BirthdayReminder\\\\birthday.wav";
      sndPlaySound(SoundPatch.c_str(), SND_SYNC);
    }
    else
     PlaySound("ID_SONG1", HInstance, SND_ASYNC | SND_RESOURCE);
  } 
}
//---------------------------------------------------------------------------
