import FWCore.ParameterSet.Config as cms


hlt_paths = [
    "HLT_DoubleTightChargedIsoPFTau35_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleMediumChargedIsoPFTau40_Trk1_TightID_eta2p1_Reg",
    "HLT_DoubleTightChargedIsoPFTau40_Trk1_eta2p1_Reg",
]


process = cms.Process("HLTFilterEfficiencyAnalyzer")


process.load("FWCore.MessageService.MessageLogger_cfi")
process.maxEvents = cms.untracked.PSet(input=cms.untracked.int32(-1))


process.source = cms.Source(
    "PoolSource",
    fileNames=cms.untracked.vstring(
        #"root://xrootd-cms.infn.it///store/mc/RunIISummer20UL17MiniAOD/DYJetsToLL_M-50_TuneCP5_13TeV-madgraphMLM-pythia8/MINIAODSIM/106X_mc2017_realistic_v6-v1/280000/00085E9D-5FBB-B840-82A6-D5CE33C0F4C4.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/9e5a127b-69bf-4873-b362-c2f5ed65f8a1.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/291dc5b9-1b75-4f45-9374-cb5b8cb3aebd.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/7a68552d-334d-4ff0-b312-86d04775392d.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/f78dfd4a-a9b1-48d5-b65a-de53193b134a.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/e62a2a75-e3b5-44b8-8ba0-2fc5fc6578ec.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/cde7bb5d-a8c5-414d-8c20-fe22979ebb43.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/0277d1b9-a619-462f-b515-e62fdd85cc3c.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/29fe1eb2-e94c-4b06-b18f-ead15cf239a4.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/597d0c2a-5194-43a2-b1be-ec1c0a179dc3.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/1345dfe3-283d-438c-8617-425a5dff9915.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/26ba62b8-4c4f-4629-8136-2149a6450f9e.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/9923eeb7-bf80-4d3d-ad77-642019f6dfbb.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/577739b5-8394-4f63-b182-17ee8ea1fe92.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/8d375638-79c2-49f4-b7f8-4ccc71937a85.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/59e894ce-c600-468a-93c1-8592f226c669.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/9963da3f-d054-4754-9aef-784f6db0ac61.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/7a81063f-d501-4119-ad70-a25d9b8a8404.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/5969f3f8-16fe-4a18-8a8d-c705211dd352.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/5122bd93-b3e1-4246-ab0e-2541e09245dd.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/dd0c3d7e-8481-41fa-ad1f-0691db2a6b8b.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/4d19f9f5-1fda-4fe9-b7fd-8ce112c1e550.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/b5a4c7a8-bf82-40ed-9911-f14d4cacf306.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/1fe5c394-2b6f-40a0-a664-f04f2b794c73.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/8cc532e8-a377-4f83-9362-249c659db246.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/bf64ceb2-6036-4307-a42a-be3f489f0e42.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/70297468-244b-466b-831d-f4ee73bca858.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/8c5be872-d703-468d-b676-7274902984b6.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/d4af1a56-74cc-4778-af3a-81b4f6a12526.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/e20cc1ca-ccdc-4600-ae0e-06b2591dac8a.root",
        "root://cmsxrootd-kit.gridka.de///store/user/sbrommer/ul_embedding/large_miniAOD_v2/TauTauFinalState/EmbeddingRun2017F/MINIAOD/inputDoubleMu_106X_ULegacy_miniAOD-v1/0000/ed54dd24-d359-46b4-92ec-ab827b232467.root",
    ),
)


process.hltFilterEfficiencyAnalyzer = cms.EDAnalyzer(
    "HLTFilterEfficiencyAnalyzer",
    hlt_paths=cms.untracked.vstring(*hlt_paths),
    results=cms.InputTag("TriggerResults", "", "SIMembeddingHLT"),
    prescales=cms.InputTag("patTrigger", "", "MERGE"),
)


process.TFileService = cms.Service("TFileService",
    fileName = cms.string("histos.root")
)


process.p = cms.Path(process.hltFilterEfficiencyAnalyzer)
