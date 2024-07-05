#include <vector>
#include <string>
#include <algorithm>
#include <unordered_map>

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/Run.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/InputSourceDescription.h"
#include "FWCore/Framework/interface/ModuleContextSentry.h"
#include "FWCore/ServiceRegistry/interface/InternalContext.h"
#include "FWCore/ServiceRegistry/interface/ModuleCallingContext.h"
#include "FWCore/ServiceRegistry/interface/ParentContext.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/EDMException.h"

#include "FWCore/Sources/interface/VectorInputSourceFactory.h"
#include "FWCore/Sources/interface/VectorInputSource.h"

#include "FWCore/Utilities/interface/InputTag.h"
#include "FWCore/Utilities/interface/ESGetToken.h"

#include "DataFormats/Common/interface/Handle.h"
#include "DataFormats/Common/interface/ValueMap.h"
#include "DataFormats/JetReco/interface/GenJetCollection.h"
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

#include "Mixing/Base/interface/PileUp.h"
#include "SimGeneral/MixingModule/interface/PileUpEventPrincipal.h"
#include "SimDataFormats/CrossingFrame/interface/CrossingFramePlaybackInfoNew.h"

using namespace edm;
using namespace reco;
using namespace std;

namespace edm {
  class ParameterSet;
}

class GenPUJetExtractor : public edm::stream::EDProducer<> {
public:
  GenPUJetExtractor(const edm::ParameterSet&);
  ~GenPUJetExtractor() override;

  // void produce(edm::StreamID, edm::Event&, const edm::EventSetup&) const override;
  void produce(edm::Event&, const edm::EventSetup&);

  bool getGenJets(EventPrincipal const& ep,
                     ModuleCallingContext const*,
                     int bcr,
                     int id,
                     int& offset,
                     const edm::EventSetup& setup,
                     edm::StreamID const&);

  std::unique_ptr<GenJetCollection> outPileUpGenJets_;
  std::unique_ptr<std::vector<int>> outPileUpGenJetsEvtIdx_;
  std::unique_ptr<std::vector<math::XYZPointF>> outPileupVtx_XYZ_;
  std::unique_ptr<std::vector<float>> outPileupVtx_t_;

  std::unique_ptr<GenParticleCollection> outPileUpGenParticles_;
  std::unique_ptr<std::vector<int>>      outPileUpGenParticlesEvtIdx_;

private:
  // ----------member data ---------------------------
  int bunchIdx_;
  int nMaxPUEvent_;
  int puEvtCounter_;

  bool saveGenParticles_;

  edm::InputTag inputTagPlayback_;

  std::unique_ptr<PileUp> input_;
  InputTag tag_;
  InputTag tag_xyz0_;
  InputTag tag_t0_;
  InputTag tag_GenParticles_;
};


GenPUJetExtractor::GenPUJetExtractor(const ParameterSet& cfg): 
  bunchIdx_(cfg.getParameter<int>("bunchIdx")),
  nMaxPUEvent_(200),
  saveGenParticles_(cfg.getParameter<bool>("saveGenParticles")),
  tag_(InputTag("ak4GenJetsNoNu")),
  tag_xyz0_(InputTag("genParticles","xyz0")),
  tag_t0_(InputTag("genParticles","t0")),
  tag_GenParticles_(InputTag("genParticles"))
{
  //
  //
  //
  inputTagPlayback_ = InputTag("mix", "", edm::InputTag::kSkipCurrentProcess);
  consumes<CrossingFramePlaybackInfoNew>(inputTagPlayback_);
  //
  //
  //
  int averageNumber = 1;
  std::unique_ptr<TH1F> h(new TH1F("h", "h", 10, 0, 10));
  bool playback = true;
  auto conf = std::make_shared<PileUpConfig>("input", averageNumber, h, playback); // practically a dummy PileUpConfig
  input_ = std::make_unique<edm::PileUp>(cfg.getParameter<edm::ParameterSet>("input"), conf, consumesCollector(), false /*mixingConfigFromDB*/);

  //
  //
  //

  produces<GenJetCollection>("ak4GenJetsNoNuFromPU");
  produces<edm::ValueMap<int>>("ak4GenJetsNoNuFromPUEventIdx");
  produces<std::vector<math::XYZPointF>>("PUEventXYZ");
  produces<edm::ValueMap<float>>("PUEventT");
  if (saveGenParticles_){
    produces<GenParticleCollection>("genParticlesStatusOneFromPU");
    produces<edm::ValueMap<int>>("genParticlesStatusOneFromPUEventIdx");
  }
}

GenPUJetExtractor::~GenPUJetExtractor() {}

void GenPUJetExtractor::produce(Event& iEvent, const EventSetup& iSetup) {
  using namespace std::placeholders;

  //
  //
  //
  edm::Handle<CrossingFramePlaybackInfoNew> playbackInfo;
  iEvent.getByLabel(inputTagPlayback_, playbackInfo);

  //
  //
  //
  ModuleCallingContext const* mcc = iEvent.moduleCallingContext();
  std::vector<edm::SecondaryEventIDAndFileInfo> recordEventID;

  //
  //
  //
  size_t numberOfEvents = playbackInfo->getNumberOfEvents(bunchIdx_, 0);

  //
  //
  //
  int playbackCounter = 0;
  std::vector<SecondaryEventIDAndFileInfo>::const_iterator begin = playbackInfo->getEventId(playbackCounter);
  playbackCounter += numberOfEvents;
  std::vector<SecondaryEventIDAndFileInfo>::const_iterator end = playbackInfo->getEventId(playbackCounter);

  //
  //
  //
  outPileUpGenJets_ = std::make_unique<reco::GenJetCollection>();
  outPileUpGenJetsEvtIdx_ = std::make_unique<std::vector<int>>();
  outPileupVtx_XYZ_ = std::make_unique<std::vector<math::XYZPointF>>();
  outPileupVtx_t_ = std::make_unique<std::vector<float>>();
  outPileUpGenParticles_  = std::make_unique<reco::GenParticleCollection>();
  outPileUpGenParticlesEvtIdx_  = std::make_unique<std::vector<int>>();

  //
  //
  //
  int vertexOffset = 0;
  puEvtCounter_ = 0;
  input_->playPileUp(begin, end, recordEventID,std::bind(
      &GenPUJetExtractor::getGenJets, this, _1, mcc, bunchIdx_, _2, vertexOffset, std::ref(iSetup), iEvent.streamID()
    )
  );

  //
  //
  //
  edm::OrphanHandle<reco::GenJetCollection> orphanHandle = iEvent.put(std::move(outPileUpGenJets_),"ak4GenJetsNoNuFromPU");
  auto genJetPUAllIdxV = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler filler_genJetPUAllIdx(*genJetPUAllIdxV);
  filler_genJetPUAllIdx.insert(orphanHandle, outPileUpGenJetsEvtIdx_->begin(), outPileUpGenJetsEvtIdx_->end());
  filler_genJetPUAllIdx.fill();
  iEvent.put(std::move(genJetPUAllIdxV),"ak4GenJetsNoNuFromPUEventIdx");

  //
  //
  //
  edm::OrphanHandle<std::vector<math::XYZPointF>> orphanHandle2 = iEvent.put(std::move(outPileupVtx_XYZ_), "PUEventXYZ");
  auto t0PUEventV = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_t0PUEvent(*t0PUEventV);
  filler_t0PUEvent.insert(orphanHandle2, outPileupVtx_t_->begin(), outPileupVtx_t_->end());
  filler_t0PUEvent.fill();
  iEvent.put(std::move(t0PUEventV),"PUEventT");


  //
  //
  //
  if (saveGenParticles_){
    edm::OrphanHandle<reco::GenParticleCollection> orphanHandle3 = iEvent.put(std::move(outPileUpGenParticles_),"genParticlesStatusOneFromPU");
    auto genParticlesPUAllIdxV = std::make_unique<edm::ValueMap<int>>();
    edm::ValueMap<int>::Filler filler_genParticlesPUAllIdx(*genParticlesPUAllIdxV);
    filler_genParticlesPUAllIdx.insert(orphanHandle3, outPileUpGenParticlesEvtIdx_->begin(), outPileUpGenParticlesEvtIdx_->end());
    filler_genParticlesPUAllIdx.fill();
    iEvent.put(std::move(genParticlesPUAllIdxV),"genParticlesStatusOneFromPUEventIdx");
  }
}

bool GenPUJetExtractor::getGenJets(EventPrincipal const& eventPrincipal,
                                  ModuleCallingContext const* mcc,
                                  int bunchCrossing,
                                  int eventId,
                                  int& vertexOffset,
                                  const edm::EventSetup& setup,
                                  StreamID const& streamID) {
  InternalContext internalContext(eventPrincipal.id(), mcc);
  ParentContext parentContext(&internalContext);
  ModuleCallingContext moduleCallingContext(&moduleDescription());
  ModuleContextSentry moduleContextSentry(&moduleCallingContext, parentContext);

  if (puEvtCounter_ >= nMaxPUEvent_){
    puEvtCounter_++;
    return true;
  }

  std::shared_ptr<Wrapper<std::vector<reco::GenJet> > const> shPtr = getProductByTag<std::vector<reco::GenJet> >(eventPrincipal, tag_, &moduleCallingContext);

  if (shPtr) {
    auto prodColl = shPtr->product();
    for (size_t i = 0; i < prodColl->size(); i++){
      outPileUpGenJets_->emplace_back(prodColl->at(i));
      outPileUpGenJetsEvtIdx_->emplace_back(puEvtCounter_);
    }
  }else{
    std::cout << "Warning: no shPtr" << std::endl;
  }

  std::shared_ptr<Wrapper<math::XYZPointF> const> shPtr_xyz0 = getProductByTag<math::XYZPointF>(eventPrincipal, tag_xyz0_, &moduleCallingContext);
  if (shPtr_xyz0) {
    outPileupVtx_XYZ_->emplace_back(math::XYZPointF(*(shPtr_xyz0->product())));
  }else{
    std::cout << "Warning: no shPtr_xyz0" << std::endl;
  }

  std::shared_ptr<Wrapper<float> const> shPtr_t0 = getProductByTag<float>(eventPrincipal, tag_t0_, &moduleCallingContext);
  if (shPtr_xyz0) {
    outPileupVtx_t_->emplace_back(*(shPtr_t0->product()));
  }else{
    std::cout << "Warning: no shPtr_t0" << std::endl;
  }
  // std::cout << (*(shPtr_t0->product())) << std::endl;

  if (saveGenParticles_){
    std::shared_ptr<Wrapper<std::vector<reco::GenParticle> > const> shPtr_genParticles = getProductByTag<std::vector<reco::GenParticle> >(eventPrincipal, tag_GenParticles_, &moduleCallingContext);
    if (shPtr_genParticles) {
      auto prodColl_genParticles = shPtr_genParticles->product();
      for (size_t i = 0; i < prodColl_genParticles->size(); i++){
        if(prodColl_genParticles->at(i).status() != 1) continue;
        outPileUpGenParticles_->emplace_back(prodColl_genParticles->at(i));
        outPileUpGenParticlesEvtIdx_->emplace_back(puEvtCounter_);
      }
    }
    else{
      std::cout << "Warning: no shPtr_genParticles" << std::endl;
    }
  }

  puEvtCounter_++;
  return true;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(GenPUJetExtractor);
