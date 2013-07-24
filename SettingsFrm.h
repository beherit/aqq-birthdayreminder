//---------------------------------------------------------------------------

#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include <Classes.hpp>
#include <Controls.hpp>
#include <StdCtrls.hpp>
#include <Forms.hpp>
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
        void __fastcall FormShow(TObject *Sender);
        void __fastcall OkButtonClick(TObject *Sender);
private:	// User declarations
public:		// User declarations
        AnsiString setPluginPath;
        __fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
