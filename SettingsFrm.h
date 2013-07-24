//---------------------------------------------------------------------------

#ifndef SettingsFrmH
#define SettingsFrmH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include "acPNG.hpp"
#include "sBevel.hpp"
#include "sButton.hpp"
#include "sCheckBox.hpp"
#include "sComboBox.hpp"
#include <Vcl.ExtCtrls.hpp>
#include <System.Actions.hpp>
#include <Vcl.ActnList.hpp>
#include "sSkinManager.hpp"
#include "sSkinProvider.hpp"
#include <IdBaseComponent.hpp>
#include <IdCoder.hpp>
#include <IdCoder3to4.hpp>
#include <IdCoderMIME.hpp>
//---------------------------------------------------------------------------
class TSettingsForm : public TForm
{
__published:	// IDE-managed Components
	TsBevel *Bevel;
	TsButton *CancelButton;
	TsButton *OKButton;
	TImage *GitfImage;
	TsCheckBox *InBirthDayCheckBox;
	TsComboBox *AnotherDayComboBox;
	TActionList *ActionList;
	TAction *aExit;
	TsSkinProvider *sSkinProvider;
	TsSkinManager *sSkinManager;
	TIdDecoderMIME *IdDecoderMIME;
	void __fastcall aExitExecute(TObject *Sender);
	void __fastcall OKButtonClick(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);

private:	// User declarations
public:		// User declarations
	bool AddSource;
	__fastcall TSettingsForm(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TSettingsForm *SettingsForm;
//---------------------------------------------------------------------------
#endif
