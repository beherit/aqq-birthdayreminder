//---------------------------------------------------------------------------

#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
#include <ExtCtrls.hpp>
#include "XPMan.hpp"
#include "IdCoderMIME.hpp"
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <FileCtrl.hpp>
#include <ActnList.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
        TLabel *TimeNot;
        TButton *OkButton;
        TComboBox *TimeBox;
        TCheckBox *SoundCheckBox;
        TCheckBox *BirthDayCheckBox;
        TComboBox *AnotherDayBox;
        TLabel *Another;
        TCheckBox *AgeCheckBox;
        TButton *TestButon;
        TTimer *Tajmer;
        TLabel *RepeatLabel;
        TComboBox *RepeatCheckBox;
        TXPMan *XPMan1;
        TBevel *Bevel;
        TIdDecoderMIME *IdDecoderMIME;
        TFileListBox *FileListBox;
        TActionList *ActionList;
        TAction *aFindContacts;
        TListBox *NicksList;
        TAction *aSaveSettings;
        TAction *aReadSettings;
        void __fastcall FormShow(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
        void __fastcall TestButonClick(TObject *Sender);
        void __fastcall TajmerTimer(TObject *Sender);
        void __fastcall aFindContactsExecute(TObject *Sender);
        void __fastcall aSaveSettingsExecute(TObject *Sender);
        void __fastcall aReadSettingsExecute(TObject *Sender);
private:	// User declarations
public:		// User declarations
        __fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
