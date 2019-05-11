import FWCore.ParameterSet.Config as cms

process = cms.Process("Demo")

process.load("FWCore.MessageService.MessageLogger_cfi")

process.maxEvents = cms.untracked.PSet( input = cms.untracked.int32(-1) )

process.source = cms.Source("PoolSource",
                                # replace 'myfile.root' with the source file you want to use
                                fileNames = cms.untracked.vstring(
                                #'file:/homeui/rrossin/DT/CMSSW_10_2_14/test/slc6_amd64_gcc700/BXtest/step2.root'
                                'file:/homeui/rrossin/DT/CMSSW_10_2_13/test/slc6_amd64_gcc700/BXtest/step1_simmuonDTDigis16BX.root'
                )
                            )

process.demo = cms.EDAnalyzer('DemoAnalyzer',
                              genParticles = cms.InputTag("genParticles"),
                              simMuonDTDigis = cms.InputTag("simMuonDTDigis"),
                              )

process.TFileService = cms.Service('TFileService',
    fileName=cms.string("hist.root")
                                   )

process.p = cms.Path(process.demo)
