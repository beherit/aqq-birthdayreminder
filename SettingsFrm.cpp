//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include <inifiles.hpp>
#include <time.h>
#include "SettingsFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "XPMan"
#pragma link "IdCoderMIME"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void TestChmurki(AnsiString PluginPath,int TimeOutTest,int ShowAgeTest, int PlaySoundTest);
__declspec(dllimport)void ShowBirthdayInfo(AnsiString Text, int TimeOut, AnsiString ImagePath, bool SoundPlay);
__declspec(dllimport)AnsiString GetPluginPath(AnsiString Dir);
__declspec(dllimport)AnsiString GetContactsPath(AnsiString Dir);
__declspec(dllimport)AnsiString GetContactNick(AnsiString JID);
//---------------------------------------------------------------------------
AnsiString ePluginDirectory;
AnsiString eContactsPath;
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
        : TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
  aReadSettings->Execute();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
  aSaveSettings->Execute();
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TestButonClick(TObject *Sender)
{
  ePluginDirectory=GetPluginPath(ePluginDirectory);
  TestChmurki(ePluginDirectory, TimeBox->ItemIndex + 3, AgeCheckBox->Checked, SoundCheckBox->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TajmerTimer(TObject *Sender)
{
  ePluginDirectory=GetPluginPath(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  int Repeat = Ini->ReadInteger("Settings", "Repeat", 0);
  delete Ini;

  if (Repeat==1) //Raz dziennie
  {
    TDateTime tmpTodey = TDateTime::CurrentDate();
    AnsiString tmpCurrentDate = tmpTodey;

    TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
    AnsiString tmpCurrentDay = Ini->ReadString("Settings", "CurrentDay", "");
    delete Ini;

    if(AnsiSameStr(tmpCurrentDay,tmpCurrentDate))
     Tajmer->Enabled=false;
    else
    {
      aFindContacts->Execute();

      TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
      Ini->WriteString("Settings", "CurrentDay", tmpCurrentDate);
      delete Ini;

      Tajmer->Enabled=false;
    }
  }
  else
  {
    aFindContacts->Execute();

    if (Repeat==0) //Przy ka¿dym w³¹czeniu AQQ
     Tajmer->Enabled=false;
    else //Inne tam
     Tajmer->Interval = (Repeat-1) * 3600000;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aFindContactsExecute(TObject *Sender)
{
  NicksList->Clear();
  int index; //Do sprawdzania nikow

  ePluginDirectory=GetPluginPath(ePluginDirectory);
  eContactsPath=GetContactsPath(eContactsPath);
  FileListBox->Clear();
  FileListBox->Directory=eContactsPath;

  Word tYear,tMonth,tDay; //Do rozkodowanej aktualnej data
  TDateTime Todey = TDateTime::CurrentDate();
  AnsiString tCurrentDate = Todey; //Pobranie aktualnej daty zakodowanej
  DecodeDate(tCurrentDate, tYear, tMonth, tDay); //Rozkodowanie daty
  Word bYear=0,bMonth=0,bDay=0; //Do rozkodowanej daty urodzin kontaktu
  AnsiString BirthDay; //Data urodzin kontaktu

  //do porównania dat
  double DataAktualna;
  double DataKontaktu;
  int RoznicaDat=0;

  AnsiString FileName; //Nazwa pliku ini
  
  //odczyt ustawien
  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  int TimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeOut = TimeOut * 1000;
  int AnotherDay = Ini->ReadInteger("Settings", "Another", 0);
  bool SoundPlay = Ini->ReadBool("Settings", "Sound", 1);
  bool InBirthDay = Ini->ReadBool("Settings", "InBirthDay", 1);
  bool ShowAge = Ini->ReadBool("Settings", "ShowAge", 1);
  delete Ini;

  for(int i=0;i<FileListBox->Items->Count;i++)
  {
    FileListBox->ItemIndex=i;

    FileName = FileListBox->FileName;
    FileName = StringReplace(FileName, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

    TIniFile *Ini = new TIniFile(FileName);
    BirthDay = IdDecoderMIME->DecodeToString((Ini->ReadString("Buddy", "Birth", "").c_str()));
    delete Ini;

    try
    {
      DecodeDate(BirthDay, bYear, bMonth, bDay);

      DataAktualna = EncodeDate(tYear, tMonth, tDay);
      DataKontaktu = EncodeDate(tYear, bMonth, bDay);
      RoznicaDat = difftime(DataKontaktu, DataAktualna);
    }
    catch (...) { /*b³¹d - nie rób nic*/ }

    if(InBirthDay==1)
    {
      if((StrToInt(bMonth)==StrToInt(tMonth))&&(StrToInt(bDay)==StrToInt(tDay)))
      {
        AnsiString Nick = GetContactNick(ExtractFileName(FileListBox->FileName).SetLength(ExtractFileName(FileListBox->FileName).Length()-4));

        int index = NicksList->Perform(LB_SELECTSTRING, -1,(LPARAM)Nick.c_str());
        if(index==-1)
        {
          bYear = tYear - bYear;

          AnsiString TextTmp = Nick + " obchodzi dziœ urodziny!";
          if(ShowAge==1)
           TextTmp = TextTmp + " (" + bYear + ")";

          ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          SoundPlay=0;
          NicksList->Items->Add(Nick);
        }
      }
    }

    if((RoznicaDat==1)||(RoznicaDat==2)||(RoznicaDat==3)||(RoznicaDat==4)||(RoznicaDat==5)||(RoznicaDat==6)||(RoznicaDat==7)||(RoznicaDat==8))
    {
      if(AnotherDay!=0)
      {
        AnsiString Nick = GetContactNick(ExtractFileName(FileListBox->FileName).SetLength(ExtractFileName(FileListBox->FileName).Length()-4));

        int index = NicksList->Perform(LB_SELECTSTRING, -1,(LPARAM)Nick.c_str());
        if(index==-1)
        {
          bYear = tYear - bYear;
          AnsiString TextTmp;

          if(AnotherDay==1)
          {
            TextTmp = Nick + " obchodzi jutro urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==2)
          {
            TextTmp = Nick + " za dwa dni obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==3)
          {
            TextTmp = Nick + " za trzy dni obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==4)
          {
            TextTmp = Nick + " za cztery dni obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==5)
          {
            TextTmp = Nick + " za piêæ dni obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==6)
          {
            TextTmp = Nick + " za szeœæ dni obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==7)
          {
            TextTmp = Nick + " za tydzieñ obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if(AnotherDay==8)
          {
            TextTmp = Nick + " za dwa tygodnie obchodzi urodziny!";
            if(ShowAge==1)
             TextTmp = TextTmp + " (" + bYear + ")";
          }

          if((RoznicaDat==1)&&(AnotherDay==1))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==2)&&(AnotherDay==2))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==3)&&(AnotherDay==3))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==4)&&(AnotherDay==4))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==5)&&(AnotherDay==5))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==6)&&(AnotherDay==6))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==7)&&(AnotherDay==7))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          else if((RoznicaDat==14)&&(AnotherDay==8))
           ShowBirthdayInfo(TextTmp, TimeOut, ePluginDirectory, SoundPlay);

          SoundPlay=0;
          NicksList->Items->Add(Nick);
        }
      }
    }
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
  ePluginDirectory=GetPluginPath(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");

  Ini->WriteInteger("Settings", "TimeOut", TimeBox->ItemIndex + 3);

  Ini->WriteBool("Settings", "Sound", SoundCheckBox->Checked);
  Ini->WriteBool("Settings", "InBirthDay", BirthDayCheckBox->Checked);
  Ini->WriteBool("Settings", "ShowAge", AgeCheckBox->Checked);

  Ini->WriteInteger("Settings", "Another", AnotherDayBox->ItemIndex);

  Ini->WriteInteger("Settings", "Repeat", RepeatCheckBox->ItemIndex);

  delete Ini;        
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aReadSettingsExecute(TObject *Sender)
{
  ePluginDirectory=GetPluginPath(ePluginDirectory);

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");

  int eTimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeBox->ItemIndex=eTimeOut - 3;

  SoundCheckBox->Checked=Ini->ReadBool("Settings", "Sound", 1);
  BirthDayCheckBox->Checked=Ini->ReadBool("Settings", "InBirthDay", 1);
  AgeCheckBox->Checked=Ini->ReadBool("Settings", "ShowAge", 1);

  AnotherDayBox->ItemIndex = Ini->ReadInteger("Settings", "Another", 0);

  RepeatCheckBox->ItemIndex = Ini->ReadInteger("Settings", "Repeat", 0);

  delete Ini;
}
//---------------------------------------------------------------------------

