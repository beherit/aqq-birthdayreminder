//---------------------------------------------------------------------------
#include <vcl.h>
#include <windows.h>
#pragma hdrstop
#pragma argsused
#include <System.hpp> //do Base64
#include <string> //do Base64
#include <filectrl.hpp> //do szukania plików
#include <inifiles.hpp> //do plików ini
#include <memory> //do RES
#include "ikonka.rh" //do png
#include <mmsystem.h> //do wav
#include "Aqq.h"
#include "SettingsFrm.h"
using std::string; //do Base64
//---------------------------------------------------------------------------

HINSTANCE hInstance; //uchwyt do wtyczki

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  hInstance = hinst;
  return 1;
}

wchar_t *AnsiTowchar_t(AnsiString Str) //zamiana AnsiString->wchar_t*
{
  wchar_t *str = new wchar_t[Str.WideCharBufSize()];
  return Str.WideChar(str, Str.WideCharBufSize());
}

TSettingsForm *handle; //tworzenie uchwytu do formy

//utworzenie obiektow do struktur
PluginAction TPluginActionSkrot;
PluginShowInfo TPluginShowInfo;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

//zmienne globalne
AnsiString UserPath;
AnsiString PluginPath;
AnsiString ContactsPath;
AnsiString ImagePathTmp;
AnsiString BirthDay;
TDateTime Todey = TDateTime::CurrentDate();
AnsiString tmpCurrentDate = Todey;
AnsiString CurrentDate = tmpCurrentDate.Delete(1,5);
AnsiString Year_Current = CurrentYear();
AnsiString Year_Birth;
int Song=0;
int TimeOut;
int plugin_icon_idx;

//serwis szybkiego dostêpu
int __stdcall BirthdayReminderSettingsService (WPARAM, LPARAM)
{
  if (handle==NULL)
  {
  Application->Handle = SettingsForm;
  handle = new TSettingsForm(Application);
  handle->setPluginPath=PluginPath;
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}

//Base64---------------------------------------------------------------------
const char          fillchar = '=';

                        // 00000000001111111111222222
                        // 01234567890123456789012345

static string       cvt = "ABCDEFGHIJKLMNOPQRSTUVWXYZ"

                        // 22223333333333444444444455
                        // 67890123456789012345678901
                          "abcdefghijklmnopqrstuvwxyz"

                        // 555555556666
                        // 234567890123
                          "0123456789+/";

AnsiString Base64Decode(string data)
{
    auto     string::size_type  i;
    auto     char               c;
    auto     char               c1;
    auto     string::size_type  len = data.length();
    auto     AnsiString             ret;

    for (i = 0; i < len; ++i)
    {
        c = (char) cvt.find(data[i]);
        ++i;
        c1 = (char) cvt.find(data[i]);
        c = (c << 2) | ((c1 >> 4) & 0x3);
        ret += c;
        if (++i < len)
        {
            c = data[i];
            if (fillchar == c)
                break;

            c = (char) cvt.find(c);
            c1 = ((c1 << 4) & 0xf0) | ((c >> 2) & 0xf);
            ret+=c1;
        }

        if (++i < len)
        {
            c1 = data[i];
            if (fillchar == c1)
                break;

            c1 = (char) cvt.find(c1);
            c = ((c << 6) & 0xc0) | c1;
            ret+=c;
        }
    }

    return(ret);
}
//---------------------------------------------------------------------------

void FindContacts(String Dir, String typ)
{
  TSearchRec sr;

  if(FindFirst(Dir + "*.*", faAnyFile, sr) == 0)
  {
    do
    {
      if(((sr.Attr & faDirectory) > 0) && (sr.Name != ".") && (sr.Name != ".."))
      {
        FindContacts(Dir + sr.Name + "\\", typ);
      }
      if((sr.Attr & faDirectory) == 0)
      {
        if(typ.IsEmpty())
        {
          // tu te¿? O.o
        }
        else if(ExtractFileExt(sr.Name).SubString(2, 5) == typ)
        {
          TIniFile *Ini = new TIniFile(Dir + sr.Name);
          BirthDay = Base64Decode((Ini->ReadString("Buddy", "Birth", "").c_str()));
          Year_Birth = BirthDay;
          Year_Birth = Year_Birth.SetLength(Year_Birth.Length()-6);
          BirthDay = BirthDay.Delete(1,5);
          if(AnsiSameStr(BirthDay, CurrentDate))
          {
            AnsiString Nick = Base64Decode((Ini->ReadString("Buddy", "Nick", "").c_str()));

            StrToInt(Year_Birth);
            StrToInt(Year_Current);
            Year_Birth = Year_Current - Year_Birth;

            AnsiString TextTmp = Nick + " obchodzi dziœ urodziny! (" + Year_Birth + ")";
            
            wchar_t* Text = AnsiTowchar_t(TextTmp);
            wchar_t* ImagePath = AnsiTowchar_t(ImagePathTmp);
            
            TPluginShowInfo.cbSize = sizeof(PluginShowInfo);
            TPluginShowInfo.Event = tmeInfo;
            TPluginShowInfo.Text = Text;
            TPluginShowInfo.ImagePath = ImagePath;
            TPluginShowInfo.TimeOut = TimeOut;
            //TPluginShowInfo.ActionID;
            //TPluginShowInfo.Tick; 
            TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

            if(Song==0)
            {
              if(FileExists(PluginPath + "\\\\BirthdayReminder\\\\birthday.wav"))
              {
                AnsiString SoundPatch = PluginPath + "\\\\BirthdayReminder\\\\birthday.wav";
                sndPlaySound(SoundPatch.c_str(), SND_SYNC);
                Song=1;
              }
              else
              {
                PlaySound("ID_SONG1", HInstance, SND_ASYNC | SND_RESOURCE);
                Song=1;
              }
            }
          }
          delete Ini;
        }
      }
    }
    while(FindNext(sr) == 0);
    FindClose(sr);
  }
}

//Program
extern "C"  __declspec(dllexport) PluginInfo* __stdcall AQQPluginInfo(DWORD AQQVersion)
{
  TPluginInfo.cbSize = sizeof(PluginInfo);
  TPluginInfo.ShortName = (wchar_t *)L"Birthday Reminder";
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,0,0);
  TPluginInfo.Description = (wchar_t *)L"Powiadomienie przypominaj¹ce o urodzinach kontaktów";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Prawa zastrze¿one, tylko dla autora.";
  TPluginInfo.Homepage = (wchar_t *)L"Brak";

  return &TPluginInfo;
}

void PrzypiszSkrotMenu()
{
  TPluginActionSkrot.cbSize = sizeof(PluginAction);
  TPluginActionSkrot.pszCaption = (wchar_t*) L"Birthday Reminder";
  TPluginActionSkrot.Position = 0;
  TPluginActionSkrot.IconIndex = plugin_icon_idx;
  TPluginActionSkrot.pszService = (wchar_t*) L"serwis_BirthdayReminderSettingsService";
  TPluginActionSkrot.pszPopupName = (wchar_t*) L"popPlugins";
  TPluginActionSkrot.PopupPosition = 0;

  TPluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)(&TPluginActionSkrot));
  TPluginLink.CreateServiceFunction(L"serwis_BirthdayReminderSettingsService",BirthdayReminderSettingsService);
}

int __stdcall OnModulesLoaded(WPARAM, LPARAM)
{
  FindContacts(ContactsPath, "ini");
  
  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Load(PluginLink *Link)
{
  TPluginLink = *Link;

  UserPath = (wchar_t *)(TPluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(hInstance),0));
  UserPath = StringReplace(UserPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
  PluginPath = (wchar_t *)(TPluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,(WPARAM)(hInstance),0));
  PluginPath = StringReplace(PluginPath, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

  ContactsPath = UserPath + "\\\\Data\\\\Contacts\\\\";
  ImagePathTmp = PluginPath + "\\\\BirthdayReminder\\\\cake.png";

  //Wypakowanie ikony
  HRSRC rsrc = FindResource(HInstance, MAKEINTRESOURCE(ID_PNG), RT_RCDATA);

  DWORD Size = SizeofResource(HInstance, rsrc);
  HGLOBAL MemoryHandle = LoadResource(HInstance, rsrc);

  if(MemoryHandle == NULL) return 0;

  BYTE *MemPtr = (BYTE *)LockResource(MemoryHandle);

  std::auto_ptr<TMemoryStream>stream(new TMemoryStream);
  stream->Write(MemPtr, Size);
  stream->Position = 0;
  if(!DirectoryExists(PluginPath + "\\\\BirthdayReminder"))
   CreateDir(PluginPath + "\\\\BirthdayReminder");
  if(!FileExists(PluginPath + "\\\\BirthdayReminder\\\\cake.png"))
   stream->SaveToFile(PluginPath + "\\\\BirthdayReminder\\\\cake.png");
  stream->SaveToFile("cake.png");
  //Wypakowanie ikony - Koniec

  TIniFile *Ini = new TIniFile(PluginPath + "\\\\BirthdayReminder\\\\Settings.ini");
  TimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeOut = TimeOut * 1000;

  //Przypisanie ikony
  wchar_t* plugin_icon_path = L"cake.png";
  plugin_icon_idx=TPluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(plugin_icon_path));

  //Usuniecie ikony
  DeleteFile("cake.png");

  PrzypiszSkrotMenu();

  TPluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED, OnModulesLoaded);

  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  TPluginLink.DestroyServiceFunction(BirthdayReminderSettingsService);
  TPluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)(plugin_icon_idx));
  TPluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)(&TPluginActionSkrot));
  TPluginLink.UnhookEvent(&OnModulesLoaded);

  return 0;
}

extern "C" int __declspec(dllexport)__stdcall Settings()
{
  if (handle==NULL)
  {
  Application->Handle = SettingsForm;
  handle = new TSettingsForm(Application);
  handle->setPluginPath=PluginPath;
  handle->Show();
  }
  else
    handle->Show();

  return 0;
}
