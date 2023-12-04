import FWCore.ParameterSet.Config as cms
from TauAnalysis.HLTFilterEfficiency.HLTFilterPassAnalyzer_cfg import process


process.drellYanGenTauTauFilter = cms.EDFilter(
    "DrellYanGenTauTauFilter",
    genparticles=cms.InputTag("prunedGenParticles", "", "PAT"),
)


process.p = cms.Path(process.drellYanGenTauTauFilter + process.hltFilterPassAnalyzer)
