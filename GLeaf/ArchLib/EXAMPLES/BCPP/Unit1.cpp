//---------------------------------------------------------------------------
// #define AL_USING_DLL  // Only if using DLL version of ArchiveLib!
#include "al.h"          // ArchiveLib header files go FIRST!
#include "arclib.h"
#include "arclist.h"

#include <vcl\vcl.h>
#pragma hdrstop
#include "Unit1.h"

//---------------------------------------------------------------------------
#pragma resource "*.dfm"
TForm1 *Form1;
//---------------------------------------------------------------------------
__fastcall TForm1::TForm1(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button2Click(TObject *Sender)
{
Application->Terminate();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button1Click(TObject *Sender)
{
 FileListBox1->Update();
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button4Click(TObject *Sender)
{
 ALPkArchive archive("example.zip");
 ALEntryList list(0, PkTools());
 ALEntry *entry;

 ListBox2->Items->Clear();
 int z = archive.ReadDirectory(list);
 if (z > 0) {
 for(entry = list.GetFirstEntry(); entry; entry = entry->GetNextEntry())
  ListBox2->Items->Add((char*)entry->mpStorageObject->mName);
  }
  else
   ListBox2->Items->Add("<empty>");
}
//---------------------------------------------------------------------------
void __fastcall TForm1::Button3Click(TObject *Sender)
{
 ALPkArchive archive("example.zip");
 ALWindowsMessage monitor( AL_MONITOR_OBJECTS,
                           Edit1->Handle,
                           AL_SEND_RATIO,
                           ProgressBar1->Handle,
                           PBM_SETPOS );
 ALEntryList list(&monitor, PkTools());

 Button3->Enabled = False;
 list.MakeEntriesFromListBox(FileListBox1->Handle,-1);
 archive.Create(list);
 Button3->Enabled = True;
}
//---------------------------------------------------------------------------
