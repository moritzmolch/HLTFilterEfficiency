import FWCore.ParameterSet.Config as cms


tauTauGenParticlesProducer = cms.EDProducer(
    "TauTauGenParticlesProducer",
    genParticles=cms.InputTag("prunedGenParticles"),
)


tauTauGenParticlesFilter = cms.EDFilter(
    "TauTauGenParticlesFilter",
    tauTauGenParticles=cms.InputTag("tauTauGenParticlesProducer", "tauTauGenParticles"),
    filter=cms.bool(True),
)

tauTauGenParticlesFilterSequence = cms.Sequence(
    tauTauGenParticlesProducer + tauTauGenParticlesFilter
)
