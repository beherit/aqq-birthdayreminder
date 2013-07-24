object SettingsForm: TSettingsForm
  Left = 294
  Top = 126
  BorderIcons = [biSystemMenu, biMinimize]
  BorderStyle = bsSingle
  Caption = 'Ustawienia'
  ClientHeight = 185
  ClientWidth = 239
  Color = clBtnFace
  Font.Charset = DEFAULT_CHARSET
  Font.Color = clWindowText
  Font.Height = -11
  Font.Name = 'MS Sans Serif'
  Font.Style = []
  Icon.Data = {
    0000010001001010000001002000680400001600000028000000100000002000
    000001002000000000004004000000000000000000000000000000000000FFFF
    FF00FFFFFF00FFFFFF000DC5EF3E0DC5EF7C0DC5EFB00DC5EFDA0DC5EFF50DC5
    EFF50DC5EFDA0DC5EFB00DC5EF7C0DC5EF3EFFFFFF00FFFFFF00FFFFFF00FFFF
    FF000DC5EF240DC5EF8E0DC5EFF747D3F3FF60D9F4FF73DEF6FF80E1F7FF80E1
    F7FF73DEF6FF60D9F4FF47D3F3FF0DC5EFF70DC5EF8E0DC5EF24FFFFFF00FFFF
    FF000DC5EFA14FD5F3FF81E1F7FF86E2F7FF86E2F7FF86E2F7FF86E2F7FF86E2
    F7FF86E2F7FF86E2F7FF86E2F7FF81E1F7FF4FD5F3FF0DC5EFA1FFFFFF00FFFF
    FF000DC5EFF882E1F7FF75BEF1FF659AEBFF577CE6FF4C63E2FF4554E0FF4554
    E0FF4C63E2FF577CE6FF659AEBFF75BEF1FF82E1F7FF0DC5EFF8FFFFFF00FFFF
    FF0014B4EDFC5F8FEAFF5372E5FF6396EBFF71B4F0FF7CCDF4FF83DCF6FF83DC
    F6FF7CCDF4FF71B4F0FF6396EBFF5372E5FF5F8FEAFF14B4EDFCFFFFFF00FFFF
    FF002C80E6FE67A0ECFF89DCF4FFC2E3EBFFA4E2F1FF86E2F7FF86E2F7FF86E2
    F7FF8DE2F6FFA8E3F0FF8CE2F5FF83DCF6FF67A0ECFF2C80E6FEFFFFFF00FFFF
    FF000DC5EFFC84E1F7FFD7DBDCFFECECECFFE7E7E7FFDFE2E3FFE3E4E4FFE3E4
    E4FFE3E3E3FFEBEBEBFFDCDFE0FF9BE1F1FF84E1F7FF0DC5EFFCFFFFFF00FFFF
    FF0072CEE3FDD7D9DAFFE3E3E3FFFFFFFFFFFFFFFFFFFCFCFCFFE7B098FFE39E
    78FFEDC2B2FFFFFFFFFFEEEEEEFFD6D8D9FFB5DEE8FF29C7EBFCFFFFFF00FFFF
    FF00B5C2C5FEFAFAFAFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFFE5A47EFFE9B4
    93FFE3A278FFFFFFFFFFFFFFFFFFFDFDFDFFEAEAEAFF7CC9DBFCFFFFFF00FFFF
    FF00BEC2C3FCFCFCFCFFEEC1A6FFEAB292FFF1CEBBFFFFFFFFFFE6AA85FFEAB8
    98FFE5A57FFFFFFFFFFFE8AF98FFE39D76FFEABEAFFFB9C2C4F8FFFFFF00FFFF
    FF00B1C2C6A1E3E3E3FFEBB999FFEEC3AAFFEBB495FFFFFFFFFF94B3A1FF55BF
    BCFF86B1A0FFFFFFFFFFE5A37BFFE9B292FFDE9A72FFB1C2C6A1FFFFFF00FFFF
    FF00C2C2C224C2C2C28EEDBB9DFFEFC8AFFFE8B696FFF6F6F6FF07D2D8FF67F5
    F6FF07D2D7FFECECECFFE5A983FFEAB798FFDB9569EBC2C2C224FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0097BCADFF57C4C3FF83B5A8F7C2C2C2DA07D2D9FF30F1
    F3FF5CBEC5ECC2C2C2B094B19FFF55BFBCFF78A794E5FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0007D2D8FF67F5F6FF06D2D8FEFFFFFF0000BBC74800C7
    D3C7FFFFFF00FFFFFF0007D2D8FF67F5F6FF06D1D7FEFFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0000D3DAF630F1F3FF00BBC77CFFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0000D3DAF630F1F3FF00BBC77CFFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0000BBC74800C7D3C7FFFFFF00FFFFFF00FFFFFF00FFFF
    FF00FFFFFF00FFFFFF0000BBC74800C7D3C7FFFFFF00FFFFFF00FFFFFF000000
    0000000000000000000000000000000000000000000000000000000000000000
    000000000000000000000000000000000000000000000000000000000000}
  OldCreateOrder = False
  Position = poScreenCenter
  OnShow = FormShow
  PixelsPerInch = 96
  TextHeight = 13
  object TimeNot: TLabel
    Left = 8
    Top = 10
    Width = 170
    Height = 13
    Caption = 'Czas wy'#347'wietlania notyfikacji (sek):'
    Font.Charset = EASTEUROPE_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'Tahoma'
    Font.Style = []
    ParentFont = False
  end
  object Another: TLabel
    Left = 8
    Top = 99
    Width = 94
    Height = 13
    Caption = 'Informuj r'#243'wnie'#380' na:'
  end
  object RepeatLabel: TLabel
    Left = 8
    Top = 127
    Width = 114
    Height = 13
    Caption = 'Pokazuj powiadomienie:'
  end
  object Bevel: TBevel
    Left = 0
    Top = 152
    Width = 239
    Height = 33
    Align = alBottom
  end
  object OkButton: TButton
    Left = 8
    Top = 156
    Width = 75
    Height = 25
    Caption = 'Zapisz'
    Font.Charset = DEFAULT_CHARSET
    Font.Color = clWindowText
    Font.Height = -11
    Font.Name = 'MS Sans Serif'
    Font.Style = [fsBold]
    ParentFont = False
    TabOrder = 0
    OnClick = OkButtonClick
  end
  object TimeBox: TComboBox
    Left = 184
    Top = 8
    Width = 45
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 1
    Items.Strings = (
      '3'
      '4'
      '5'
      '6'
      '7'
      '8'
      '9'
      '10'
      '11'
      '12'
      '13'
      '14'
      '15')
  end
  object SoundCheckBox: TCheckBox
    Left = 8
    Top = 34
    Width = 185
    Height = 17
    Caption = 'Odtwarzaj muzyk'#281' przy notyfikacji'
    Checked = True
    State = cbChecked
    TabOrder = 2
  end
  object BirthDayCheckBox: TCheckBox
    Left = 8
    Top = 54
    Width = 161
    Height = 17
    Caption = 'Powiadamiaj w dniu urodzin'
    Checked = True
    State = cbChecked
    TabOrder = 3
  end
  object AnotherDayBox: TComboBox
    Left = 108
    Top = 96
    Width = 123
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 4
    Items.Strings = (
      'Brak'
      'Dzie'#324' przed'
      'Dwa dni przed'
      'Trzy dni przed'
      'Cztery dni przed'
      'Pi'#281#263' dni przed'
      'Sze'#347#263' dni przed'
      'Tydzie'#324' przed'
      'Dwa tygodnie przed')
  end
  object AgeCheckBox: TCheckBox
    Left = 8
    Top = 74
    Width = 153
    Height = 17
    Caption = 'Pokazuj wiek solenizanta'
    Checked = True
    State = cbChecked
    TabOrder = 5
  end
  object TestButon: TButton
    Left = 156
    Top = 156
    Width = 75
    Height = 25
    Caption = 'Podgl'#261'd'
    TabOrder = 6
    OnClick = TestButonClick
  end
  object RepeatCheckBox: TComboBox
    Left = 125
    Top = 124
    Width = 106
    Height = 21
    Style = csDropDownList
    ItemHeight = 13
    TabOrder = 7
    Items.Strings = (
      'Przy w'#322#261'czeniu'
      'Raz dziennie'
      'Co godzin'#281
      'Co dwie godziny'
      'Co trzy godziny'
      'Co cztery godziny')
  end
  object FileListBox: TFileListBox
    Left = 248
    Top = 8
    Width = 137
    Height = 137
    FileType = [ftReadOnly, ftHidden, ftSystem, ftArchive, ftNormal]
    ItemHeight = 13
    Mask = '*.ini'
    TabOrder = 8
    Visible = False
  end
  object NicksList: TListBox
    Left = 392
    Top = 8
    Width = 137
    Height = 137
    ItemHeight = 13
    TabOrder = 9
    Visible = False
  end
  object Tajmer: TTimer
    Enabled = False
    Interval = 10000
    OnTimer = TajmerTimer
    Left = 88
    Top = 152
  end
  object XPMan1: TXPMan
    Left = 120
    Top = 152
  end
  object IdDecoderMIME: TIdDecoderMIME
    FillChar = '='
    Left = 32
  end
  object ActionList: TActionList
    object aFindContacts: TAction
      Caption = 'aFindContacts'
      OnExecute = aFindContactsExecute
    end
    object aSaveSettings: TAction
      Caption = 'aSaveSettings'
      OnExecute = aSaveSettingsExecute
    end
    object aReadSettings: TAction
      Caption = 'aReadSettings'
      OnExecute = aReadSettingsExecute
    end
  end
end
