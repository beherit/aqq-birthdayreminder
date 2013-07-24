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
__declspec(dllimport)void TestChmurki(int TimeOutTest,bool ShowAgeTest, bool PlaySoundTest);
__declspec(dllimport)void ShowBirthdayInfo(UnicodeString CText, int CTimeOut, bool CSoundPlay);
__declspec(dllimport)UnicodeString GetPluginPath();
__declspec(dllimport)UnicodeString GetContactsPath();
__declspec(dllimport)UnicodeString GetContactNick(UnicodeString JID);
__declspec(dllimport)bool AQQSoundOff();
//---------------------------------------------------------------------------
UnicodeString ePluginDirectory;
UnicodeString eContactsPath;
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
  TestChmurki(TimeBox->ItemIndex + 3, AgeCheckBox->Checked, SoundCheckBox->Checked);
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::TimerTimer(TObject *Sender)
{
  ePluginDirectory=GetPluginPath();

  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  int Repeat = Ini->ReadInteger("Settings", "Repeat", 0);
  delete Ini;

  if (Repeat==1) //Raz dziennie
  {
    TDateTime tmpTodey = TDateTime::CurrentDate();
    UnicodeString tmpCurrentDate = tmpTodey;

    TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
    UnicodeString tmpCurrentDay = Ini->ReadString("Settings", "CurrentDay", "");
    delete Ini;

    if(AnsiSameStr(tmpCurrentDay,tmpCurrentDate))
     Timer->Enabled=false;
    else
    {
      aFindContacts->Execute();

      TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
      Ini->WriteString("Settings", "CurrentDay", tmpCurrentDate);
      delete Ini;

	  Timer->Enabled=false;
	}
  }
  else
  {
    aFindContacts->Execute();

    if (Repeat==0) //Przy ka¿dym w³¹czeniu AQQ
     Timer->Enabled=false;
    else //Inne tam
     Timer->Interval = (Repeat-1) * 3600000;
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aFindContactsExecute(TObject *Sender)
{
  NicksList->Clear();
  int index; //Do sprawdzania nikow

  ePluginDirectory=GetPluginPath();
  eContactsPath=GetContactsPath();
  FileListBox->Clear();
  FileListBox->Directory=eContactsPath;

  Word tYear=0,tMonth=0,tDay=0; //Do rozkodowanej aktualnej data
  TDateTime Todey = TDateTime::CurrentDate(); //Pobranie aktualnej daty zakodowanej
  DecodeDate(Todey, tYear, tMonth, tDay); //Rozkodowanie aktualnej daty
  Word WbYear=0,WbMonth=0,WbDay=0; //Do rozkodowanej daty urodzin kontaktu
  UnicodeString AbYear=0,AbMonth=0,AbDay=0; //j.w.
  TDateTime BirthDay; //Data urodzin kontaktu
  UnicodeString BirthDayAnsi; //Data urodzin kontaktu

  //do porównania dat
  double DataAktualna;
  double DataKontaktu;
  int RoznicaDat;

  UnicodeString FileName; //Nazwa pliku ini
  UnicodeString Text; //Tekst na chmurce

  //odczyt ustawien
  TIniFile *Ini = new TIniFile(ePluginDirectory + "\\\\BirthdayReminder\\\\Settings.ini");
  int TimeOut = Ini->ReadInteger("Settings", "TimeOut", 6);
  TimeOut = TimeOut * 1000;
  int AnotherDay = Ini->ReadInteger("Settings", "Another", 0);
  bool SoundPlay = Ini->ReadBool("Settings", "Sound", 1);
  if(SoundPlay==true)
   SoundPlay=AQQSoundOff();
  bool InBirthDay = Ini->ReadBool("Settings", "InBirthDay", 1);
  bool ShowAge = Ini->ReadBool("Settings", "ShowAge", 1);
  delete Ini;

  for(int i=0;i<FileListBox->Items->Count;i++)
  {
    FileListBox->ItemIndex=i;

    FileName = FileListBox->FileName;
    FileName = StringReplace(FileName, "\\", "\\\\", TReplaceFlags() << rfReplaceAll);

	TIniFile *Ini = new TIniFile(FileName);
	BirthDayAnsi = IdDecoderMIME->DecodeString((Ini->ReadString("Buddy", "Birth", "A===").c_str()));
    delete Ini;

    try
    {
	  if(AnsiPos("-",BirthDayAnsi)>0)
	  {
        //Wyciagniecie roku urodzin
		AbYear = BirthDayAnsi;
		AbYear = AbYear.Delete(AnsiPos("-",AbYear),AbYear.Length());

        //Wyciagniecie miesiaca urodzin
		AbMonth = BirthDayAnsi;
        AbMonth = AbMonth.Delete(1,AnsiPos("-",AbMonth));
        AbMonth = AbMonth.Delete(AnsiPos("-",AbMonth),AbMonth.Length());

        //Wyciagniecie dnia urodzin
		AbDay = BirthDayAnsi;
        AbDay = AbDay.Delete(1,AnsiPos("-",AbDay));
		AbDay = AbDay.Delete(1,AnsiPos("-",AbDay));

        //Zakodowanie do domyslnego formatu i odkodowanie
		BirthDay = EncodeDate(StrToInt(AbYear),StrToInt(AbMonth),StrToInt(AbDay));
		DecodeDate(BirthDay,WbYear,WbMonth,WbDay);

        //Porownanie dat
        DataAktualna = EncodeDate(tYear, tMonth, tDay);
        DataKontaktu = EncodeDate(tYear, WbMonth, WbDay);
        RoznicaDat = difftime(DataKontaktu, DataAktualna);

		if((InBirthDay==1)&&(RoznicaDat==0))
        {
		  UnicodeString Nick = GetContactNick(ExtractFileName(FileListBox->FileName).SetLength(ExtractFileName(FileListBox->FileName).Length()-4));

		  int index = NicksList->Items->IndexOf(Nick);
          if(index==-1)
          {
            WbYear = tYear - WbYear;

			Text = Nick + " obchodzi dziœ urodziny!";
            if(ShowAge==1)
			 Text = Text + " (" + WbYear + ")";

			ShowBirthdayInfo(Text, TimeOut, SoundPlay);

			SoundPlay=0;
            NicksList->Items->Add(Nick);
          }
        }

        if((RoznicaDat==1)||(RoznicaDat==2)||(RoznicaDat==3)||(RoznicaDat==4)||(RoznicaDat==5)||(RoznicaDat==6)||(RoznicaDat==7)||(RoznicaDat==8))
        {
          if(AnotherDay!=0)
          {
            UnicodeString Nick = GetContactNick(ExtractFileName(FileListBox->FileName).SetLength(ExtractFileName(FileListBox->FileName).Length()-4));

			int index = NicksList->Items->IndexOf(Nick);
            if(index==-1)
            {
              WbYear = tYear - WbYear;

              if(AnotherDay==1)
              {
				Text = Nick + " obchodzi jutro urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==2)
              {
				Text = Nick + " za dwa dni obchodzi urodziny!";
				if(ShowAge==1)
				 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==3)
              {
				Text = Nick + " za trzy dni obchodzi urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==4)
              {
                Text = Nick + " za cztery dni obchodzi urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==5)
              {
                Text = Nick + " za piêæ dni obchodzi urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==6)
              {
                Text = Nick + " za szeœæ dni obchodzi urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==7)
              {
				Text = Nick + " za tydzieñ obchodzi urodziny!";
				if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }
              if(AnotherDay==8)
              {
                Text = Nick + " za dwa tygodnie obchodzi urodziny!";
                if(ShowAge==1)
                 Text = Text + " (" + WbYear + ")";
			  }

			  if(((RoznicaDat==1)&&(AnotherDay==1))||
			  ((RoznicaDat==2)&&(AnotherDay==2))||
			  ((RoznicaDat==3)&&(AnotherDay==3))||
			  ((RoznicaDat==4)&&(AnotherDay==4))||
			  ((RoznicaDat==5)&&(AnotherDay==5))||
			  ((RoznicaDat==6)&&(AnotherDay==6))||
			  ((RoznicaDat==7)&&(AnotherDay==7))||
			  ((RoznicaDat==14)&&(AnotherDay==8)))
			  {
				ShowBirthdayInfo(Text, TimeOut, SoundPlay);
				SoundPlay=0;
				NicksList->Items->Add(Nick);
			  }
			}
		  }
		}
	  }
	}
	catch (...) { /*b³¹d - nie rób nic*/ }
  }
}
//---------------------------------------------------------------------------

void __fastcall TSettingsForm::aSaveSettingsExecute(TObject *Sender)
{
  ePluginDirectory=GetPluginPath();

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
  ePluginDirectory=GetPluginPath();

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

void __fastcall TSettingsForm::aExitExecute(TObject *Sender)
{
  Close();
}
//---------------------------------------------------------------------------

