//---------------------------------------------------------------------------
// Copyright (C) 2009-2015 Krzysztof Grochocki
//
// This file is part of Birthday Reminder
//
// Birthday Reminder is free software; you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation; either version 3, or (at your option)
// any later version.
//
// Birthday Reminder is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with GNU Radio; see the file COPYING. If not, write to
// the Free Software Foundation, Inc., 51 Franklin Street,
// Boston, MA 02110-1301, USA.
//---------------------------------------------------------------------------

#include <vcl.h>
#include <windows.h>
#include <inifiles.hpp>
#include <time.h>
#include <IdHashMessageDigest.hpp>
#include <PluginAPI.h>
#include <LangAPI.hpp>
#pragma hdrstop
#include "SettingsFrm.h"

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
	return 1;
}
//---------------------------------------------------------------------------

//STRUKTURY-GLOWNE-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
//STALE-IDENTYFIKUJACE-WTYCZKE-W-CENTRUM-POWIADOMIEN-------------------------
#define NEWS_BIRTHDAYREMINDER L"News/BirthdayReminder"
#define NEWS_BIRTHDAYREMINDER_SOURCE L"News/BirthdayReminder/Source"
//IKONY-W-INTERFEJSIE--------------------------------------------------------
int GITF;
//UCHWYTY-DO-OKIEN-----------------------------------------------------------
HWND hFrmSourceAdd;
//ID-WYWOLANIA-ENUMERACJI-LISTY-KONTAKTOW------------------------------------
DWORD ReplyListID = 0;
//LISTA-KONTAKTOW-&-LISTA-NICKOW---------------------------------------------
TStringList* ContactList = new TStringList;
TCustomIniFile* ContactsNickList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
TMemIniFile* SiblingsList = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
TStringList* SourceContactsList = new TStringList;
//GDY-ZOSTALO-URUCHOMIONE-WYLADOWANIE-WTYCZKI-WRAZ-Z-ZAMKNIECIEM-KOMUNIKATORA
bool ForceUnloadExecuted = false;
//SETTINGS-------------------------------------------------------------------
bool SourceAddedChk;
bool SourceActiveChk;
bool InBirthDayChk;
int AnotherDayChk;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
INT_PTR __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnNewsActive(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnNewsDelete(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnNewsFetch(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam);
INT_PTR __stdcall ServiceBirthdayReminderAddSource(WPARAM wParam, LPARAM lParam);
//FORWARD-OTHER-FUNCTION-----------------------------------------------------
void LoadSettings();
//---------------------------------------------------------------------------

//Pobieranie sciezki katalogu prywatnego wtyczek
UnicodeString GetPluginUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETPLUGINUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezko do katalogu zawierajacego informacje o kontaktach
UnicodeString GetContactsUserDir()
{
	return StringReplace((wchar_t *)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Data\\\\Contacts\\\\";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do katalogu profilu
UnicodeString GetUserDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETUSERDIR,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do katalogu aplikacji
UnicodeString GetApplicationPath()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETAPPPATH,(WPARAM)(HInstance),0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do skorki kompozycji
UnicodeString GetThemeSkinDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll) + "\\\\Skin";
}
//---------------------------------------------------------------------------

//Pobieranie sciezki do kompozycji
UnicodeString GetThemeDir()
{
	return StringReplace((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETTHEMEDIR,0,0), "\\", "\\\\", TReplaceFlags() << rfReplaceAll);
}
//---------------------------------------------------------------------------

//Sprawdzanie czy wlaczona jest zaawansowana stylizacja okien
bool ChkSkinEnabled()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString SkinsEnabled = Settings->ReadString("Settings","UseSkin","1");
	delete Settings;
	return StrToBool(SkinsEnabled);
}
//---------------------------------------------------------------------------

//Sprawdzanie ustawien animacji AlphaControls
bool ChkThemeAnimateWindows()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString AnimateWindowsEnabled = Settings->ReadString("Theme","ThemeAnimateWindows","1");
	delete Settings;
	return StrToBool(AnimateWindowsEnabled);
}
//---------------------------------------------------------------------------
bool ChkThemeGlowing()
{
	TStrings* IniList = new TStringList();
	IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
	TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
	Settings->SetStrings(IniList);
	delete IniList;
	UnicodeString GlowingEnabled = Settings->ReadString("Theme","ThemeGlowing","1");
	delete Settings;
	return StrToBool(GlowingEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie ustawien koloru AlphaControls
int GetHUE()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETHUE,0,0);
}
//---------------------------------------------------------------------------
int GetSaturation()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETSATURATION,0,0);
}
//---------------------------------------------------------------------------
int GetBrightness()
{
	return (int)PluginLink.CallService(AQQ_SYSTEM_COLORGETBRIGHTNESS,0,0);
}
//---------------------------------------------------------------------------

//Dekodowanie ciagu znakow z Base64
UnicodeString DecodeBase64(UnicodeString Str)
{
	return (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_BASE64,(WPARAM)Str.w_str(),2);
}
//---------------------------------------------------------------------------

//Pobieranie pseudonimu kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
	//Odczyt pseudonimu z pliku INI
	UnicodeString Nick = ContactsNickList->ReadString("Nick",JID,"");
	//Pseudonim nie zostal pobrany
	if(Nick.IsEmpty())
	{
		//Skracanie JID do ladniejszej formy
		if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
		return JID;
	}
	return Nick;
}
//---------------------------------------------------------------------------

//Usuwanie elementu do dodawania zrodla wtyczki
void DestroyBirthdayReminderAddSource()
{
	TPluginAction PluginAction;
	ZeroMemory(&PluginAction,sizeof(TPluginAction));
	PluginAction.cbSize = sizeof(TPluginAction);
	PluginAction.pszName = L"BirthdayReminderItemButton";
	PluginAction.Handle = (int)hFrmSourceAdd;
	PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)&PluginAction);
}
//---------------------------------------------------------------------------

//Tworzenie elementu do dodawania zrodla wtyczki
void BuildBirthdayReminderAddSource()
{
	TPluginAction PluginAction;
	ZeroMemory(&PluginAction,sizeof(TPluginAction));
	PluginAction.cbSize = sizeof(TPluginAction);
	PluginAction.pszName = L"BirthdayReminderItemButton";
	PluginAction.pszCaption = L"Birthday Reminder";
	PluginAction.Position = 0;
	PluginAction.IconIndex = GITF;
	PluginAction.pszService = L"sBirthdayReminderAddSource";
	PluginAction.pszPopupName = L"popAgents";
	PluginAction.PopupPosition = 0;
	PluginAction.Handle = (int)hFrmSourceAdd;
	PluginLink.CallService(AQQ_CONTROLS_CREATEPOPUPMENUITEM,0,(LPARAM)&PluginAction);
}
//---------------------------------------------------------------------------

//Usuwanie elementu z listy zrodel powiadomien
void DestroyNewsDataItem()
{
	TPluginNewsData PluginNewsData;
	PluginNewsData.Kind = NEWS_BIRTHDAYREMINDER;
	PluginNewsData.Title = L"";
	PluginNewsData.ID = NEWS_BIRTHDAYREMINDER_SOURCE;
	PluginNewsData.Active = false;
	PluginNewsData.ImageIndex = GITF;
	PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_DELETE, (WPARAM)&PluginNewsData, 0);
}
//---------------------------------------------------------------------------

//Tworzenie elementu na liscie zrodel powiadomien
void BuildNewsDataItem()
{
	if(SourceAddedChk)
	{
		TPluginNewsData PluginNewsData;
		PluginNewsData.Kind = NEWS_BIRTHDAYREMINDER;
		PluginNewsData.Title = L"Birthday Reminder";
		PluginNewsData.ID = NEWS_BIRTHDAYREMINDER_SOURCE;
		PluginNewsData.Active = SourceActiveChk;
		PluginNewsData.ImageIndex = GITF;
		PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_ADD, (WPARAM)&PluginNewsData, 0);
	}
}
//---------------------------------------------------------------------------

//Hook na wylaczenie komunikatora poprzez usera
INT_PTR __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam)
{
	//Info o rozpoczeciu procedury zamykania komunikatora
	ForceUnloadExecuted = true;

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmianê stanu kontaktu
INT_PTR __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam)
{
	//Pobieranie danych tyczacych kontatku
	TPluginContact ContactsUpdateContact = *(PPluginContact)wParam;
	//Kontakt nie jest czatem
	if(!ContactsUpdateContact.IsChat)
	{
		//Pobieranie identyfikatora kontatku
		UnicodeString JID = (wchar_t*)ContactsUpdateContact.JID;
		//Dowanie JID do listy kontakow
		if(ContactList->IndexOf(JID)==-1) ContactList->Add(JID);
		//Pobieranie i zapisywanie nicku kontatku
		ContactsNickList->WriteString("Nick",JID,(wchar_t*)ContactsUpdateContact.Nick);
		//Odczyt pliku INI kontaktu
		TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
		//Odczyt informacji o metakontakcie
		UnicodeString MetaParent = DecodeBase64(Ini->ReadString("Buddy", "MetaParent", ""));
		MetaParent = MetaParent.Trim();
		//Zamkniecie pliku INI kontaktu
		delete Ini;
		//Kontakt jest metakontaktem
		if(!MetaParent.IsEmpty())
		{
			//Zapis podstawowej zaleznosci
			SiblingsList->WriteBool(JID,MetaParent,true);
			SiblingsList->WriteBool(MetaParent,JID,true);
			//Zapis rozszerzonej zaleznosci
			//Odczyt metakontaktow
			TStringList *Siblings = new TStringList;
			SiblingsList->ReadSection(MetaParent,Siblings);
			//Pobieranie ilosci metakontaktow
			int SiblingsCount = Siblings->Count;
			//Petla zapisywania rozszerzonej zaleznosci
			for(int Count=0;Count<SiblingsCount;Count++)
			{
				if(Siblings->Strings[Count]!=JID)
				{
					SiblingsList->WriteBool(Siblings->Strings[Count],JID,true);
					SiblingsList->WriteBool(JID,Siblings->Strings[Count],true);
				}
			}
			//Usuniecie listy metakontaktow
			delete Siblings;
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane lokalizacji
INT_PTR __stdcall OnLangCodeChanged(WPARAM wParam, LPARAM lParam)
{
	//Czyszczenie cache lokalizacji
	ClearLngCache();
	//Pobranie sciezki do katalogu prywatnego uzytkownika
	UnicodeString PluginUserDir = GetPluginUserDir();
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)lParam;
	LangPath = PluginUserDir + "\\\\Languages\\\\BirthdayReminder\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\BirthdayReminder\\\\" + LangCode + "\\\\";
	}
	//Odswiezenie wszystkich zrodel
	PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_REFRESH, 0, 0);

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modulow
INT_PTR __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
	//Pobranie ID dla enumeracji kontaktów
	ReplyListID = GetTickCount();
	//Wywolanie enumeracji kontaktow
	PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
	//Tworzenie elementu na liscie zrodel powiadomien
	BuildNewsDataItem();

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane stanu aktywnosci zrodla przez usera
INT_PTR __stdcall OnNewsActive(WPARAM wParam, LPARAM lParam)
{
	//Zmienil sie stan naszego zrodla
	if((wchar_t*)wParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
	{
		//Odzaczenie aktywnosci zrodla
		SourceActiveChk = lParam;
		//Zapisywanie informacji o aktywnosci zrodla
		TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
		Ini->WriteBool("Settings","SourceActive",SourceActiveChk);
		delete Ini;
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na usuniecie zrodla powiadomien
INT_PTR __stdcall OnNewsDelete(WPARAM wParam, LPARAM lParam)
{
	//Usuniete zostalo nasze zrodlo
	if((wchar_t*)wParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
	{
		//Zapisywanie informacji o usunieciu zrodla
		TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
		Ini->WriteBool("Settings","SourceAdded",false);
		Ini->WriteBool("Settings","SourceActive",false);
		delete Ini;
		//Odzaczenie usuniecia zrodla
		SourceAddedChk = false;
		SourceActiveChk = false;
		//Usuwanie elementu z listy zrodel powiadomien
		DestroyNewsDataItem();
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na pobranie danych przez wskazane zrodlo
INT_PTR __stdcall OnNewsFetch(WPARAM wParam, LPARAM lParam)
{
	//Wywolane zostalo nasze zrodlo
	if((wchar_t*)lParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
	{
		//Informacja o rozpoczeciu pobierania danych
		PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_FETCHSTART, wParam, 0);
		//Czyszczenie listy kontaktow do blokowania ponownego pokazania metakontaktow
		SourceContactsList->Clear();
		//Pobieranie i rozkodowanie aktualnej daty
		unsigned short tYear=0,tMonth=0,tDay=0;
		TDateTime Todey = TDateTime::CurrentDate();
		DecodeDate(Todey, tYear, tMonth, tDay);
		//Sprawdzanie wszystkich kontatkow z listy
		for(int Count=0;Count<ContactList->Count;Count++)
		{
			//Odczyt daty urodzin kontatku
			TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
			UnicodeString ContactBirthDay = DecodeBase64(Ini->ReadString("Buddy", "Birth", "A==="));
			delete Ini;
			//Usuwanie spacji
			ContactBirthDay = ContactBirthDay.Trim();
			//Odkodowanie daty
			try
			{
				if(ContactBirthDay.Pos("-"))
				{
					//Wyciagniecie roku urodzin
					UnicodeString ContactYear = ContactBirthDay;
					ContactYear = ContactYear.Delete(ContactYear.Pos("-"),ContactYear.Length());
					//Rok daty urodzin jest poprawny (mniejszy od aktualnego roku)
					if((int)tYear>StrToInt(ContactYear))
					{
						//Wyciagniecie miesiaca urodzin
						UnicodeString ContactMonth = ContactBirthDay;
						ContactMonth = ContactMonth.Delete(1,ContactMonth.Pos("-"));
						ContactMonth = ContactMonth.Delete(ContactMonth.Pos("-"),ContactMonth.Length());
						//Wyciagniecie dnia urodzin
						UnicodeString ContactDay = ContactBirthDay;
						ContactDay = ContactDay.Delete(1,ContactDay.Pos("-"));
						ContactDay = ContactDay.Delete(1,ContactDay.Pos("-"));
						//Zakodowanie do domyslnego formatu i odkodowanie
						TDateTime BirthDay = EncodeDate(StrToInt(ContactYear),StrToInt(ContactMonth),StrToInt(ContactDay));
						Word cYear=0,cMonth=0,cDay=0;
						DecodeDate(BirthDay,cYear,cMonth,cDay);
						//Porownanie aktualnej daty z data urodzin kontaktu
						double CurrentDate = EncodeDate(tYear, tMonth, tDay);
						double ContactDate = EncodeDate(tYear, cMonth, cDay);
						int Difference = difftime(ContactDate, CurrentDate);
						//Kontakt obchodzi dzis urodziny
						if((InBirthDayChk)&&(Difference==0))
						{
							//Powiadomienie dla kontatku nie bylo wywolywane
							if(SourceContactsList->IndexOf(ContactList->Strings[Count])==-1)
							{
								//Dodawanie kontatku do listy
								SourceContactsList->Add(ContactList->Strings[Count]);
								//Odczyt metakontaktow
								TStringList *Siblings = new TStringList;
								SiblingsList->ReadSection(ContactList->Strings[Count],Siblings);
								//Pobieranie ilosci metakontaktow
								int SiblingsCount = Siblings->Count;
								//Kontakt zawiera metakontakty
								if(SiblingsCount)
								{
									//Dodawanie metakontaktow do listy
									for(int Count=0;Count<SiblingsCount;Count++)
										SourceContactsList->Add(Siblings->Strings[Count]);
								}
								//Wypelnienie struktury
								TPluginNewsItem PluginNewsItem;
								//Data powiadomienia
								PluginNewsItem.Date = TDateTime::CurrentDate();
								//Tytul powiadomienia
								UnicodeString Title = GetContactNick(ContactList->Strings[Count]).w_str();
								PluginNewsItem.Title = Title.w_str();
								//Ustalanie tekstu powiadomienia
								UnicodeString Text = GetLangStr("Birthday");
                Text = StringReplace(Text, "CC_NICK", GetContactNick(ContactList->Strings[Count]), TReplaceFlags());
								Text = StringReplace(Text, "CC_COUNT", IntToStr(tYear-cYear), TReplaceFlags());
								Text = StringReplace(Text, "CC_JID", ContactList->Strings[Count], TReplaceFlags());
								//Zrodlo powiadomienia
								UnicodeString Source = ContactList->Strings[Count];
								PluginNewsItem.Source = Source.w_str();
								//Dekodowanie sciezki awatara
								TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
								UnicodeString Avatar = DecodeBase64(Ini->ReadString("Other","Avatar",""));
								delete Ini;
								//Usuwanie spacji
								Avatar = Avatar.Trim();
								//Jezeli sciezka awatara zostala prawidlowo pobrana
								if(!Avatar.IsEmpty())
								{
									//Zamienianie sciezki relatywnej na absolutna
									if(Avatar.Pos("{PROFILEPATH}"))
										Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
									else
										Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
								}
								//Plik awatara istnieje
								if(FileExists(Avatar))
								{
									//Zmiana znakow w adresie awatara
									Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
									//Tresc powiadomienia
									UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+Text+"</div>";
									PluginNewsItem.News = News.w_str();
								}
								//Tresc powiadomienia bez awatara
								else
								{
									UnicodeString News = "<div class=\"fb_content\">"+Text+"</div>";
									PluginNewsItem.News = News.w_str();
								}
								//Wewnetrzne ID powiadomienia
								PluginNewsItem.ParentIndex = wParam;
								//Przekazanie danych do AQQ
								PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_ITEM, (WPARAM)&PluginNewsItem, 0);
							}
						}
						//Kontakt obchodzi urodziny za kolejno 1, 2, 3, 4, 5, 6, 7, 14 dni
						else if(((AnotherDayChk==1)&&(Difference==1))||((AnotherDayChk==2)&&(Difference==2))||((AnotherDayChk==3)&&(Difference==3))||((AnotherDayChk==4)&&(Difference==4))||((AnotherDayChk==5)&&(Difference==5))||((AnotherDayChk==6)&&(Difference==6))||((AnotherDayChk==7)&&(Difference==7))||((AnotherDayChk==8)&&(Difference==14)))
						{
							//Powiadomienie dla kontatku nie bylo wywolywane
							if(SourceContactsList->IndexOf(ContactList->Strings[Count])==-1)
							{
								//Dodawanie kontatku do listy
								SourceContactsList->Add(ContactList->Strings[Count]);
								//Odczyt metakontaktow
								TStringList *Siblings = new TStringList;
								SiblingsList->ReadSection(ContactList->Strings[Count],Siblings);
								//Pobieranie ilosci metakontaktow
								int SiblingsCount = Siblings->Count;
								//Kontakt zawiera metakontakty
								if(SiblingsCount)
								{
									//Dodawanie metakontaktow do listy
									for(int Count=0;Count<SiblingsCount;Count++)
										SourceContactsList->Add(Siblings->Strings[Count]);
								}
								//Ustalanie dnia urodzin
								TDateTime DiffBirthDay = EncodeDate(tYear,StrToInt(ContactMonth),StrToInt(ContactDay));
								UnicodeString BirthDayName = DiffBirthDay.FormatString("dddd");
								//Wypelnienie struktury
								TPluginNewsItem PluginNewsItem;
								//Data powiadomienia
								PluginNewsItem.Date = TDateTime::CurrentDate();
								//Tytul powiadomienia
								UnicodeString Title = GetContactNick(ContactList->Strings[Count]).w_str();
								PluginNewsItem.Title = Title.w_str();
								//Ustalanie tekstu powiadomienia
								UnicodeString Text = GetLangStr("Days"+IntToStr(AnotherDayChk));
								Text = StringReplace(Text, "CC_NICK", GetContactNick(ContactList->Strings[Count]), TReplaceFlags());
								Text = StringReplace(Text, "CC_DAY", BirthDayName, TReplaceFlags());
								Text = StringReplace(Text, "CC_DATE", ContactDay+"."+ContactMonth, TReplaceFlags());
								Text = StringReplace(Text, "CC_COUNT", IntToStr(tYear-cYear), TReplaceFlags());
								//Zrodlo powiadomienia
								UnicodeString Source = ContactList->Strings[Count];
								PluginNewsItem.Source = Source.w_str();
								//Dekodowanie sciezki awatara
								TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
								UnicodeString Avatar = DecodeBase64(Ini->ReadString("Other","Avatar",""));
								delete Ini;
								//Usuwanie spacji
								Avatar = Avatar.Trim();
								//Jezeli sciezka awatara zostala prawidlowo pobrana
								if(!Avatar.IsEmpty())
								{
									//Zamienianie sciezki relatywnej na absolutna
									if(Avatar.Pos("{PROFILEPATH}"))
										Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
									else
										Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
								}
								//Plik awatara istnieje
								if(FileExists(Avatar))
								{
									//Zmiana znakow w adresie awatara
									Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
									//Tresc powiadomienia
									UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+Text+"</div>";
									PluginNewsItem.News = News.w_str();
								}
								//Tresc powiadomienia bez awatara
								else
								{
									UnicodeString News = "<div class=\"fb_content\">"+Text+"</div>";
									PluginNewsItem.News = News.w_str();
								}
								//Wewnetrzne ID powiadomienia
								PluginNewsItem.ParentIndex = wParam;
								//Przekazanie danych do AQQ
								PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_ITEM, (WPARAM)&PluginNewsItem, 0);
							}
						}
					}
				}
			}
			catch (...) { /*blad - nie rob nic*/ }
		}
		//Informacja o zakonczeniu pobierania danych
		return PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_FETCHEND, wParam, 0);
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na enumeracje listy kontatkow
INT_PTR __stdcall OnReplyList(WPARAM wParam, LPARAM lParam)
{
	//Sprawdzanie ID wywolania enumeracji
	if(wParam==ReplyListID)
	{
		//Pobieranie danych tyczacych kontatku
		TPluginContact ReplyListContact = *(PPluginContact)lParam;
		//Kontakt nie jest czatem
		if(!ReplyListContact.IsChat)
		{
			//Pobieranie identyfikatora kontatku
			UnicodeString JID = (wchar_t*)ReplyListContact.JID;
			//Dowanie JID do listy kontakow
			if(ContactList->IndexOf(JID)==-1) ContactList->Add(JID);
			//Pobieranie i zapisywanie nicku kontatku
			ContactsNickList->WriteString("Nick",JID,(wchar_t*)ReplyListContact.Nick);
			//Odczyt pliku INI kontaktu
			TIniFile *Ini = new TIniFile(GetContactsUserDir()+JID+".ini");
			//Odczyt informacji o metakontakcie
			UnicodeString MetaParent = DecodeBase64(Ini->ReadString("Buddy", "MetaParent", ""));
			MetaParent = MetaParent.Trim();
			//Zamkniecie pliku INI kontaktu
			delete Ini;
			//Kontakt jest metakontaktem
			if(!MetaParent.IsEmpty())
			{
				//Zapis podstawowej zaleznosci
				SiblingsList->WriteBool(JID,MetaParent,true);
				SiblingsList->WriteBool(MetaParent,JID,true);
				//Zapis rozszerzonej zaleznosci
				//Odczyt metakontaktow
				TStringList *Siblings = new TStringList;
				SiblingsList->ReadSection(MetaParent,Siblings);
				//Pobieranie ilosci metakontaktow
				int SiblingsCount = Siblings->Count;
				//Petla zapisywania rozszerzonej zaleznosci
				for(int Count=0;Count<SiblingsCount;Count++)
				{
					if(Siblings->Strings[Count]!=JID)
					{
						SiblingsList->WriteBool(Siblings->Strings[Count],JID,true);
						SiblingsList->WriteBool(JID,Siblings->Strings[Count],true);
					}
				}
				//Usuniecie listy metakontaktow
				delete Siblings;
			}
		}
	}

	return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie/otwarcie okien
INT_PTR __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam)
{
	//Pobranie informacji o oknie i eventcie
	TPluginWindowEvent WindowEvent = *(PPluginWindowEvent)lParam;
	int Event = WindowEvent.WindowEvent;
	UnicodeString ClassName = (wchar_t*)WindowEvent.ClassName;
	//Otwarcie okna z lista zrodel powiadomien
	if((ClassName=="TfrmSourceAdd")&&(Event==WINDOW_EVENT_CREATE))
	{
		//Pobranie uchwytu do okna
		hFrmSourceAdd = (HWND)(int)WindowEvent.Handle;
		//Tworzenie elementu do dodawania zrodla wtyczki w interfejsie AQQ
		BuildBirthdayReminderAddSource();
	}
	//Zamkniecie okna z lista zrodel powiadomien
	if((ClassName=="TfrmSourceAdd")&&(Event==WINDOW_EVENT_CLOSE))
	{
		//Usuwanie elementu do dodawania zrodla wtyczki z interfejsu AQQ
		DestroyBirthdayReminderAddSource();
		//Usuniecie uchwytu do okna
		hFrmSourceAdd = NULL;
	}

	return 0;
}
//---------------------------------------------------------------------------

INT_PTR __stdcall ServiceBirthdayReminderAddSource(WPARAM wParam, LPARAM lParam)
{
	//Przypisanie uchwytu do formy
	Application->Handle = (HWND)SettingsForm;
	TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
	//Pokaznie okna
	hModalSettingsForm->ShowModal();
	//Zostal wcisniety przycisk OK
	if(hModalSettingsForm->AddSource)
	{
		//Zapisywanie informacji o dodaniu zrodla
		TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
		Ini->WriteBool("Settings","SourceAdded",true);
		Ini->WriteBool("Settings","SourceActive",true);
		delete Ini;
		//Odzaczenie dodania zrodla
		SourceAddedChk = true;
		SourceActiveChk = true;
		//Tworzenie elementu na liscie zrodel powiadomien
		DestroyNewsDataItem();
		BuildNewsDataItem();
		//Odswiezenie wszystkich zrodel
		PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_REFRESH, 0, 0);
	}
	//Usuniecie uchwytu do formy
	delete hModalSettingsForm;

	return 0;
}
//---------------------------------------------------------------------------

//Zapisywanie zasobów
void ExtractRes(wchar_t* FileName, wchar_t* ResName, wchar_t* ResType)
{
	TPluginTwoFlagParams PluginTwoFlagParams;
	PluginTwoFlagParams.cbSize = sizeof(TPluginTwoFlagParams);
	PluginTwoFlagParams.Param1 = ResName;
	PluginTwoFlagParams.Param2 = ResType;
	PluginTwoFlagParams.Flag1 = (int)HInstance;
	PluginLink.CallService(AQQ_FUNCTION_SAVERESOURCE,(WPARAM)&PluginTwoFlagParams,(LPARAM)FileName);
}
//---------------------------------------------------------------------------

//Obliczanie sumy kontrolnej pliku
UnicodeString MD5File(UnicodeString FileName)
{
	if(FileExists(FileName))
	{
		UnicodeString Result;
		TFileStream *fs;
		fs = new TFileStream(FileName, fmOpenRead | fmShareDenyWrite);
		try
		{
			TIdHashMessageDigest5 *idmd5= new TIdHashMessageDigest5();
			try
			{
				Result = idmd5->HashStreamAsHex(fs);
			}
			__finally
			{
				delete idmd5;
			}
		}
		__finally
		{
			delete fs;
		}
		return Result;
	}
	else return 0;
}
//---------------------------------------------------------------------------

//Odczyt ustawien
void LoadSettings()
{
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
	//Status dodania zrodla
	SourceAddedChk = Ini->ReadBool("Settings","SourceAdded",true);
	//Status aktywnosci zrodla
	SourceActiveChk = Ini->ReadBool("Settings","SourceActive",true);
	//Powiadomienie w dniu urodzin
	InBirthDayChk = Ini->ReadBool("Settings","InBirthDay",true);
	//Informowanie rowniez w innym okresie
	AnotherDayChk = Ini->ReadInteger("Settings","Another",0);
	delete Ini;
}
//---------------------------------------------------------------------------

//Zaladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
	//Linkowanie wtyczki z komunikatorem
	PluginLink = *Link;
	//Pobranie sciezki do katalogu prywatnego uzytkownika
	UnicodeString PluginUserDir = GetPluginUserDir();
  //Tworzenie katalogow lokalizacji
	if(!DirectoryExists(PluginUserDir+"\\\\Languages"))
		CreateDir(PluginUserDir+"\\\\Languages");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\BirthdayReminder");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN");
	if(!DirectoryExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL"))
		CreateDir(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL");
	//Wypakowanie plikow lokalizacji
	//81011FF783D47CDEF526FB9D52DB4698
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\Const.lng")!="81011FF783D47CDEF526FB9D52DB4698")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\Const.lng").w_str(),L"EN_CONST",L"DATA");
	//B6B0E733A914E123973A44133E14D649
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\TSettingsForm.lng")!="B6B0E733A914E123973A44133E14D649")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\EN\\\\TSettingsForm.lng").w_str(),L"EN_SETTINGSFRM",L"DATA");
	//F915FF27C2602CA507D1B52A7EEC9996
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\Const.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\Const.lng")!="F915FF27C2602CA507D1B52A7EEC9996")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\Const.lng").w_str(),L"PL_CONST",L"DATA");
	//6B66D8D6D133948041302E7B34F83EC0
	if(!FileExists(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\TSettingsForm.lng"))
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	else if(MD5File(PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\TSettingsForm.lng")!="6B66D8D6D133948041302E7B34F83EC0")
		ExtractRes((PluginUserDir+"\\\\Languages\\\\BirthdayReminder\\\\PL\\\\TSettingsForm.lng").w_str(),L"PL_SETTINGSFRM",L"DATA");
	//Ustawienie sciezki lokalizacji wtyczki
	UnicodeString LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETLANGCODE,0,0);
	LangPath = PluginUserDir + "\\\\Languages\\\\BirthdayReminder\\\\" + LangCode + "\\\\";
	if(!DirectoryExists(LangPath))
	{
		LangCode = (wchar_t*)PluginLink.CallService(AQQ_FUNCTION_GETDEFLANGCODE,0,0);
		LangPath = PluginUserDir + "\\\\Languages\\\\BirthdayReminder\\\\" + LangCode + "\\\\";
	}
	//Tworzeniu z ustawieniami wtyczki
	if(!DirectoryExists(PluginUserDir + "\\\\BirthdayReminder"))
		CreateDir(PluginUserDir + "\\\\BirthdayReminder");
	//Wypakiwanie ikonki BirthdayReminder.dll.png
	//E597085BBB5D15D4882B7FFF8A478B50
	if(!DirectoryExists(PluginUserDir + "\\\\Shared"))
		CreateDir(PluginUserDir + "\\\\Shared");
	if(!FileExists(PluginUserDir + "\\\\Shared\\\\BirthdayReminder.dll.png"))
		ExtractRes((PluginUserDir + "\\\\Shared\\\\BirthdayReminder.dll.png").w_str(),L"SHARED",L"DATA");
	else if(MD5File(PluginUserDir + "\\\\Shared\\\\BirthdayReminder.dll.png")!="E597085BBB5D15D4882B7FFF8A478B50")
		ExtractRes((PluginUserDir + "\\\\Shared\\\\BirthdayReminder.dll.png").w_str(),L"SHARED",L"DATA");
	//Wypakiwanie ikonki Gift.png
	//D1A83D9E809742142A7BDA9B8AF37EB7
	if(!FileExists(PluginUserDir + "\\\\BirthdayReminder\\\\Gift.png"))
		ExtractRes((PluginUserDir + "\\\\BirthdayReminder\\\\Gift.png").w_str(),L"GITF",L"DATA");
	else if(MD5File(PluginUserDir + "\\\\BirthdayReminder\\\\Gift.png")!="D1A83D9E809742142A7BDA9B8AF37EB7")
		ExtractRes((PluginUserDir + "\\\\BirthdayReminder\\\\Gift.png").w_str(),L"GITF",L"DATA");
	//Przypisanie ikonki do interfejsu AQQ
	GITF = PluginLink.CallService(AQQ_ICONS_LOADPNGICON,0, (LPARAM)(PluginUserDir + "\\\\BirthdayReminder\\\\Gift.png").w_str());
	//Odczyt ustawien
	LoadSettings();
	//Hook na wylaczenie komunikatora poprzez usera
	PluginLink.HookEvent(AQQ_SYSTEM_BEFOREUNLOAD,OnBeforeUnload);
	//Hook na zmianê stanu kontaktu
	PluginLink.HookEvent(AQQ_CONTACTS_UPDATE,OnContactsUpdate);
	//Hook na zmiane lokalizacji
	PluginLink.HookEvent(AQQ_SYSTEM_LANGCODE_CHANGED,OnLangCodeChanged);
	//Hook na zaladowanie wszystkich modulow
	PluginLink.HookEvent(AQQ_SYSTEM_MODULESLOADED,OnModulesLoaded);
	//Hook na zmiane stanu aktywnosci zrodla przez usera
	PluginLink.HookEvent(AQQ_SYSTEM_NEWSSOURCE_ACTIVE,OnNewsActive);
	//Hook na usuniecie zrodla powiadomien
	PluginLink.HookEvent(AQQ_SYSTEM_NEWSSOURCE_DELETE,OnNewsDelete);
	//Hook na pobranie danych przez wskazane zrodlo
	PluginLink.HookEvent(AQQ_SYSTEM_NEWSSOURCE_FETCH,OnNewsFetch);
	//Hook na enumeracje listy kontatkow
	PluginLink.HookEvent(AQQ_CONTACTS_REPLYLIST,OnReplyList);
	//Hook na zamkniecie/otwarcie okien
	PluginLink.HookEvent(AQQ_SYSTEM_WINDOWEVENT,OnWindowEvent);
	//Tworzenie serwisu dla elementu do dodawania zrodla wtyczki z interfejsu AQQ
	PluginLink.CreateServiceFunction(L"sBirthdayReminderAddSource",ServiceBirthdayReminderAddSource);
	//Wszystkie moduly zostaly zaladowane
	if(PluginLink.CallService(AQQ_SYSTEM_MODULESLOADED,0,0))
	{
		//Pobranie ID dla enumeracji kontaktów
		ReplyListID = GetTickCount();
		//Wywolanie enumeracji kontaktow
		PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
		//Tworzenie elementu na liscie zrodel powiadomien
		BuildNewsDataItem();
		//Odswiezenie wszystkich zrodel
		PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_REFRESH, 0, 0);
	}

	return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" INT_PTR __declspec(dllexport) __stdcall Unload()
{
	//Wyladowanie wszystkich hookow
	PluginLink.UnhookEvent(OnBeforeUnload);
	PluginLink.UnhookEvent(OnContactsUpdate);
	PluginLink.UnhookEvent(OnLangCodeChanged);
	PluginLink.UnhookEvent(OnModulesLoaded);
	PluginLink.UnhookEvent(OnNewsActive);
	PluginLink.UnhookEvent(OnNewsDelete);
	PluginLink.UnhookEvent(OnNewsFetch);
	PluginLink.UnhookEvent(OnReplyList);
	PluginLink.UnhookEvent(OnWindowEvent);
	//Gdy komunikator nie jest wylaczany
	if(!ForceUnloadExecuted)
	{
		//Usuwanie elementu z listy zrodel powiadomien
		if(SourceAddedChk) DestroyNewsDataItem();
		//Usuwanie elementu do dodawania zrodla wtyczki z interfejsu AQQ
		if(hFrmSourceAdd) DestroyBirthdayReminderAddSource();
		//Usuwanie serwisu dla elementu do dodawania zrodla wtyczki z interfejsu AQQ
		PluginLink.DestroyServiceFunction(ServiceBirthdayReminderAddSource);
		//Wyladowanie ikonki z intefejsu
		PluginLink.CallService(AQQ_ICONS_DESTROYPNGICON,0,(LPARAM)GITF);
	}

	return 0;
}
//---------------------------------------------------------------------------

//Informacje o wtyczce
extern "C" PPluginInfo __declspec(dllexport) __stdcall AQQPluginInfo(DWORD AQQVersion)
{
	PluginInfo.cbSize = sizeof(TPluginInfo);
	PluginInfo.ShortName = L"Birthday Reminder";
	PluginInfo.Version = PLUGIN_MAKE_VERSION(3,3,0,0);
	PluginInfo.Description = L"Powiadamia, poprzez centrum powiadomieñ, o urodzinach kontaktów z naszej listy.";
	PluginInfo.Author = L"Krzysztof Grochocki";
	PluginInfo.AuthorMail = L"kontakt@beherit.pl";
	PluginInfo.Copyright = L"Krzysztof Grochocki";
	PluginInfo.Homepage = L"http://beherit.pl";
	PluginInfo.Flag = 0;
	PluginInfo.ReplaceDefaultModule = 0;

	return &PluginInfo;
}
//---------------------------------------------------------------------------
