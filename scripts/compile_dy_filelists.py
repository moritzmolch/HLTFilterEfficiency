import json
import os
import subprocess
import yaml


SAMPLE_DATABASE_PATH = os.path.abspath(os.path.join(__file__, "..", "..", "data", "sample_database_miniaod"))

DAS_KEYS = {
    "2016preVFP": "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL16MiniAODAPVv2-106X_mcRun2_asymptotic_preVFP_v11-v1/MINIAODSIM",
    "2016postVFP": "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL16MiniAODv2-106X_mcRun2_asymptotic_v17-v1/MINIAODSIM",
    "2017": "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL17MiniAODv2-106X_mc2017_realistic_v9_ext1-v1/MINIAODSIM",
    "2018": "/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/RunIISummer20UL18MiniAODv2-106X_upgrade2018_realistic_v16_L1v1-v2/MINIAODSIM",
}


for era in DAS_KEYS.keys():
    name = DAS_KEYS[era].split("/")[1]
    camp_cond = DAS_KEYS[era].split("/")[2].split("_")[0]
    nick = name + "_" + camp_cond
    db_entry = {
        "dbs": DAS_KEYS[era],
        "era": era,
        "filelist": [],
        "generator_weight": 1.0,
        "nevents": 0,
        "nfiles": 0,
        "nick": nick,
        "sample_type": "dyjets",
        "xsec": 6077.22,
    }

    p = subprocess.Popen(
        "dasgoclient -query=\"file dataset=" + DAS_KEYS[era] + "\" -json",
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        env=os.environ,
        shell=True,
    )
    out, err = p.communicate()

    response = json.loads(out.decode("utf-8"))

    for item in response:
        assert len(item["file"]) == 1
        file_info = item["file"][0]
        db_entry["filelist"].append("root://xrootd-cms.infn.it//" + file_info["name"])
        db_entry["nevents"] += file_info["nevents"]
        db_entry["nfiles"] += 1

    db_path = os.path.join(SAMPLE_DATABASE_PATH, era, "dyjets", name + ".yaml")
    if not os.path.exists(os.path.dirname(db_path)):
        os.makedirs(os.path.dirname(db_path))
    with open(db_path, mode="w") as f:
        yaml.safe_dump(db_entry, f)
