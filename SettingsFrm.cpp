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

  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OkButtonClick(TObject *Sender)
{
  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  Ini->WriteInteger("Settings", "TimeOut", TimeBox->ItemIndex + 3);

  delete Ini;

  Close();
}
//---------------------------------------------------------------------------

