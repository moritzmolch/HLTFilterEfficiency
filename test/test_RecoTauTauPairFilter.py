import FWCore.ParameterSet.Config as cms


# define the process
process = cms.Process("TestRecoTauTauPairFilter")

# number of events being processed
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(1000))

# verbosity
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 1

# input files
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17MiniAODv2/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v9_ext1-v1/40000/DEBA5603-4426-6640-A065-5E84C445B21D.root"
    ),
)

# re-run DeepTau
# https://twiki.cern.ch/twiki/bin/view/CMSPublic/SWGuidePFTauID#Running_of_the_DeepTauIDs_ver_20
updatedTauName = (
    "slimmedTausWithDeepTau2p1"  # name of pat::Tau collection with new tau-Ids
)
import RecoTauTag.RecoTau.tools.runTauIdMVA as tauIdConfig

tauIdEmbedder = tauIdConfig.TauIDEmbedder(
    process,
    cms,
    debug=False,
    updatedTauName=updatedTauName,
    toKeep=[
        "deepTau2017v2p1",  # deepTau TauIDs
    ],
)
tauIdEmbedder.runTauID()

# gen-level filter for tau tau pairs from DY production
process.load("TauAnalysis.HLTFilterEfficiencyStudies.RecoTauTauPairFilter_cff")

# process path
process.p = cms.Path(
    process.rerunMvaIsolationSequence * getattr(process, updatedTauName)
    + process.recoTauTauPairFilterSequence
)
