#include <vcl.h>
#include <windows.h>
#include <PluginAPI.h>
#pragma hdrstop
#pragma argsused
#include "SettingsFrm.h"
#include <inifiles.hpp>
#include <time.h>
#include <IdHashMessageDigest.hpp>

int WINAPI DllEntryPoint(HINSTANCE hinst, unsigned long reason, void* lpReserved)
{
  return 1;
}
//---------------------------------------------------------------------------

//STRUKTURY-GLOWNE-----------------------------------------------------------
TPluginLink PluginLink;
TPluginInfo PluginInfo;
TPluginNewsData PluginNewsData;
TPluginAction PluginAction;
PPluginContact ContactsUpdateContact;
PPluginContact ReplyListContact;
PPluginWindowEvent WindowEvent;
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
TStringList* ContactNickList = new TStringList;
//GDY-ZOSTALO-URUCHOMIONE-WYLADOWANIE-WTYCZKI-WRAZ-Z-ZAMKNIECIEM-KOMUNIKATORA
bool ForceUnloadExecuted = false;
//SETTINGS-------------------------------------------------------------------
bool SourceAddedChk;
bool SourceActiveChk;
bool InBirthDayChk;
int AnotherDayChk;
//FORWARD-AQQ-HOOKS----------------------------------------------------------
int __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam);
int __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam);
int __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam);
int __stdcall OnNewsActive(WPARAM wParam, LPARAM lParam);
int __stdcall OnNewsDelete(WPARAM wParam, LPARAM lParam);
int __stdcall OnNewsFetch(WPARAM wParam, LPARAM lParam);
int __stdcall OnReplyList(WPARAM wParam, LPARAM lParam);
int __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam);
int __stdcall ServiceBirthdayReminderAddSource(WPARAM wParam, LPARAM lParam);
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

//Sprawdzanie czy wlaczona jest obsluga stylow obramowania okien
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

//Sprawdzanie czy wlaczony jest natywny styl Windows
bool ChkNativeEnabled()
{
  TStrings* IniList = new TStringList();
  IniList->SetText((wchar_t*)PluginLink.CallService(AQQ_FUNCTION_FETCHSETUP,0,0));
  TMemIniFile *Settings = new TMemIniFile(ChangeFileExt(Application->ExeName, ".INI"));
  Settings->SetStrings(IniList);
  delete IniList;
  UnicodeString NativeEnabled = Settings->ReadString("Settings","Native","0");
  delete Settings;
  return StrToBool(NativeEnabled);
}
//---------------------------------------------------------------------------

//Pobieranie pseudonimu kontaktu podajac jego JID
UnicodeString GetContactNick(UnicodeString JID)
{
  UnicodeString Nick = ContactsNickList->ReadString("Nick",JID,"");
  if(Nick.IsEmpty())
  {
	if(JID.Pos("@")) JID.Delete(JID.Pos("@"),JID.Length());
	return JID;
  }
  return Nick;
}
//---------------------------------------------------------------------------

void DestroyBirthdayReminderAddSource()
{
  PluginAction.cbSize = sizeof(TPluginAction);
  PluginAction.pszName = L"BirthdayReminderItemButton";
  PluginLink.CallService(AQQ_CONTROLS_DESTROYPOPUPMENUITEM,0,(LPARAM)&PluginAction);
}
//---------------------------------------------------------------------------

void BuildBirthdayReminderAddSource()
{
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
  PluginNewsData.Kind = NEWS_BIRTHDAYREMINDER;
  PluginNewsData.Title = L"Birthday Reminder";
  PluginNewsData.ID = NEWS_BIRTHDAYREMINDER_SOURCE;
  PluginNewsData.Active = SourceActiveChk;
  PluginNewsData.ImageIndex = GITF;
  PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_ADD, (WPARAM)&PluginNewsData, 0);
}
//---------------------------------------------------------------------------

//Hook na wylaczenie komunikatora poprzez usera
int __stdcall OnBeforeUnload(WPARAM wParam, LPARAM lParam)
{
  //Info o rozpoczeciu procedury zamykania komunikatora
  ForceUnloadExecuted = true;

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmianê stanu kontaktu
int __stdcall OnContactsUpdate(WPARAM wParam, LPARAM lParam)
{
  //Pobieranie danych tyczacych kontatku
  ContactsUpdateContact = (PPluginContact)wParam;
  //Kontakt nie jest czatem
  if(!ContactsUpdateContact->IsChat)
  {
    //Pobieranie identyfikatora kontatku
	UnicodeString JID = (wchar_t*)ContactsUpdateContact->JID;
	//Dowanie JID do listy kontakow
	if(ContactList->IndexOf(JID)==-1) ContactList->Add(JID);
	//Pobieranie i zapisywanie nicku kontatku
	ContactsNickList->WriteString("Nick",JID,(wchar_t*)ContactsUpdateContact->Nick);
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zaladowanie wszystkich modulow
int __stdcall OnModulesLoaded(WPARAM wParam, LPARAM lParam)
{
  //Pobranie ID dla enumeracji kontaktów
  ReplyListID = GetTickCount();
  //Wywolanie enumeracji kontaktow
  PluginLink.CallService(AQQ_CONTACTS_REQUESTLIST,(WPARAM)ReplyListID,0);
  //Sprawdzenie czy wtyczka zostala dodana do zrodel powiadomien
  if(SourceAddedChk) BuildNewsDataItem();

  return 0;
}
//---------------------------------------------------------------------------

//Hook na zmiane stanu aktywnosci zrodla przez usera
int __stdcall OnNewsActive(WPARAM wParam, LPARAM lParam)
{
  //Zmienil sie stan naszego zrodla
  if((wchar_t*)wParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
  {
	//Zapisywanie informacji o aktywnosci zrodla
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
	Ini->WriteBool("Settings","SourceActive",lParam);
	delete Ini;
	//Odzaczenie aktywnosci zrodla
	SourceActiveChk = lParam;
	//Odswiezenie wszystkich zrodel
	if(SourceActiveChk) PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_REFRESH, 0, 0);
  }
  
  return 0;
}
//---------------------------------------------------------------------------

//Hook na usuniecie zrodla powiadomien
int __stdcall OnNewsDelete(WPARAM wParam, LPARAM lParam)
{
  //Usuniete zostalo nasze zrodlo
  if((wchar_t*)wParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
  {
	//Zapisywanie informacji o usunieciu zrodla
	TIniFile *Ini = new TIniFile(GetPluginUserDir()+"\\\\BirthdayReminder\\\\Settings.ini");
	Ini->WriteBool("Settings","SourceAdded",false);
	Ini->WriteBool("Settings","SourceActive",true);
	delete Ini;
	//Odzaczenie usuniecia zrodla
	SourceAddedChk = false;
	SourceActiveChk = true;
    //Usuwanie elementu z listy zrodel powiadomien
	DestroyNewsDataItem();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Hook na pobranie danych przez wskazane zrodlo
int __stdcall OnNewsFetch(WPARAM wParam, LPARAM lParam)
{
  //Wywolane zostalo nasze zrodlo
  if((wchar_t*)lParam==(UnicodeString)NEWS_BIRTHDAYREMINDER_SOURCE)
  {
	//Informacja o rozpoczeciu pobierania danych
	PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_FETCHSTART, wParam, 0);
	//Czyszczenie listy metakontatkow
	ContactNickList->Clear();
	//Pobieranie i rozkodowanie aktualnej daty
	Word tYear=0,tMonth=0,tDay=0;
	TDateTime Todey = TDateTime::CurrentDate();
	DecodeDate(Todey, tYear, tMonth, tDay);
	//Przypisanie uchwytu do formy
	Application->Handle = (HWND)SettingsForm;
	TSettingsForm *hModalSettingsForm = new TSettingsForm(Application);
	//Sprawdzanie wszystkich kontatkow z listy
	for(int Count=0;Count<ContactList->Count;Count++)
	{
	  //Odczyt daty urodzin kontatku
	  TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
	  UnicodeString ContactBirthDay = hModalSettingsForm->IdDecoderMIME->DecodeString((Ini->ReadString("Buddy", "Birth", "A===").c_str()));
	  delete Ini;
	  ContactBirthDay = ContactBirthDay.Trim();
	  //Odkodowanie daty
      try
	  {
		if(ContactBirthDay.Pos("-"))
		{
		  //Usuwanie spacji
		  ContactBirthDay = ContactBirthDay.Trim();
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
			  if(ContactNickList->IndexOf(GetContactNick(ContactList->Strings[Count]))==-1)
			  {
				//Dodawanie pseudonimu kontatku do listy
				ContactNickList->Add(GetContactNick(ContactList->Strings[Count]));
				//Wypelnienie struktury
				TPluginNewsItem PluginNewsItem;
				//Data powiadomienia
				PluginNewsItem.Date = TDateTime::CurrentDate();
				//Tytul powiadomienia
				UnicodeString Title = GetContactNick(ContactList->Strings[Count])+" ma dzisiaj urodziny";
				PluginNewsItem.Title = Title.w_str();
				//Zrodlo powiadomienia
				PluginNewsItem.Source = L"";
				//Dekodowanie sciezki awatara
				TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
				UnicodeString Avatar = hModalSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
				delete Ini;
				//Jezeli sciezka awatata zostala prawidlowo pobrana
				if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
				{
				  //Zamienianie sciezki relatywnej na absolutna
				  if(Avatar.Pos("{PROFILEPATH}"))
				   Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
				  else
				   Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
				  //Plik awatara istnieje
				  if(FileExists(Avatar))
				  {
					//Zmiana znakow w adresie awatara
					Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					//Tresc powiadomienia
					UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi dziœ "+IntToStr(tYear-cYear)+" urodziny! Nie czekaj i <a href=\"xmpp:"+ContactList->Strings[Count]+"\">z³ó¿ ¿yczenia</a>!</div>";
					PluginNewsItem.News = News.w_str();
				  }
				  //Domyslny awatar
				  else
				  {
					//Pobranie sciezki do domyslnego awatara
					Avatar = GetThemeDir()+"\\\\Graphics\\\\noavatar.png";
					//Plik awatara istnieje
					if(FileExists(Avatar))
					{
					  //Zmiana znakow w adresie awatara
					  Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					  //Tresc powiadomienia
					  UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi dziœ "+IntToStr(tYear-cYear)+" urodziny! Nie czekaj i <a href=\"xmpp:"+ContactList->Strings[Count]+"\">z³ó¿ ¿yczenia</a>!</div>";
					  PluginNewsItem.News = News.w_str();
					}
					//Tresc powiadomienia bez awatara
					else
					{
					  UnicodeString News = "<div class=\"fb_content\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi dziœ "+IntToStr(tYear-cYear)+" urodziny! Nie czekaj i <a href=\"xmpp:"+ContactList->Strings[Count]+"\">z³ó¿ ¿yczenia</a>!</div>";
					  PluginNewsItem.News = News.w_str();
					}
				  }
				}
				//Domyslny awatar
				else
				{
				  //Pobranie sciezki do domyslnego awatara
				  Avatar = GetThemeDir()+"\\\\Graphics\\\\noavatar.png";
				  //Plik awatara istnieje
				  if(FileExists(Avatar))
				  {
					//Zmiana znakow w adresie awatara
					Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					//Tresc powiadomienia
					UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi dziœ "+IntToStr(tYear-cYear)+" urodziny! Nie czekaj i <a href=\"xmpp:"+ContactList->Strings[Count]+"\">z³ó¿ ¿yczenia</a>!</div>";
					PluginNewsItem.News = News.w_str();
				  }
				  //Tresc powiadomienia bez awatara
				  else
				  {
					UnicodeString News = "<div class=\"fb_content\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi dziœ "+IntToStr(tYear-cYear)+" urodziny! Nie czekaj i <a href=\"xmpp:"+ContactList->Strings[Count]+"\">z³ó¿ ¿yczenia</a>!</div>";
					PluginNewsItem.News = News.w_str();
				  }
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
			  if(ContactNickList->IndexOf(GetContactNick(ContactList->Strings[Count]))==-1)
			  {
				//Dodawanie pseudonimu kontatku do listy
				ContactNickList->Add(GetContactNick(ContactList->Strings[Count]));
				//Ustalanie tekstu powiadomienia
				UnicodeString Text;
				if(AnotherDayChk==1) Text = "jutro";
				else if(AnotherDayChk==2) Text = "za dwa dni";
				else if(AnotherDayChk==3) Text = "za trzy dni";
				else if(AnotherDayChk==4) Text = "za cztery dni";
				else if(AnotherDayChk==5) Text = "za piêæ dni";
				else if(AnotherDayChk==6) Text = "za szeœæ dni";
				else if(AnotherDayChk==7) Text = "za tydzieñ";
				else if(AnotherDayChk==8) Text = "za dwa tygodnie";
				//Wypelnienie struktury
				TPluginNewsItem PluginNewsItem;
				//Data powiadomienia
				PluginNewsItem.Date = TDateTime::CurrentDate();
				//Tytul powiadomienia
				UnicodeString Title = GetContactNick(ContactList->Strings[Count])+" ma "+Text+" urodziny";
				PluginNewsItem.Title = Title.w_str();
				//Zrodlo powiadomienia
				PluginNewsItem.Source = L"";
				//Dekodowanie sciezki awatara
				TIniFile *Ini = new TIniFile(GetContactsUserDir()+ContactList->Strings[Count]+".ini");
				UnicodeString Avatar = hModalSettingsForm->IdDecoderMIME->DecodeString(Ini->ReadString("Other","Avatar",""));
				delete Ini;
				//Jezeli sciezka awatata zostala prawidlowo pobrana
				if((!Avatar.IsEmpty())&&(Avatar.Length()>1))
				{
				  //Zamienianie sciezki relatywnej na absolutna
				  if(Avatar.Pos("{PROFILEPATH}"))
				   Avatar = StringReplace(Avatar, "{PROFILEPATH}", GetUserDir(), TReplaceFlags());
				  else
				   Avatar = StringReplace(Avatar, "{APPPATH}", GetApplicationPath(), TReplaceFlags());
				  //Plik awatara istnieje
				  if(FileExists(Avatar))
				  {
					//Zmiana znakow w adresie awatara
					Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					//Tresc powiadomienia
					UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi "+Text+" "+IntToStr(tYear-cYear)+" urodziny!</div>";
					PluginNewsItem.News = News.w_str();
				  }
				  //Domyslny awatar
				  else
				  {
					//Pobranie sciezki do domyslnego awatara
					Avatar = GetThemeDir()+"\\\\Graphics\\\\noavatar.png";
					//Plik awatara istnieje
					if(FileExists(Avatar))
					{
					  //Zmiana znakow w adresie awatara
					  Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					  //Tresc powiadomienia
					  UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi "+Text+" "+IntToStr(tYear-cYear)+" urodziny!</div>";
					  PluginNewsItem.News = News.w_str();
					}
					//Tresc powiadomienia bez awatara
					else
					{
					  UnicodeString News = "<div class=\"fb_content\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi "+Text+" "+IntToStr(tYear-cYear)+" urodziny!</div>";
					  PluginNewsItem.News = News.w_str();
					}
				  }
				}
				//Domyslny awatar
				else
				{
				  //Pobranie sciezki do domyslnego awatara
				  Avatar = GetThemeDir()+"\\\\Graphics\\\\noavatar.png";
				  //Plik awatara istnieje
				  if(FileExists(Avatar))
				  {
					//Zmiana znakow w adresie awatara
					Avatar = StringReplace(Avatar, "\\\\", "/", TReplaceFlags() << rfReplaceAll);
					//Tresc powiadomienia
					UnicodeString News = "<div class=\"fb_content\"><img src=\"file:///"+Avatar+"\" class=\"fb_userpic\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi "+Text+" "+IntToStr(tYear-cYear)+" urodziny!</div>";
					PluginNewsItem.News = News.w_str();
				  }
				  //Tresc powiadomienia bez awatara
				  else
				  {
					UnicodeString News = "<div class=\"fb_content\">"+GetContactNick(ContactList->Strings[Count])+" obchodzi "+Text+" "+IntToStr(tYear-cYear)+" urodziny!</div>";
					PluginNewsItem.News = News.w_str();
				  }
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
	//Anty "Abnormal program termination"
	hModalSettingsForm->DestroyComponents();
	//Usuniecie uchwytu do formy
	delete hModalSettingsForm;
	//Informacja o zakonczeniu pobierania danych
	return PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_FETCHEND, wParam, 0);
  }
  
  return 0;
}
//---------------------------------------------------------------------------

//Hook na enumeracje listy kontatkow
int __stdcall OnReplyList(WPARAM wParam, LPARAM lParam)
{
  //Sprawdzanie ID wywolania enumeracji
  if(wParam==ReplyListID)
  {
    //Pobieranie danych tyczacych kontatku
	ReplyListContact = (PPluginContact)lParam;
	//Kontakt nie jest czatem
	if(!ReplyListContact->IsChat)
	{
	  //Pobieranie identyfikatora kontatku
	  UnicodeString JID = (wchar_t*)ReplyListContact->JID;
	  //Dowanie JID do listy kontakow
	  if(ContactList->IndexOf(JID)==-1) ContactList->Add(JID);
	  //Pobieranie i zapisywanie nicku kontatku
	  ContactsNickList->WriteString("Nick",JID,(wchar_t*)ReplyListContact->Nick);
	}
  }
  
  return 0;
}
//---------------------------------------------------------------------------

//Hook na zamkniecie/otwarcie okien
int __stdcall OnWindowEvent(WPARAM wParam, LPARAM lParam)
{
  //Pobranie informacji o oknie i eventcie
  WindowEvent = (PPluginWindowEvent)lParam;
  int Event = WindowEvent->WindowEvent;
  UnicodeString EventType = (wchar_t*)WindowEvent->ClassName;
  
  //Otwarcie okna z lista zrodel powiadomien
  if((EventType=="TfrmSourceAdd")&&(Event==WINDOW_EVENT_CREATE))
  {
    //Pobranie uchwytu do okna	
	hFrmSourceAdd = (HWND)WindowEvent->Handle;
	//Tworzenie elementu do dodawania zrodla wtyczki w interfejsie AQQ
	BuildBirthdayReminderAddSource();
  }
  //Zamkniecie okna z lista zrodel powiadomien
  if((EventType=="TfrmSourceAdd")&&(Event==WINDOW_EVENT_CLOSE))
  {
    //Usuwanie elementu do dodawania zrodla wtyczki z interfejsu AQQ
	DestroyBirthdayReminderAddSource();
	//Usuniecie uchwytu do okna
	hFrmSourceAdd = NULL;
  }
  
  return 0;
}
//---------------------------------------------------------------------------

int __stdcall ServiceBirthdayReminderAddSource(WPARAM wParam, LPARAM lParam)
{
  //Element na liscie zrdodel powiadomien nie zostal jeszcze dodany
  if(!SourceAddedChk)
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
  }
  //Element na liscie zrodel powiadomien zostal juz dodany
  else
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
	  //Ponowne tworzenie elementu na liscie zrodel powiadomien
	  DestroyNewsDataItem();
	  BuildNewsDataItem();
	  //Odswiezenie wszystkich zrodel
	  PluginLink.CallService(AQQ_SYSTEM_NEWSSOURCE_REFRESH, 0, 0);
	}
	//Usuniecie uchwytu do formy
	delete hModalSettingsForm;
  }
  
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
  else
   return 0;
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
extern "C" int __declspec(dllexport) __stdcall Load(PPluginLink Link)
{
  //Linkowanie wtyczki z komunikatorem
  PluginLink = *Link;
  //Pobranie sciezki do katalogu prywatnego uzytkownika
  UnicodeString PluginUserDir = GetPluginUserDir();
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
	//Sprawdzenie czy wtyczka zostala dodana do zrodel powiadomien
	if(SourceAddedChk) BuildNewsDataItem();
  }

  return 0;
}
//---------------------------------------------------------------------------

//Wyladowanie wtyczki
extern "C" int __declspec(dllexport) __stdcall Unload()
{
  //Wyladowanie wszystkich hookow
  PluginLink.UnhookEvent(OnBeforeUnload);
  PluginLink.UnhookEvent(OnContactsUpdate);
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
  PluginInfo.Version = PLUGIN_MAKE_VERSION(3,1,0,0);
  PluginInfo.Description = L"Wtyczka powiadamia, poprzez centrum powiadomieñ, o obchodzeniu urodzin kontaktów z naszej listy.";
  PluginInfo.Author = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.AuthorMail = L"kontakt@beherit.pl";
  PluginInfo.Copyright = L"Krzysztof Grochocki (Beherit)";
  PluginInfo.Homepage = L"http://beherit.pl";

  return &PluginInfo;
}
//---------------------------------------------------------------------------