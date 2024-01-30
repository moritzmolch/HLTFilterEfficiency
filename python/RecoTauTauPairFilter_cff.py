import FWCore.ParameterSet.Config as cms

# calculate custom isolation of the electron
# from https://github.com/cms-sw/cmssw/blob/CMSSW_10_6_X/PhysicsTools/NanoAOD/python/electrons_cff.py


isoForEle = cms.EDProducer(
    "EleIsoValueMapProducer",
    src=cms.InputTag("slimmedElectrons"),
    relative=cms.bool(False),
    rho_MiniIso=cms.InputTag("fixedGridRhoFastjetAll"),
    rho_PFIso=cms.InputTag("fixedGridRhoFastjetAll"),
    EAFile_MiniIso=cms.FileInPath(
        "RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"
    ),
    EAFile_PFIso=cms.FileInPath(
        "RecoEgamma/ElectronIdentification/data/Fall17/effAreaElectrons_cone03_pfNeuHadronsAndPhotons_94X.txt"
    ),
)


# customize electron ID and isolation
slimmedElectronsWithUserData = cms.EDProducer(
    "PATElectronUserDataEmbedder",
    src=cms.InputTag("slimmedElectrons"),
    userFloats=cms.PSet(
        PFIsoAll=cms.InputTag("isoForEle:PFIsoAll"),
    ),
)


# select good electrons for tau tau pair selection
slimmedElectronsForTauTauPair = cms.EDFilter(
    "PATElectronSelector",
    src=cms.InputTag("slimmedElectronsWithUserData"),
    cut=cms.string(
        "(pt > 20) && (abs(eta) < 2.5)"
        + ' && (abs(dB("PV2D")) < 0.045) && (abs(dB("PVDZ")) < 0.2)'
        + ' && (userFloat("PFIsoAll")/pt < 0.15)'
        + ' && (electronID("mvaEleID-Fall17-noIso-V1-wp90") > 0.5)'
    ),
)


# select veto electrons for tau tau pair selection
vetoElectronsForTauTauPair = cms.EDFilter(
    "PATElectronSelector",
    src=cms.InputTag("slimmedElectronsWithUserData"),
    cut=cms.string(
        "(pt > 10) && (abs(eta) < 2.5)"
        + ' && (abs(dB("PV2D")) < 0.045) && (abs(dB("PVDZ")) < 0.2)'
        + ' && (userFloat("PFIsoAll")/pt < 0.3)'
        + ' && (electronID("mvaEleID-Fall17-noIso-V1-wp90") > 0.5)'
    ),
)


# select good muons for tau tau pair selection
slimmedMuonsForTauTauPair = cms.EDFilter(
    "PATMuonSelector",
    src=cms.InputTag("slimmedMuons"),
    cut=cms.string(
        "(pt > 20) && (abs(eta) < 2.4)"
        + ' && (abs(dB("PV2D")) < 0.045) && (abs(dB("PVDZ")) < 0.2)'
        + " && ((pfIsolationR04().sumChargedHadronPt + max(pfIsolationR04().sumNeutralHadronEt + pfIsolationR04().sumPhotonEt - 0.5 * pfIsolationR04().sumPUPt, 0.0)) / pt() < 0.15)"
        + " && (isMediumMuon)"
    ),
)


# select veto muons for tau tau pair selection
vetoMuonsForTauTauPair = cms.EDFilter(
    "PATMuonSelector",
    src=cms.InputTag("slimmedMuons"),
    cut=cms.string(
        "(pt > 10) && (abs(eta) < 2.4)"
        + ' && (abs(dB("PV2D")) < 0.045) && (abs(dB("PVDZ")) < 0.2)'
        + " && ((pfIsolationR04().sumChargedHadronPt + max(pfIsolationR04().sumNeutralHadronEt + pfIsolationR04().sumPhotonEt - 0.5 * pfIsolationR04().sumPUPt, 0.0)) / pt() < 0.3)"
        + " && (isMediumMuon)"
    ),
)

# select good taus for tau tau pair selection
slimmedTausForTauTauPair = cms.EDFilter(
    "PATTauSelector",
    src=cms.InputTag("slimmedTausWithDeepTau2p1"),
    cut=cms.string(
        "(pt > 20) && (eta < 2.4)"
        + " && ((decayMode == 0) || (decayMode == 1) || (decayMode == 10) || (decayMode == 11))"
        + ' && (tauID("byVVLooseDeepTau2017v2p1VSe") > 0.5)'
        + ' && (tauID("byVLooseDeepTau2017v2p1VSmu") > 0.5)'
        + ' && (tauID("byMediumDeepTau2017v2p1VSjet") > 0.5)'
    ),
)


# the producer for selecting the tau tau pair final state
recoTauTauPairProducer = cms.EDProducer(
    "RecoTauTauPairProducer",
    electrons=cms.InputTag("slimmedElectronsForTauTauPair"),
    muons=cms.InputTag("slimmedMuonsForTauTauPair"),
    taus=cms.InputTag("slimmedTausForTauTauPair"),
    vetoElectrons=cms.InputTag("vetoElectronsForTauTauPair"),
    vetoMuons=cms.InputTag("vetoMuonsForTauTauPair"),
)


# the filter for all events with a valid tau tau pair
recoTauTauPairFilter = cms.EDFilter(
    "RecoTauTauPairFilter",
    pairElectrons=cms.InputTag("recoTauTauPairProducer", "pairElectrons"),
    pairMuons=cms.InputTag("recoTauTauPairProducer", "pairMuons"),
    pairTaus=cms.InputTag("recoTauTauPairProducer", "pairTaus"),
    filter=cms.bool(True),
)


recoTauTauPairFilterSequence = cms.Sequence(
    isoForEle
    + slimmedElectronsWithUserData
    + slimmedElectronsForTauTauPair
    + vetoElectronsForTauTauPair
    + slimmedMuonsForTauTauPair
    + vetoMuonsForTauTauPair
    + slimmedTausForTauTauPair
    + recoTauTauPairProducer
    + recoTauTauPairFilter
)
