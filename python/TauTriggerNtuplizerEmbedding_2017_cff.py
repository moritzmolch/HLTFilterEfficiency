import FWCore.ParameterSet.Config as cms


HLT_PATH_LIST = [
    "HLT_Ele27_WPTight_Gsf",
    "HLT_Ele32_WPTight_Gsf",
    "HLT_Ele32_WPTight_Gsf_DoubleL1EG",
    "HLT_Ele32_WPTight_Gsf_DoubleL1EG",
    "HLT_Ele35_WPTight_Gsf",
    "HLT_Ele24_eta2p1_WPTight_Gsf_LooseChargedIsoPFTau30_eta2p1_CrossL1",
    "HLT_IsoMu24",
    "HLT_IsoMu27",
    "HLT_IsoMu20_eta2p1_LooseChargedIsoPFTau27_eta2p1_CrossL1",
    "HLT_DoubleTightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleTightChargedIsoPFTau40_Trk1_eta2p1_Reg",
]


patTriggerUnpacker = cms.EDProducer(
    "PATTriggerObjectStandAloneUnpacker",
    patTriggerObjectsStandAlone=cms.InputTag("slimmedPatTrigger"),
    triggerResults=cms.InputTag("TriggerResults", "", "SIMembeddingHLT"),
    unpackFilterLabels=cms.bool(True),
)


tauTriggerNtuplizer = cms.EDAnalyzer(
    "TauTriggerNtuplizer",
    hltPathList=cms.untracked.vstring(HLT_PATH_LIST),
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
