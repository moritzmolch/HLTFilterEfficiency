import FWCore.ParameterSet.Config as cms


patTriggerUnpacker = cms.EDProducer(
    "PATTriggerObjectStandAloneUnpacker",
    patTriggerObjectsStandAlone=cms.InputTag("slimmedPatTrigger"),
    triggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    unpackFilterLabels=cms.bool(True),
)


tauTriggerNtuplizer = cms.EDAnalyzer(
    "TauTriggerNtuplizer",
    hltPathList=cms.untracked.vstring([]),
    pairElectrons=cms.InputTag("recoTauTauPairProducer", "pairElectrons"),
    pairMuons=cms.InputTag("recoTauTauPairProducer", "pairMuons"),
    pairTaus=cms.InputTag("recoTauTauPairProducer", "pairTaus"),
    tauTauGenParticles=cms.InputTag("tauTauGenParticlesProducer", "tauTauGenParticles"),
    triggerResults=cms.InputTag("TriggerResults", "", "HLT"),
    triggerObjects=cms.InputTag("patTriggerUnpacker"),
    generator=cms.InputTag("generator"),
    isMC=cms.untracked.bool(True),
    isEmb=cms.untracked.bool(False),
)


tauTriggerNtuplizerSequence = cms.Sequence(patTriggerUnpacker + tauTriggerNtuplizer)
