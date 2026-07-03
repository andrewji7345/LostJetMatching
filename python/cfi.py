import FWCore.ParameterSet.Config as cms

lostJetMatching = cms.EDAnalyzer(
    "LostJetMatching",

    genParticles = cms.InputTag("prunedGenParticles"),

    jets = cms.InputTag("slimmedJets"),

    fatjets = cms.InputTag("slimmedJetsAK8"),

    chiID = cms.int32(9936662)
)