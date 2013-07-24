//---------------------------------------------------------------------------
#include <vcl.h>
#pragma hdrstop
#include "SettingsFrm.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma link "acPNG"
#pragma link "sBevel"
#pragma link "sButton"
#pragma link "sCheckBox"
#pragma link "sComboBox"
#pragma link "sSkinManager"
#pragma link "sSkinProvider"
#pragma resource "*.dfm"
TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
__declspec(dllimport)UnicodeString GetPluginUserDir();
__declspec(dllimport)UnicodeString GetThemeSkinDir();
__declspec(dllimport)bool ChkSkinEnabled();
__declspec(dllimport)bool ChkNativeEnabled();
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = true;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
  //Skorkowanie okna
  if(!ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	if((FileExists(ThemeSkinDir + "\\\\Skin.asz"))&&(!ChkNativeEnabled()))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  sSkinProvider->DrawNonClientArea = false;
	  sSkinManager->Active = true;
	}
	else
	 sSkinManager->Active = false;
  }
  //Odczyt ustawien
  TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
  //Powiadomienie w dniu urodzin
  InBirthDayCheckBox->Checked = Ini->ReadBool("Settings","InBirthDay",true);
  //Informowanie rowniez w innym okresie
  AnotherDayComboBox->ItemIndex = Ini->ReadInteger("Settings","Another",0);
  delete Ini;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
  //Odzaczenie typu zamkniecia formy
  AddSource = false;
  //Zamkniecie formy
  Close();
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::OKButtonClick(TObject *Sender)
{
  //Zapisanie nowych ustawien
  TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
  //Powiadomienie w dniu urodzin
  Ini->WriteBool("Settings","InBirthDay",InBirthDayCheckBox->Checked);
  //Informowanie rowniez w innym okresie
  Ini->WriteInteger("Settings","Another",AnotherDayComboBox->ItemIndex);
  delete Ini;
  //Odczyt ustawien w rdzeniu wtyczki
  LoadSettings();
  //Odzaczenie typu zamkniecia formy
  AddSource = true;
  //Zamkniecie formy
  Close();
}
//---------------------------------------------------------------------------

