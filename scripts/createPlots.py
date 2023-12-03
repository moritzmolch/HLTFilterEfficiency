import os
import ROOT

#
# TODO MUST BE UPDATED!
#

ROOT.gROOT.SetBatch(True)
ROOT.gStyle.SetOptTitle(0)

hlt_path = "HLT_DoubleTightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg"
#hlt_path = "HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg"
#hlt_path = "HLT_DoubleTightChargedIsoPFTau40_Trk1_eta2p1_Reg"

dy_file = ROOT.TFile.Open(os.path.abspath(os.path.join(os.path.expandvars("${CMSSW_BASE}"), "src", "histos_" + hlt_path + "_dy.root")), "READ")
emb_file = ROOT.TFile.Open(os.path.abspath(os.path.join(os.path.expandvars("${CMSSW_BASE}"), "src", "histos_" + hlt_path + "_embedding.root")), "READ")


h_emb_cutflow = emb_file.Get("filterEfficiencyAnalyzer/cutflow_evt")
h_emb_rej_rate = emb_file.Get("filterEfficiencyAnalyzer/rej_rate_evt")
h_dy_cutflow = dy_file.Get("filterEfficiencyAnalyzer/cutflow_evt")
h_dy_rej_rate = dy_file.Get("filterEfficiencyAnalyzer/rej_rate_evt")


def plot(h_emb_cutflow, h_dy_cutflow, h_emb_rej_rate, h_dy_rej_rate, title, filename):

    c = ROOT.TCanvas("c", "c", 1024, 1024)
    c.Divide(1, 2)

    c.cd(1)
    h_dy_cutflow.Draw("HIST")
    h_emb_cutflow.Draw("HISTSAME")
    h_dy_cutflow.GetYaxis().SetRangeUser(0., 0.2)
    h_dy_cutflow.SetLineWidth(2)
    h_dy_cutflow.SetTitle("Z #rightarrow #tau#tau")
    h_emb_cutflow.SetTitle("#mu #rightarrow #tau embedding")
    h_emb_cutflow.SetLineWidth(2)
    h_dy_cutflow.SetLineColor(38)
    h_emb_cutflow.SetLineColor(46)
    h_dy_cutflow.SetStats(0)
    h_emb_cutflow.SetStats(0)
    legend = c.cd(1).BuildLegend()
    pave_title = ROOT.TPaveText(0.0, 0.92, 0.5, 1.0, "NDC")
    pave_title.AddText(title)
    pave_title.Draw("SAME")
    c.Update()

    c.cd(2)
    h_dy_rej_rate.Draw("HIST")
    h_emb_rej_rate.Draw("HISTSAME")
    h_dy_rej_rate.GetYaxis().SetRangeUser(0., 1.)
    h_dy_rej_rate.SetLineWidth(2)
    h_emb_rej_rate.SetLineWidth(2)
    h_dy_rej_rate.SetLineColor(38)
    h_emb_rej_rate.SetLineColor(46)
    h_dy_rej_rate.SetStats(0)
    h_emb_rej_rate.SetStats(0)
    c.Update()

    c.SaveAs(filename)


plot(h_emb_cutflow, h_dy_cutflow, h_emb_rej_rate, h_dy_rej_rate, hlt_path, hlt_path + "_filter_efficiencies.pdf")


dy_file.Close()
emb_file.Close()