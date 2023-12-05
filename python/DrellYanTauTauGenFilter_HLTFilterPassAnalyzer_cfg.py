import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


# define command-line options
options = VarParsing.VarParsing("analysis")
options.register(
    "hltPaths",
    [],
    VarParsing.VarParsing.multiplicity.list,
    VarParsing.VarParsing.varType.string,
    "Names of the high-level trigger paths that are studied",
)
options.register(
    "triggerResultsProcess",
    "HLT",
     VarParsing.VarParsing.multiplicity.singleton,
     VarParsing.VarParsing.varType.string,
    "name of the process producing the information about high-level trigger decisions",
)
options.register(
    "triggerPrescalesProcess",
    "PAT",
     VarParsing.VarParsing.multiplicity.singleton,
     VarParsing.VarParsing.varType.string,
    "name of the process producing the information about high-level trigger prescales",
)
options.register(
    "finalState",
    "all",
     VarParsing.VarParsing.multiplicity.singleton,
     VarParsing.VarParsing.varType.string,
    "name of the considered final state; can be 'all', 'ElTau', 'MuTau', 'TauTau', 'ElMu', 'ElEl', 'MuMu'",
)
options.parseArguments()


# break if the list of input files is too long
if len(options.inputFiles) > 255:
    raise RuntimeError("the 'cms.Source' source object is not capable of taking more than 255 input files")


process = cms.Process("HLTFilterPassAnalyzer")


process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))


process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(*options.inputFiles),
)


process.drellYanGenTauTauFilter = cms.EDFilter(
    "DrellYanGenTauTauFilter",
    genparticles=cms.InputTag("prunedGenParticles", "", "PAT"),
    finalState=cms.untracked.string("all"),
)


process.hltFilterPassAnalyzer = cms.EDAnalyzer(
    "HLTFilterPassAnalyzer",
    hlt_paths=cms.untracked.vstring(*options.hltPaths),
    results=cms.InputTag("TriggerResults", "", options.triggerResultsProcess),
    prescales=cms.InputTag("patTrigger", "", options.triggerPrescalesProcess),
)


process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(options.outputFile),
)


process.p = cms.Path(process.drellYanGenTauTauFilter + process.hltFilterPassAnalyzer)
