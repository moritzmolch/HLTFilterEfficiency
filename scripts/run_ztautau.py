import glob
import random
import os
import subprocess
import sys
import yaml


SAMPLE_DATABASE_PATH = os.path.abspath(os.path.join(__file__, "..", "..", "data", "sample_database_miniaod"))
OUTPUT_PATH = os.path.abspath(os.path.join(__file__, "..", "..", "store", "HLTFilterPassAnalyzer"))


N_FILES = 100


HLT_PATHS = {
    "ElTau": [
        "HLT_Ele27_WPTight_Gsf",
        "HLT_Ele32_WPTight_Gsf",
        "HLT_Ele32_WPTight_Gsf_DoubleL1EG",
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


def get_filelist(era, n_files=None):
    sample_file = list(glob.glob(os.path.join(
        SAMPLE_DATABASE_PATH,
        era,
        "dyjets",
        "*.yaml"
    )))[0]
    with open(sample_file, mode="r") as f:
        sample = yaml.safe_load(f)

    filelist = sample["filelist"]
    if n_files is not None:
        filelist = filelist[0:n_files]

    return filelist


def run(era, final_state, n_files=None):
    filelist = get_filelist(era, n_files=n_files)
    input_files = ",".join(filelist)
    output_file = os.path.join(OUTPUT_PATH, "ZTauTau-" + final_state + "FinalState_Run" + era + ".root")
    if not os.path.exists(os.path.dirname(output_file)):
        os.makedirs(os.path.dirname(output_file))
    output_file = "file://" + output_file
    hlt_paths = ",".join(HLT_PATHS[final_state])
    p = subprocess.Popen(
        "cmsRun ${CMSSW_BASE}/src/TauAnalysis/HLTFilterEfficiencyStudies/python/DrellYanTauTauGenFilter_HLTFilterPassAnalyzer_cfg.py inputFiles=" + input_files + " outputFile=" + output_file + " hltPaths=" + hlt_paths + " finalState=" + final_state.replace("Tau", "TauHad"),
        stdout=sys.stdout,
        stderr=sys.stdout,
        shell=True,
        env=os.environ,
        cwd="/work/mmolch/CMSSW_10_6_28/src",
    )
    out, err = p.communicate()


for final_state in ["TauTau", "ElTau", "MuTau"]:

    for era in ["2017", ]:
        run(era, final_state, n_files=N_FILES)
