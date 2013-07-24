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

#include <time.h>

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
PluginShowInfo TPluginShowInfo;
PluginLink TPluginLink;
PluginInfo TPluginInfo;

//zmienne globalne
AnsiString UserPath;
AnsiString PluginPath;
AnsiString ContactsPath;
AnsiString ImagePathTmp;

TDateTime Todey = TDateTime::CurrentDate();
AnsiString tCurrentDate = Todey;
Word tYear,tMonth,tDay;
Word bYear=0,bMonth=0,bDay=0;
AnsiString BirthDay;
AnsiString Nick;

//do porównania dat
double DataAktualna;
double DataKontaktu;
int RoznicaDat=0;
int RoznicaDatUstawienia;

int Song=0;
int TimeOut;
int SoundPlay;
int AnotherDay;
int InBirthDay;
int ShowAge;

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

void Base64Repair() //poprawa BASE64
{
  Nick = StringReplace(Nick, "Ä™", "ê", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ã³", "ó", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ä…", "¹", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å›", "œ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å‚", "³", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å¼", "¿", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Åº", "Ÿ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ä‡", "æ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å„", "ñ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ä˜", "Ê", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ã“", "Ó", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ä„", "¥", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Åš", "Œ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å", "£", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å»", "¯", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Å¹", "", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Ä†", "Æ", TReplaceFlags() << rfReplaceAll);
  Nick = StringReplace(Nick, "Åƒ", "Ñ", TReplaceFlags() << rfReplaceAll);
}

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
          BirthDay = Base64Decode((Ini->ReadString("Buddy", "Birth", "nic").c_str()));

          try
          {
            DecodeDate(BirthDay, bYear, bMonth, bDay);
          }
          catch (...)
          {
            //b³¹d - nie rób nic
          }

          int RoznicaOk=1;
          try
          {
            DataAktualna = EncodeDate(tYear, tMonth, tDay);
            DataKontaktu = EncodeDate(tYear, bMonth, bDay);

            RoznicaDat = difftime(DataAktualna, DataKontaktu);
          }
          catch (...)
          {
            //b³¹d?
            RoznicaOk=0;
          }

          if (InBirthDay==1)
          {
            if((StrToInt(bMonth)==StrToInt(tMonth))&&(StrToInt(bDay)==StrToInt(tDay)))
            {
              Nick = Base64Decode((Ini->ReadString("Buddy", "Nick", "").c_str()));
              Base64Repair();

              bYear = tYear - bYear;

              AnsiString TextTmp = Nick + " obchodzi dziœ urodziny!";
              if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";

              wchar_t* Text = AnsiTowchar_t(TextTmp);
              wchar_t* ImagePath = AnsiTowchar_t(ImagePathTmp);

              TPluginShowInfo.cbSize = sizeof(PluginShowInfo);
              TPluginShowInfo.Event = tmeInfo;
              TPluginShowInfo.Text = Text;
              TPluginShowInfo.ImagePath = ImagePath;
              TPluginShowInfo.TimeOut = TimeOut;
              TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              if (SoundPlay==1)
              {
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
            }
          }

          if (AnotherDay!=0)
          {
            if(RoznicaOk==1)
            {
              Nick = Base64Decode((Ini->ReadString("Buddy", "Nick", "").c_str()));
              Base64Repair();
              
              bYear = tYear - bYear;
              AnsiString TextTmp;
              int eSong=0;

              if (AnotherDay==1)
              {
                TextTmp = Nick + " obchodzi jutro urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==2)
              {
                TextTmp = Nick + " za dwa dni obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==3)
              {
                TextTmp = Nick + " za trzy dni obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==4)
              {
                TextTmp = Nick + " za cztery dni obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==5)
              {
                TextTmp = Nick + " za piêæ dni obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==6)
              {
                TextTmp = Nick + " za szeœæ dni obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==7)
              {
                TextTmp = Nick + " za tydzieñ obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              if (AnotherDay==8)
              {
                TextTmp = Nick + " za dwa tygodnie obchodzi urodziny!";
                if(ShowAge==1)
                TextTmp = TextTmp + " (" + bYear + ")";
              }

              wchar_t* Text = AnsiTowchar_t(TextTmp);
              wchar_t* ImagePath = AnsiTowchar_t(ImagePathTmp);

              TPluginShowInfo.cbSize = sizeof(PluginShowInfo);
              TPluginShowInfo.Event = tmeInfo;
              TPluginShowInfo.Text = Text;
              TPluginShowInfo.ImagePath = ImagePath;
              TPluginShowInfo.TimeOut = TimeOut;

              if ((RoznicaDat==-1)&&(AnotherDay==1))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-2)&&(AnotherDay==2))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-3)&&(AnotherDay==3))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-4)&&(AnotherDay==4))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-5)&&(AnotherDay==5))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-6)&&(AnotherDay==6))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-7)&&(AnotherDay==7))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else if ((RoznicaDat==-14)&&(AnotherDay==8))
                TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

              else eSong=1;

              if (SoundPlay==1)
              {
                if(eSong==0)
                {
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
              }
            }
          }

          bYear=0,bMonth=0,bDay=0;
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
  TPluginInfo.Version = PLUGIN_MAKE_VERSION(1,0,5,2);
  TPluginInfo.Description = (wchar_t *)L"Wtyczka przypomina o urodzinach kontaktów";
  TPluginInfo.Author = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.AuthorMail = (wchar_t *)L"beherit666@vp.pl";
  TPluginInfo.Copyright = (wchar_t *)L"Krzysztof Grochocki (Beherit)";
  TPluginInfo.Homepage = (wchar_t *)L"";

  return &TPluginInfo;
}

void SprawdzUrodziny()
{
  Application->Handle = SettingsForm;
  handle = new TSettingsForm(Application);
  handle->setContactsPath=ContactsPath;
  handle->setPluginPath=PluginPath;
  handle->Tajmer->Enabled=true;
  handle->Close();
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
  //Wypakowanie ikony - Koniec

  //odczyt ustawien
  TIniFile *Ini = new TIniFile(PluginPath + "\\\\BirthdayReminder\\\\Settings.ini");
  TimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeOut = TimeOut * 1000;
  SoundPlay = Ini->ReadInteger("Settings", "Sound", 1);
  AnotherDay = Ini->ReadInteger("Settings", "Another", 0);
  InBirthDay = Ini->ReadInteger("Settings", "InBirthDay", 1);
  ShowAge = Ini->ReadInteger("Settings", "ShowAge", 1);

  //Data
  DecodeDate(tCurrentDate, tYear, tMonth, tDay);

  SprawdzUrodziny();

  return 0;
}

extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //TPluginLink.UnhookEvent(&OnModulesLoaded);

  return 0;
}

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

void ResetDzwieku()
{
  Song=0;
}

void ZmianaUstawien(int eTimeOut, int eSoundPlay, int eAnotherDay, int eInBirthDay, int eShowAge)
{
  TimeOut=eTimeOut * 1000;
  SoundPlay=eSoundPlay;
  AnotherDay=eAnotherDay;
  InBirthDay=eInBirthDay;
  ShowAge=eShowAge;
}

void TestChmurki(int TimeOutTest, int ShowAgeTest, int PlaySoundTest)
{
  AnsiString TextTmp = "Infobot obchodzi dziœ urodziny!";
  if(ShowAgeTest==1)
    TextTmp = TextTmp + " (3)";
  wchar_t* Text = AnsiTowchar_t(TextTmp);

  wchar_t* ImagePath = AnsiTowchar_t(ImagePathTmp);

  TPluginShowInfo.cbSize = sizeof(PluginShowInfo);
  TPluginShowInfo.Event = tmeInfo;
  TPluginShowInfo.Text = Text;
  TPluginShowInfo.ImagePath = ImagePath;
  TPluginShowInfo.TimeOut = 1000 * TimeOutTest;

  TPluginLink.CallService(AQQ_FUNCTION_SHOWINFO,0,(LPARAM)(&TPluginShowInfo));

  if(PlaySoundTest==1)
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
