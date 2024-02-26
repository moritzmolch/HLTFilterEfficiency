import FWCore.ParameterSet.Config as cms


genWeightNtuplizer = cms.EDAnalyzer(
    "GenWeightNtuplizer",
    generator=cms.InputTag("generator"),
)
