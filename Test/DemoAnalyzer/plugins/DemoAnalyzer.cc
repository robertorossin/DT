// -*- C++ -*-
//
// Package:    Test/DemoAnalyzer
// Class:      DemoAnalyzer
//
/**\class DemoAnalyzer DemoAnalyzer.cc Test/DemoAnalyzer/plugins/DemoAnalyzer.cc

 Description: [one line class summary]

 Implementation:
     [Notes on implementation]
*/
//
// Original Author:  Roberto Rossin
//         Created:  Thu, 09 May 2019 08:15:17 GMT
//
//


// system include files
#include <memory>

// user include files
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/one/EDAnalyzer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
 #include "FWCore/Utilities/interface/InputTag.h"
 #include "DataFormats/TrackReco/interface/Track.h"
 #include "DataFormats/TrackReco/interface/TrackFwd.h"
 #include "DataFormats/HepMCCandidate/interface/GenParticle.h"
 #include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"
 #include "DataFormats/DTDigi/interface/DTDigiCollection.h"
#include "FWCore/MessageLogger/interface/MessageLogger.h"

 #include "TH1.h"
 #include "FWCore/ServiceRegistry/interface/Service.h"
 #include "CommonTools/UtilAlgos/interface/TFileService.h"
//
// class declaration
//

// If the analyzer does not use TFileService, please remove
// the template argument to the base class so the class inherits
// from  edm::one::EDAnalyzer<>
// This will improve performance in multithreaded jobs.


//using reco::TrackCollection;

class DemoAnalyzer : public edm::one::EDAnalyzer<edm::one::SharedResources>  {
public:
  explicit DemoAnalyzer(const edm::ParameterSet&);
  ~DemoAnalyzer();
  
  static void fillDescriptions(edm::ConfigurationDescriptions& descriptions);
  
  
private:
  virtual void beginJob() override;
  virtual void analyze(const edm::Event&, const edm::EventSetup&) override;
  virtual void endJob() override;
  
  
  
  // ----------member data ---------------------------
  //edm::EDGetTokenT<TrackCollection> tracksToken_;  //used to select what tracks to read from configuration file
  int nevents;
  edm::EDGetTokenT<reco::GenParticleCollection> genParticlesToken_;
  edm::EDGetTokenT<DTDigiCollection> simMuonDTDigisToken_;
  TH1F *h_nGen_;
  TH1F *h_simMuonDTDigis_countsTDC_;
  TH1F *h_simMuonDTDigis_time_;
};

//
// constants, enums and typedefs
//

//
// static data member definitions
//

//
// constructors and destructor
//
DemoAnalyzer::DemoAnalyzer(const edm::ParameterSet& iConfig)  :
  nevents(0),
  genParticlesToken_  (consumes<reco::GenParticleCollection>(edm::InputTag{"genParticles"})),
  simMuonDTDigisToken_(consumes<DTDigiCollection>(edm::InputTag{"simMuonDTDigis"}))
			  //simMuonDTDigisToken_(consumes<DTDigiCollection>(iConfig.getUntrackedParameter<std::string>("simMuonDTDigis")))


{
   //now do what ever initialization is needed
  std::cout << "DemoAnalyzer" << std::endl;

  edm::Service<TFileService> fileService;
  if(!fileService){
    throw edm::Exception( edm::errors::Configuration,
			  "TFileService is not registered in cfg file" );
  }

  TFileDirectory histo = fileService->mkdir( "histo" );
  h_nGen_                     = histo.make<TH1F>("nGenParticles"           , "nGenParticles"           ,    300,  0, 3000);
  h_simMuonDTDigis_time_      = histo.make<TH1F>("simMuonDTDigis_time"     , "simMuonDTDigis_time"     ,    400,  0, 2000);
  h_simMuonDTDigis_countsTDC_ = histo.make<TH1F>("simMuonDTDigis_countsTDC", "simMuonDTDigis_countsTDC",    400,  0, 2000);
}

DemoAnalyzer::~DemoAnalyzer()
{

   // do anything here that needs to be done at desctruction time
   // (e.g. close files, deallocate resources etc.)

}


//
// member functions
//

// ------------ method called for each event  ------------
void
DemoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup)
{
   using namespace edm;

   nevents++;

   edm::Handle<reco::GenParticleCollection> genParticles;
   iEvent.getByToken(genParticlesToken_, genParticles);
   LogInfo("Demo") << "number of genParticles "<<genParticles->size();

   h_nGen_->Fill(genParticles->size());
   for(reco::GenParticleCollection::const_iterator itGen = genParticles->begin();
       itGen != genParticles->end();
       ++itGen) {
     // do something with track parameters, e.g, plot the charge.
   }

   // CalibMuon/DTCalibration/plugins/DTT0Calibration.cc
   // Get the digis from the event
   Handle<DTDigiCollection> digis; 
   iEvent.getByToken(simMuonDTDigisToken_, digis);

   // Get the DT Geometry
//    if (nevents == 1)
//      iSetup.get<MuonGeometryRecord>().get(dtGeom);
 
   // Iterate through all digi collections ordered by LayerId   
   for (const auto& digis_per_layer : *digis)
     {
       // Get the iterators over the digis associated with this LayerId
       const DTDigiCollection::Range& digiRange = digis_per_layer.second;
 
       // Get the layerId
//        const DTLayerId layerId = digis_per_layer.first;
       //const DTChamberId chamberId = layerId.superlayerId().chamberId();
 
//        if((theCalibWheel != "All") && (layerId.superlayerId().chamberId().wheel() != selWheel))
//           continue;
//        if((theCalibSector != "All") && (layerId.superlayerId().chamberId().sector() != selSector))
//           continue;
 
       // Loop over all digis in the given layer
       for (DTDigiCollection::const_iterator digi = digiRange.first;
             digi != digiRange.second;
             ++digi)
  {
    const double time      = digi->time();
    const double countsTDC = digi->countsTDC();
//     const DTWireId wireIdtmp(layerId, (*digi).wire());
    h_simMuonDTDigis_time_     ->Fill(time       );
    h_simMuonDTDigis_countsTDC_->Fill(countsTDC);
  }

     }

//    edm::Handle<DTDigiCollection> dtDigis;
//    iEvent.getByToken(simMuonDTDigisToken_, dtDigis);
//    for(DTDigiCollection::const_iterator itDTDigi = dtDigis->begin();
//        itDTDigi != dtDigis->end();
//        ++itDTDigi) {
//      // do something with track parameters, e.g, plot the charge.
//    }
    

#ifdef THIS_IS_AN_EVENT_EXAMPLE
   Handle<ExampleData> pIn;
   iEvent.getByLabel("example",pIn);
#endif

#ifdef THIS_IS_AN_EVENTSETUP_EXAMPLE
   ESHandle<SetupData> pSetup;
   iSetup.get<SetupRecord>().get(pSetup);
#endif
}


// ------------ method called once each job just before starting event loop  ------------
void
DemoAnalyzer::beginJob()
{
}

// ------------ method called once each job just after ending the event loop  ------------
void
DemoAnalyzer::endJob()
{
}

// ------------ method fills 'descriptions' with the allowed parameters for the module  ------------
void
DemoAnalyzer::fillDescriptions(edm::ConfigurationDescriptions& descriptions) {
  //The following says we do not know what parameters are allowed so do no validation
  // Please change this to state exactly what you do use, even if it is no parameters
  edm::ParameterSetDescription desc;
  desc.setUnknown();
  descriptions.addDefault(desc);

  //Specify that only 'tracks' is allowed
  //To use, remove the default given above and uncomment below
  //ParameterSetDescription desc;
  //desc.addUntracked<edm::InputTag>("tracks","ctfWithMaterialTracks");
  //descriptions.addDefault(desc);
}

//define this as a plug-in
DEFINE_FWK_MODULE(DemoAnalyzer);
