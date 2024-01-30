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

options.parseArguments()

# break if the list of input files is too long
if len(options.inputFiles) > 255:
    raise RuntimeError(
        "the 'cms.Source' source object is not capable of taking more than 255 input files"
    )

# define the process
process = cms.Process("TauTriggerNtuplizerMC")

# number of events being processed
process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))

# verbosity
process.load("FWCore.MessageService.MessageLogger_cfi")
process.MessageLogger.cerr.FwkReport.reportEvery = 100

# input files
process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(*options.inputFiles),
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

# reconstruction-level filter for tau tau pairs
process.load("TauAnalysis.HLTFilterEfficiencyStudies.RecoTauTauPairFilter_cff")

# the ntuplizer plugin
process.load("TauAnalysis.HLTFilterEfficiencyStudies.TauTriggerNtuplizerMC_cff")

# service that provides the output file
process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(options.outputFile),
    name=cms.string("TauTriggerNtuple"),
)

# process path
process.p = cms.Path(
    process.rerunMvaIsolationSequence * getattr(process, updatedTauName)
    + process.genHZGammaToLLFilterSequence
    + process.recoTauTauPairFilterSequence
    + process.tauTriggerNtuplizerSequence
)
