import FWCore.ParameterSet.Config as cms

process = cms.Process("MATCH")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet(
    input = cms.untracked.int32(-1)
)

process.source = cms.Source(
    "PoolSource",
    fileNames = cms.untracked.vstring(
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/70000/8A20D274-79E8-9744-BF1E-692746267B49.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/2550000/FFF0F47A-E840-6F4F-8482-146BC8CEAC71.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBZTToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/2550000/12336D4E-4C57-BB49-A8DF-620717523595.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBZTToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/70000/DB98A3AF-F5E2-5A48-A34D-79C0CAD97491.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBHTToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/01031B3D-9DE4-B841-ACC0-05043577F0F4.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBHTToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/2550000/4A877E13-83DC-F746-9AE1-72E78F08F9F0.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToZTZTToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/02F5A7F2-BE6B-8545-981B-E77F435324EF.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToZTZTToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/70000/40DCE5E4-7D9A-5D4B-A340-69B0175E49AC.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToHTZTToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/80000/01C81C33-9B8B-5248-A31B-A23739947485.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToHTZTToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/2550000/A79592A2-8156-C34B-8A27-1F22E07F2B1B.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToHTHTToJets_MSuu-4000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/5734DEE6-8D26-704A-BFA9-E8976034C7F0.root"
        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToHTHTToJets_MSuu-8000_MChi-2000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/2ED3092B-B19E-1745-9E66-70D966F75F0B.root"

        # "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-8000_MChi-1000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/D45496EA-E030-E540-BEF1-9820D2E3DC96.root"
        #"/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-8000_MChi-1500_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v1/70000/8F77E931-68BC-2646-9C2C-C2432767FB7B.root"
        #"/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-8000_MChi-2500_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v3/50000/BB8D88B0-1681-8A42-B3D2-169AF068EC82.root"
         "/store/mc/RunIISummer20UL17MiniAODv2/SuuToChiChiToWBWBToJets_MSuu-8000_MChi-3000_TuneCP5_13TeV-madgraph-pythia8/MINIAODSIM/106X_mc2017_realistic_v9-v2/2550000/91429315-C6A2-2B4B-81C3-3EF62A53A03F.root"
    )
)

process.TFileService = cms.Service(
    "TFileService",
    #fileName = cms.string("rootfiles_lostJetMatching/WbWb_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/WbWb_8000_2000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/WbZt_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/WbZt_8000_2000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/Wbht_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/Wbht_8000_2000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/ZtZt_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/ZtZt_8000_2000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/htZt_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/htZt_8000_2000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/htht_4000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/htht_8000_2000_LostJetMatching.root")

    #fileName = cms.string("rootfiles_lostJetMatching/WbWb_8000_1000_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/WbWb_8000_1500_LostJetMatching.root")
    #fileName = cms.string("rootfiles_lostJetMatching/WbWb_8000_2500_LostJetMatching.root")
    fileName = cms.string("rootfiles_lostJetMatching/WbWb_8000_3000_LostJetMatching.root")
)

process.load("SuuAnalysis.LostJetMatching.cfi")

process.MessageLogger.cerr.FwkReport.reportEvery = 1000

process.p = cms.Path(
    process.lostJetMatching
)