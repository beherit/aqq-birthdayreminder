//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop

#include <inifiles.hpp>

#include "SettingsFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;

AnsiString ePluginDirectory;
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
    
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
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

  delete Ini;

  Close();
}
//---------------------------------------------------------------------------



