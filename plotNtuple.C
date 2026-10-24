void plotNtuple(const char* fname = "B4a_Pb_Gamma_Livermore.root")
{
  gROOT->Reset(); gStyle->SetOptStat(0);
  TFile* f = TFile::Open(fname);
  if(!f||f->IsZombie()){std::cout<<"ERROR"<<std::endl;return;}
  TTree* nt=(TTree*)f->Get("B4");
  if(!nt){std::cout<<"ntuple not found"<<std::endl;return;}
  std::cout<<"Entries: "<<nt->GetEntries()<<std::endl;

  TCanvas* c=new TCanvas("c","Gamma ntuple",10,10,1200,500);
  c->Divide(3,2);

  c->cd(1); nt->Draw("Eabs>>hE(100,0,110)","","");
  ((TH1*)gPad->GetPrimitive("hE"))->SetTitle("Eabs Pb [MeV]");

  c->cd(2); nt->Draw("Labs*0.1>>hL(100,0,5000)","","");
  ((TH1*)gPad->GetPrimitive("hL"))->SetTitle("Labs [mm]");

  c->cd(3); nt->Draw("SecEdep>>hS(100,0,100)","","");
  ((TH1*)gPad->GetPrimitive("hS"))->SetTitle("SecEdep [MeV]");

  c->cd(4); nt->Draw("Nsec>>hN(100,0,500)","","");
  ((TH1*)gPad->GetPrimitive("hN"))->SetTitle("Nsec (shower mult)");

  c->cd(5); nt->Draw("EGap>>hEg(50,0,1)","","");
  ((TH1*)gPad->GetPrimitive("hEg"))->SetTitle("EGap (0mm)");

  c->cd(6); nt->Draw("Eabs:Nsec>>h2(50,0,500,50,0,110)","","colz");

  c->Update(); c->Print("Gamma_ntuple.png");
  std::cout<<"Saved: Gamma_ntuple.png"<<std::endl;
}
