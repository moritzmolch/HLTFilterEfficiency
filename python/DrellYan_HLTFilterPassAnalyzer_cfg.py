import FWCore.ParameterSet.Config as cms


hlt_paths = [
    "HLT_DoubleTightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleTightChargedIsoPFTau40_Trk1_eta2p1_Reg",
]


process = cms.Process("HLTFilterPassAnalyzer")


process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(10000))
process.MessageLogger.cerr.FwkReport.reportEvery = 1


process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        "file:///work/mmolch/CMSSW_10_6_28/src/DY.root",
    ),
)


process.drellYanGenTauTauFilter = cms.EDFilter(
    "DrellYanGenTauTauFilter",
    genparticles=cms.InputTag("prunedGenParticles", "", "PAT"),
)


process.hltFilterPassAnalyzer = cms.EDAnalyzer(
    "HLTFilterPassAnalyzer",
    hlt_paths=cms.untracked.vstring(*hlt_paths),
    results=cms.InputTag("TriggerResults", "", "HLT"),
    prescales=cms.InputTag("patTrigger", "", "PAT"),
)


process.TFileService = cms.Service("TFileService",
    fileName = cms.string("TauAnalysis/HLTFilterEfficiency/histos.root")
)


process.p = cms.Path(process.drellYanGenTauTauFilter + process.hltFilterPassAnalyzer)
