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
__declspec(dllimport)bool ChkThemeAnimateWindows();
__declspec(dllimport)bool ChkThemeGlowing();
__declspec(dllimport)void LoadSettings();
//---------------------------------------------------------------------------

__fastcall TSettingsForm::TSettingsForm(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormCreate(TObject *Sender)
{
  //Wlaczona zaawansowana stylizacja okien
  if(ChkSkinEnabled())
  {
	UnicodeString ThemeSkinDir = GetThemeSkinDir();
	//Plik zaawansowanej stylizacji okien istnieje
	if(FileExists(ThemeSkinDir + "\\\\Skin.asz"))
	{
	  ThemeSkinDir = StringReplace(ThemeSkinDir, "\\\\", "\\", TReplaceFlags() << rfReplaceAll);
	  sSkinManager->SkinDirectory = ThemeSkinDir;
	  sSkinManager->SkinName = "Skin.asz";
	  if(ChkThemeAnimateWindows()) sSkinManager->AnimEffects->FormShow->Time = 200;
	  else sSkinManager->AnimEffects->FormShow->Time = 0;
	  sSkinManager->Effects->AllowGlowing = ChkThemeGlowing();
	  sSkinManager->Active = true;
	}
	//Brak pliku zaawansowanej stylizacji okien
	else sSkinManager->Active = false;
  }
  //Zaawansowana stylizacja okien wylaczona
  else sSkinManager->Active = false;
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::FormShow(TObject *Sender)
{
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
