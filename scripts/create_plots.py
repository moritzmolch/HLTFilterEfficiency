import re
import os
import ROOT
from time import sleep

ROOT.gROOT.SetBatch(False)
#ROOT.gStyle.SetOptStats(0)
#ROOT.gStyle.SetOptTitle(0)
ROOT.TH1.AddDirectory(False)


COLORS = [
    ROOT.kAzure + 1,
    ROOT.kGreen + 2,
]


LINESTYLE = [
    1,
    7,
]


HLT_PATHS = {
    "ElTau": [
        "HLT_Ele27_WPTight_Gsf",
        "HLT_Ele32_WPTight_Gsf",
        "HLT_Ele32_WPTight_Gsf_DoubleL1EG",
        "HLT_Ele35_WPTight_Gsf",
        "HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1",
    ],
    "MuTau": [
        "HLT_IsoMu24",
        "HLT_IsoMu27",
        "HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1",
    ],
    "TauTau": [
        "HLT_DoubleTightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg",
        "HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg",
        "HLT_DoubleTightChargedIsoPFTau40_Trk1_eta2p1_Reg",
    ],
}


def load_hists(filepath):
    hists = {}
    root_file = ROOT.TFile.Open(filepath, "READ")
    histograms = root_file.Get("hltFilterPassAnalyzer")
    for k in histograms.GetListOfKeys():
        root_hist = k.ReadObj()
        match = re.match(r"^(.*)(_v\d+_saveTags)$", root_hist.GetName())
        if not match:
            continue
        key = match.group(1)
        hists[key] = root_hist
    root_file.Close()
    return hists


def create_cutflow_efficiency_histogram(histogram):
    name = histogram.GetName() + "_cutflow_eff"
    title = histogram.GetTitle()
    n_bins = histogram.GetNbinsX() - 1
    h_cutflow_eff = ROOT.TEfficiency(name, title, n_bins, 0, n_bins)
    h_cutflow_eff.SetConfidenceLevel(0.68)
    h_cutflow_eff.SetStatisticOption(ROOT.TEfficiency.kFCP)
    n_total = int(histogram.GetBinContent(1))
    for i in range(1, n_bins + 1):
        n_pass = int(histogram.GetBinContent(i + 1))
        h_cutflow_eff.SetTotalEvents(i, n_total)
        h_cutflow_eff.SetPassedEvents(i, n_pass)
        #h_cutflow_eff.SetBinContent(i, n_pass / n_total)
        #h_cutflow_eff.GetXaxis().SetBinLabel(i, histogram.GetXaxis().GetBinLabel(i + 1))
    return h_cutflow_eff


def create_rejection_rate_histogram(histogram):
    name = histogram.GetName() + "_rej_rate"
    title = histogram.GetTitle()
    n_bins = histogram.GetNbinsX() - 1
    h_rej_rate = ROOT.TEfficiency(name, title, n_bins, 0, n_bins)
    h_rej_rate.SetConfidenceLevel(0.68)
    h_rej_rate.SetStatisticOption(ROOT.TEfficiency.kFCP)
    n_prev = int(histogram.GetBinContent(1))
    for i in range(1, n_bins + 1):
        n_pass = int(histogram.GetBinContent(i + 1))
        h_rej_rate.SetTotalEvents(i, n_prev)
        h_rej_rate.SetPassedEvents(i, n_prev - n_pass)
        #h_rej_rate.SetBinContent(i, 1 - n_pass / n_prev)
        #h_rej_rate.GetXaxis().SetBinLabel(i, histogram.GetXaxis().GetBinLabel(i + 1))
        n_prev = n_pass
    return h_rej_rate


def create_efficiency_histograms(hlt_path, histograms):
    h_eff = {}
    for process in histograms.keys():
        if hlt_path not in histograms[process]:
            raise Exception("HLT path {} not found".format(hlt_path))
        h = histograms[process][hlt_path]
        h_eff[process] = {
            "cutflow_eff": create_cutflow_efficiency_histogram(h),
            "rej_rate": create_rejection_rate_histogram(h),
        }
    return h_eff


def plot(hlt_path, histograms_eff):
    hists = histograms_eff[hlt_path]

    c = ROOT.TCanvas("c", "c", 1024, 1024)
    c.Divide(1, 2)

    for i, process in enumerate(hists.keys()):

        c.cd(1)
        h = hists[process]["cutflow_eff"]
        options = ""
        if i > 0:
            options += "SAME"
        h.SetLineWidth(3)
        h.SetLineColor(COLORS[i])
        h.SetLineStyle(LINESTYLE[i])
        h.Draw(options)
        c.Update()

        c.cd(2)
        h = hists[process]["rej_rate"]
        #h.SetStats(0)
        h.SetLineWidth(3)
        h.SetLineColor(COLORS[i])
        h.SetLineStyle(LINESTYLE[i])
        h.Draw(options)
        c.Update()

    c.Update()

    c.Draw()

    c.SaveAs(hlt_path + ".png")


for final_state in ["TauTau", "ElTau", "MuTau"]:

    emb_filepath = os.path.abspath(os.path.join(__file__, "..", "..", "store", "HLTFilterPassAnalyzer", "TauEmbedding-" + final_state + "FinalState_Run2017F.root"))
    ztautau_filepath = os.path.abspath(os.path.join(__file__, "..", "..", "store", "HLTFilterPassAnalyzer", "ZTauTau-" + final_state + "FinalState_Run2017.root"))

    histograms = {
        "embedding": load_hists(emb_filepath),
        "ztautau": load_hists(ztautau_filepath),
    }

    histograms_eff = {}
    for hlt_path in HLT_PATHS[final_state]:
        histograms_eff[hlt_path] = create_efficiency_histograms(hlt_path, histograms)
        plot(hlt_path, histograms_eff)
