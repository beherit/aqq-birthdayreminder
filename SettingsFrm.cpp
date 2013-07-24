//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>

#include "SettingsFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)void TestChmurki(int TimeOutTest,int ShowAgeTest, int PlaySoundTest);
__declspec(dllimport)void FindContacts(String Dir, String typ);
__declspec(dllimport)void ZmianaUstawien(int eTimeOut, int eSoundPlay, int eAnotherDay, int eInBirthDay, int eShowAge);
__declspec(dllimport)void ResetDzwieku();
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
  ePluginDirectory = setPluginPath;

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  int eTimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeBox->ItemIndex=eTimeOut - 3;

  int Sounds = Ini->ReadInteger("Settings", "Sound", 1);
  if (Sounds==1)
    SoundCheckBox->Checked=true;
  if (Sounds==0)
    SoundCheckBox->Checked=false;

  int InBirthDay = Ini->ReadInteger("Settings", "InBirthDay", 1);
  if (InBirthDay==1)
    BirthDayCheckBox->Checked=true;
  if (InBirthDay==0)
    BirthDayCheckBox->Checked=false;

  int ShowAge = Ini->ReadInteger("Settings", "ShowAge", 1);
  if (ShowAge==1)
    AgeCheckBox->Checked=true;
  if (ShowAge==0)
    AgeCheckBox->Checked=false;

  AnotherDayBox->ItemIndex = Ini->ReadInteger("Settings", "Another", 0);

  RepeatCheckBox->ItemIndex = Ini->ReadInteger("Settings", "Repeat", 0);

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
  ePluginDirectory = setPluginPath;
  
  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");

  Ini->WriteInteger("Settings", "TimeOut", TimeBox->ItemIndex + 3);

  if (SoundCheckBox->Checked==true)
   Ini->WriteInteger("Settings", "Sound", 1);
  if (SoundCheckBox->Checked==false)
   Ini->WriteInteger("Settings", "Sound", 0);

  if (BirthDayCheckBox->Checked==true)
   Ini->WriteInteger("Settings", "InBirthDay", 1);
  if (BirthDayCheckBox->Checked==false)
   Ini->WriteInteger("Settings", "InBirthDay", 0);

  if (AgeCheckBox->Checked==true)
   Ini->WriteInteger("Settings", "ShowAge", 1);
  if (AgeCheckBox->Checked==false)
   Ini->WriteInteger("Settings", "ShowAge", 0);

  Ini->WriteInteger("Settings", "Another", AnotherDayBox->ItemIndex);

  Ini->WriteInteger("Settings", "Repeat", RepeatCheckBox->ItemIndex);

  delete Ini;

  ZmianaUstawien((TimeBox->ItemIndex + 3), (SoundCheckBox->Checked), (AnotherDayBox->ItemIndex), (BirthDayCheckBox->Checked), (AgeCheckBox->Checked));

  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TestButonClick(TObject *Sender)
{
 int TimeOutTest = TimeBox->ItemIndex + 3;
 int ShowAgeTest = AgeCheckBox->Checked;
 int PlaySoundTest = SoundCheckBox->Checked;
 TestChmurki(TimeOutTest, ShowAgeTest, PlaySoundTest);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TajmerTimer(TObject *Sender)
{
 eContactsPath = setContactsPath;
 ePluginDirectory = setPluginPath;
 
 FindContacts(eContactsPath, "ini");

 TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");

 int Repeat = Ini->ReadInteger("Settings", "Repeat", 0);

 if (Repeat==0)
  Tajmer->Enabled=false;

 else
  Tajmer->Interval = Repeat * 3600000;

 delete Ini;
 
 ResetDzwieku();
}
//---------------------------------------------------------------------------

