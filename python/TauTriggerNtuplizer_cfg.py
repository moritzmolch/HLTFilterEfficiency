import FWCore.ParameterSet.Config as cms
import FWCore.ParameterSet.VarParsing as VarParsing


# define command-line options
options = VarParsing.VarParsing("analysis")
options.register(
    "datasetType",
    [],
    VarParsing.VarParsing.multiplicity.singleton,
    VarParsing.VarParsing.varType.string,
    "dataset type of the processed file, i.e. if it is a file from an embedding or a MC simulation dataset; possible values are 'emb' and 'mc'",
)
options.register(
    "hltPaths",
    [],
    VarParsing.VarParsing.multiplicity.list,
    VarParsing.VarParsing.varType.string,
    "list of HLT paths, which are regarded when processing these events",
)

# parse and validate the arguments
options.parseArguments()

dataset_type = options.datasetType
if dataset_type not in ["emb", "mc"]:
    raise ValueError("dataset type '{}' unknown; must be 'emb' or 'mc'".format(dataset_type))

hlt_paths = options.hltPaths

# break if the list of input files is too long
if len(options.inputFiles) > 255:
    raise RuntimeError(
        "the 'cms.Source' source object is not capable of taking more than 255 input files"
    )

# define the process
process = None
if dataset_type == "emb":
    process = cms.Process("TauTriggerNtuplizerEmbedding")
elif dataset_type == "mc":
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

# ntuplizer for writing out all generator weights
process.load("TauAnalysis.TauTriggerNtuples.GenWeightNtuplizer_cff")

# gen-level filter for tau tau pairs from DY production
process.load("TauAnalysis.TauTriggerNtuples.TauTauGenParticlesFilter_cff")

# reconstruction-level filter for tau tau pairs
process.load("TauAnalysis.TauTriggerNtuples.RecoTauTauPairFilter_cff")

# the ntuplizer plugin
if dataset_type == "emb":
    process.load("TauAnalysis.TauTriggerNtuples.TauTriggerNtuplizerEmbedding_cff")
elif dataset_type == "mc":
    process.load("TauAnalysis.TauTriggerNtuples.TauTriggerNtuplizerMC_cff")

# initialize the HLT path argument of the ntuplizer correctly
process.tauTriggerNtuplizer.hltPathList = cms.untracked.vstring(hlt_paths)

# service that provides the output file
process.TFileService = cms.Service(
    "TFileService",
    fileName=cms.string(options.outputFile),
)

# process path
process.p = cms.Path(
    process.genWeightNtuplizer
    + process.tauTauGenParticlesFilterSequence
    + process.rerunMvaIsolationSequence * getattr(process, updatedTauName)
    + process.recoTauTauPairFilterSequence
    + process.tauTriggerNtuplizerSequence
)
