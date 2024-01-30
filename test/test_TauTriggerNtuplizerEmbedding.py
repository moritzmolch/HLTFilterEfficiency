import FWCore.ParameterSet.Config as cms


# define the process
process = cms.Process("TestRecoTauTauPairFilter")

# number of events being processed
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(5000))

# verbosity
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

# input files
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "root://cmsxrootd-kit-disk.gridka.de:1094///store/user/sbrommer/ul_embedding/large_miniAOD_v2/MuTauFinalState/EmbeddingRun2017B/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/6151aac9-e212-4ba2-a26b-01c719750776.root",
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
process.load("TauAnalysis.HLTFilterEfficiencyStudies.GenHZGammaToLLFilter_cff")
process.load("TauAnalysis.HLTFilterEfficiencyStudies.RecoTauTauPairFilter_cff")
process.load("TauAnalysis.HLTFilterEfficiencyStudies.TauTriggerNtuplizerEmbedding_cff")

# service that provides the output file
process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string("file:./test_TauTriggerNtuplizerEmbedding.root"),
    name=cms.string("TauTriggerNtuple"),
)

# process path
process.p = cms.Path(
    process.rerunMvaIsolationSequence * getattr(process, updatedTauName)
    + process.recoTauTauPairFilterSequence
    + process.tauTriggerNtuplizerSequence
)
