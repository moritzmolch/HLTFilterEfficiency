import FWCore.ParameterSet.Config as cms


genHZGammaToLLProducer = cms.EDProducer(
    "GenHZGammaToLLProducer",
    genParticles=cms.InputTag("prunedGenParticles"),
)


genHZGammaToLLFilter = cms.EDFilter(
    "GenHZGammaToLLFilter",
    slimmedGenParticles=cms.InputTag("genHZGammaToLLProducer", "slimmedGenParticles"),
    filter=cms.bool(True),
)

genHZGammaToLLFilterSequence = cms.Sequence(
    genHZGammaToLLProducer + genHZGammaToLLFilter
)
