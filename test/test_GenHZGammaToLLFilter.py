import FWCore.ParameterSet.Config as cms


# define the process
process = cms.Process("TestGenHZGammaToLLFilter")

# number of events being processed
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(5390))

# verbosity
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

# input files
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v9_ext1-v1/40000/3DF8E861-B3C8-9840-8706-741ECFCC114D.root"
    ),
)

# gen-level filter for tau tau pairs from DY production
process.load("TauAnalysis.HLTFilterEfficiencyStudies.GenHZGammaToLLFilter_cff")

# process path
process.p = cms.Path(process.genHZGammaToLLFilterSequence)
