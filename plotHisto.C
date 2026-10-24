// ============================================================
// plotHisto.C — Gamma 2-layer enclosed detector
// Single DepthDose histogram: Al(10cm) + Pb(10cm) + Al(10cm)
// Publication-quality depth-dose with material boundaries
// ============================================================

void plotHisto(const char* fname = "B4a_Enclosed_Gamma_Livermore.root")
{
  gROOT->Reset();
  gStyle->SetOptStat(0);
  gStyle->SetOptTitle(0);
  gStyle->SetPadTickX(1);
  gStyle->SetPadTickY(1);

  TFile* f = TFile::Open(fname);
  if (!f || f->IsZombie()) {
    std::cout << "ERROR: Cannot open " << fname << std::endl;
    return;
  }

  TH1D* hDD = (TH1D*)f->Get("DepthDose");
  if (!hDD) { std::cout << "ERROR: DepthDose not found" << std::endl; return; }

  // X axis: mm → cm (0-30 cm)
  TH1D* h = new TH1D("h","",300, 0., 30.);
  for (int i = 1; i <= 300; i++) {
    h->SetBinContent(i, hDD->GetBinContent(i));
    h->SetBinError(i,   hDD->GetBinError(i));
  }

  double pkY = h->GetMaximum();
  if (pkY > 0) h->Scale(1./pkY);
  pkY = 1.0;
  double pkX = h->GetBinCenter(h->GetMaximumBin());

  TCanvas* c = new TCanvas("c","",850,580);
  c->SetLeftMargin(0.13); c->SetRightMargin(0.05);
  c->SetBottomMargin(0.13); c->SetTopMargin(0.12);

  // Shaded regions for material boundaries
  // Al front: 0-10cm (grey)
  TBox* bAl1 = new TBox(0.,0.,10.,1.35);
  bAl1->SetFillColorAlpha(kGray, 0.25); bAl1->SetLineColor(0);
  bAl1->Draw();
  // Pb: 10-20cm (purple)
  TBox* bPb = new TBox(10.,0.,20.,1.35);
  bPb->SetFillColorAlpha(kMagenta-9, 0.35); bPb->SetLineColor(0);
  bPb->Draw();
  // Al back: 20-30cm (grey)
  TBox* bAl2 = new TBox(20.,0.,30.,1.35);
  bAl2->SetFillColorAlpha(kGray, 0.25); bAl2->SetLineColor(0);
  bAl2->Draw();

  h->SetLineColor(kBlue+1); h->SetLineWidth(2);
  h->GetXaxis()->SetTitle("Depth along beam axis (cm)");
  h->GetYaxis()->SetTitle("Relative dose (peak = 1)");
  h->GetXaxis()->SetTitleSize(0.05);
  h->GetYaxis()->SetTitleSize(0.05);
  h->GetXaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetLabelSize(0.045);
  h->GetYaxis()->SetRangeUser(0, 1.35);
  h->Draw("HIST E same");

  // Boundary lines
  TLine* l1 = new TLine(10.,0.,10.,1.30);
  l1->SetLineColor(kBlack); l1->SetLineStyle(2); l1->SetLineWidth(2); l1->Draw();
  TLine* l2 = new TLine(20.,0.,20.,1.30);
  l2->SetLineColor(kBlack); l2->SetLineStyle(2); l2->SetLineWidth(2); l2->Draw();

  // Peak marker
  TLine* lPk = new TLine(pkX,0.,pkX,1.0);
  lPk->SetLineColor(kRed); lPk->SetLineStyle(3); lPk->SetLineWidth(2); lPk->Draw();

  // Labels
  TLatex lat; lat.SetTextSize(0.040); lat.SetNDC(false);
  lat.SetTextColor(kGray+1);
  lat.DrawLatex(3.5, 1.25, "Al 10cm");
  lat.SetTextColor(kMagenta+2);
  lat.DrawLatex(13., 1.25, "Pb 10cm");
  lat.SetTextColor(kGray+1);
  lat.DrawLatex(23., 1.25, "Al 10cm");

  lat.SetTextColor(kRed); lat.SetTextSize(0.038);
  lat.DrawLatex(pkX+0.3, 1.04, TString::Format("Peak: %.1f cm", pkX));

  TLatex hdr; hdr.SetNDC(true); hdr.SetTextSize(0.036);
  hdr.DrawLatex(0.13, 0.95,
      "#gamma 100 MeV  |  Al(10cm) + Pb(10cm) + Al(10cm)  |  Livermore");

  // Legend
  TLegend* leg = new TLegend(0.60,0.68,0.93,0.88);
  leg->AddEntry(h,    "Depth-dose","l");
  TObject* dummy1 = new TBox(0,0,1,1);
  ((TBox*)dummy1)->SetFillColorAlpha(kGray,0.4);
  TObject* dummy2 = new TBox(0,0,1,1);
  ((TBox*)dummy2)->SetFillColorAlpha(kMagenta-9,0.5);
  leg->AddEntry(dummy1,"Al shell","f");
  leg->AddEntry(dummy2,"Pb inner cube","f");
  leg->SetBorderSize(0);
  leg->Draw();

  c->Update();
  c->Print("DepthDose_Enclosed_Gamma.png");
  c->Print("DepthDose_Enclosed_Gamma.pdf");

  std::cout << "Shower peak at: " << pkX << " cm" << std::endl;
  std::cout << "Saved: DepthDose_Enclosed_Gamma.png/pdf" << std::endl;

  // Canvas 2: Layer energy comparison
  TCanvas* c2 = new TCanvas("c2","Layer energies",60,60,1000,500);
  c2->Divide(2,1);

  c2->cd(1);
  TH1D* hPb = (TH1D*)f->Get("EPb");
  TH1D* hAl = (TH1D*)f->Get("EAl");
  if (hPb && hAl) {
    hPb->SetLineColor(kMagenta+1); hPb->SetLineWidth(2);
    hPb->GetXaxis()->SetTitle("Edep [MeV]");
    hPb->SetTitle("Pb (purple) vs Al (grey) Edep per event");
    hPb->Draw("HIST");
    hAl->SetLineColor(kGray+1); hAl->SetLineWidth(2);
    hAl->DrawNormalized("HIST same", hPb->GetMaximum());
    TLegend* lg=new TLegend(0.5,0.7,0.9,0.9);
    lg->AddEntry(hPb,"Pb 10cm cube","l");
    lg->AddEntry(hAl,"Al 10cm shell","l");
    lg->Draw();
  }

  c2->cd(2);
  TH1D* hN = (TH1D*)f->Get("Nsec");
  if (hN) {
    hN->SetFillColor(kGreen-9);
    hN->GetXaxis()->SetTitle("N secondaries");
    hN->SetTitle("EM shower multiplicity");
    hN->Draw("HIST");
  }

  c2->Update();
  c2->Print("Enclosed_Gamma_LayerEnergy.png");
  std::cout << "Saved: Enclosed_Gamma_LayerEnergy.png" << std::endl;
}
