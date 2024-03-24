import FWCore.ParameterSet.Config as cms


patTriggerUnpacker = cms.EDProducer(
    "PATTriggerObjectStandAloneUnpacker",
    patTriggerObjectsStandAlone=cms.InputTag("slimmedPatTrigger"),
    triggerResults=cms.InputTag("TriggerResults", "", "SIMembeddingHLT"),
    unpackFilterLabels=cms.bool(True),
)


tauTriggerNtuplizer = cms.EDAnalyzer(
    "TauTriggerNtuplizer",
    hltPathList=cms.untracked.vstring([]),
    pairElectrons=cms.InputTag("recoTauTauPairProducer", "pairElectrons"),
    pairMuons=cms.InputTag("recoTauTauPairProducer", "pairMuons"),
    pairTaus=cms.InputTag("recoTauTauPairProducer", "pairTaus"),
    tauTauGenParticles=cms.InputTag("tauTauGenParticlesProducer", "tauTauGenParticles"),
    triggerResults=cms.InputTag("TriggerResults", "", "SIMembeddingHLT"),
    triggerObjects=cms.InputTag("patTriggerUnpacker"),
    generator=cms.InputTag("generator"),
    isMC=cms.untracked.bool(False),
    isEmb=cms.untracked.bool(True),
)


tauTriggerNtuplizerSequence = cms.Sequence(patTriggerUnpacker + tauTriggerNtuplizer)
