from collections import OrderedDict
import hashlib
import re
import os
import ROOT
from time import sleep

from ROOT import TCanvas, TH1D

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


DEFAULT_CANVAS_WIDTH = 1024
DEFAULT_CANVAS_HEIGHT = 1024


class Plot():

    def __init__(self, width=None, height=None, n_rows=None, n_cols=None):
        self._name = hashlib.md5()
        self._width = width or DEFAULT_CANVAS_WIDTH
        self._height = height or DEFAULT_CANVAS_HEIGHT
        self._title = "{};{};{}".format(self._name, self._width, self._height)
        self._n_rows = n_rows or 1
        self._n_cols = n_cols or 1
        self._canvas = self._create_canvas()
        self._subplots = self._get_subplots()
   
    def _create_canvas(self):
        canvas = TCanvas(self._name , self._title, self._width, self._height)
        if self._n_rows > 1 or self._n_cols > 1:
            canvas.Divide(self._n_rows, self._n_cols, 0.0, 0.0, 0)
        return canvas

    def _get_pads(self):
        subplots = []
        pad_index = 0
        for _ in range(self._n_rows):
            subplots.append([])
            for _ in range(self._n_cols):
                pad_index += 1
                subplot = Subplot(self, self._canvas.cd(pad_index))
                subplots[-1].append(subplot)
        return subplots


class SubplotMeta(type):

    def __new__(meta_cls, cls_name, bases, cls_dict):

        # add color table to the class dict
        cls_dict.setdefault("_colors", OrderedDict())
        cls_dict.setdefault("_color_index", 0)

        # create the class
        cls = super().__new__(meta_cls, cls_name, bases, cls_dict)

        # initialize the color table
        cls._initialize_color_table()

    def _initialize_color_table(cls):
        colors_hex = OrderedDict(
            blue="0173b2",
            orange="de8f05",
            green="029e73",
            red="d55e00",
            purple="cc78bc",
            brown="ca9161",
            pink="fbafe4",
            gray="949494",
            olive="ece133",
            cyan="56b4e9",
            black="000000",
            white="ffffff",
        )
        colors = OrderedDict()
        for name, hex_code in colors_hex.items():
            r, g, b = int(hex_code[0:2], 16), int(hex_code[2:4], 16), int(hex_code[4:6], 16)
            index = ROOT.TColor.GetFreeColorIndex()
            color = ROOT.TColor(index, r, g, b)
            colors[name] = color
        cls._colors = colors

    def get_next_color(cls):
        colors_list = list(cls._colors)
        color = colors_list[cls._color_index][1]
        cls._color_index += 1
        cls._color_index = cls._color_index % (len(cls._colors) - 2)
        return color


class Subplot():

    def __init__(self, plot, pad):
        self._name = hashlib.md5()
        self._plot = plot
        self._pad = pad
        self._hist_bkg = TH1D(self._name, self._name, 1, 0.0, 1.0)
        self._histograms = []

    def add_histogram(self, hist, linewidth=None, linecolor=None):
        linecolor = linecolor or 
        self._histograms.append(hist)


class Histogram():

    def __init__(self, root_hist, linewidth=None, linecolor=None):
        self._root_hist = root_hist.Clone()
        self._linewidth = linewidth or 3
        self._linecolor = 

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
