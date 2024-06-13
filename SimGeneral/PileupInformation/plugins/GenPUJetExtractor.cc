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

  std::unique_ptr<GenJetCollection> outGenJetsPUAll_;
  std::vector<int> outGenJetsPUAllIdx_;
  // std::unique_ptr<std::vector<math::XYZPointF>> outXYZ0PUAll_;

  std::vector<math::XYZPointF> outXYZ0PUAll_;
  edm::EDPutTokenT<std::vector<math::XYZPointF>> ptokenXYZ0PUAll_;

  std::vector<float> outt0PUAll_;

  std::vector<std::unique_ptr<GenJetCollection>> vecOutGenJets_;
  std::vector<std::unique_ptr<math::XYZPointF>>  vecOutXYZ0_;
  std::vector<std::unique_ptr<float>>            vecOutt0_;

private:
  // ----------member data ---------------------------
  int minBunch_;
  int maxBunch_;
  int nMaxPUEvent_;
  int puEvtCounter_;

  edm::InputTag inputTagPlayback_;

  std::unique_ptr<PileUp> input_;
  InputTag tag_;
  InputTag tag_xyz0_;
  InputTag tag_t0_;
};


GenPUJetExtractor::GenPUJetExtractor(const ParameterSet& cfg): 
  minBunch_(0),maxBunch_(0),nMaxPUEvent_(100),
  tag_(InputTag("ak4GenJetsNoNu")),
  tag_xyz0_(InputTag("genParticles","xyz0")),
  tag_t0_(InputTag("genParticles","t0"))
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
  for(int iPU=0; iPU < nMaxPUEvent_; iPU++){
    produces<GenJetCollection>("ak4GenJetsNoNuPUEvent"+std::to_string(iPU));
    produces<math::XYZPointF>("xyz0PUEvent"+std::to_string(iPU));
    produces<float>("t0PUEvent"+std::to_string(iPU));
  }

  produces<GenJetCollection>("ak4GenJetsNoNuFromPU");
  produces<edm::ValueMap<int>>("ak4GenJetsNoNuFromPUEventIdx");
  ptokenXYZ0PUAll_ = produces<std::vector<math::XYZPointF>>("xyz0PUEvent");
  produces<edm::ValueMap<float>>("t0PUEvent");

  // produces<std::vector<math::XYZPointF>>("xyz0PUEvent");

  
}

GenPUJetExtractor::~GenPUJetExtractor() {}

void GenPUJetExtractor::produce(Event& iEvent, const EventSetup& iSetup) {
  using namespace std::placeholders;

  //
  //
  //
  edm::Handle<CrossingFramePlaybackInfoNew> playbackInfo;
  bool got = iEvent.getByLabel(inputTagPlayback_, playbackInfo);

  //
  //
  //
  ModuleCallingContext const* mcc = iEvent.moduleCallingContext();
  std::vector<edm::SecondaryEventIDAndFileInfo> recordEventID;

  //
  //
  //
  int bunchIdx = 0;
  size_t numberOfEvents = playbackInfo->getNumberOfEvents(bunchIdx, 0);

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
  outGenJetsPUAll_ = std::make_unique<reco::GenJetCollection>();
  outGenJetsPUAllIdx_.clear();
  // outXYZ0PUAll_ = std::make_unique<std::vector<math::XYZPointF>>();
  outXYZ0PUAll_.clear();
  outt0PUAll_.clear();

  vecOutGenJets_.clear();
  vecOutXYZ0_.clear();
  vecOutt0_.clear();
  for(int iPU = 0; iPU < nMaxPUEvent_; iPU++){
    vecOutGenJets_.emplace_back(std::make_unique<reco::GenJetCollection>());
    vecOutXYZ0_.emplace_back(std::make_unique<math::XYZPointF>());
    vecOutt0_.emplace_back(std::make_unique<float>());
  }

  //
  //
  //
  int vertexOffset = 0;
  puEvtCounter_ = 0;
  input_->playPileUp(begin, end, recordEventID,std::bind(
      &GenPUJetExtractor::getGenJets, this, _1, mcc, bunchIdx, _2, vertexOffset, std::ref(iSetup), iEvent.streamID()
    )
  );


  for(int iPU = 0; iPU < nMaxPUEvent_; iPU++){
    // std::cout << "ak4GenJetsNoNuPUEvent"+std::to_string(iPU) << std::endl;
    iEvent.put(std::move(vecOutGenJets_[iPU]),"ak4GenJetsNoNuPUEvent"+std::to_string(iPU));
    iEvent.put(std::move(vecOutXYZ0_[iPU]),   "xyz0PUEvent"+std::to_string(iPU));
    iEvent.put(std::move(vecOutt0_[iPU]),     "t0PUEvent"+std::to_string(iPU));
  }


  edm::OrphanHandle<reco::GenJetCollection> orphanHandle = iEvent.put(std::move(outGenJetsPUAll_),"ak4GenJetsNoNuFromPU");
  auto genJetPUAllIdxV = std::make_unique<edm::ValueMap<int>>();
  edm::ValueMap<int>::Filler filler_genJetPUAllIdx(*genJetPUAllIdxV);
  filler_genJetPUAllIdx.insert(orphanHandle, outGenJetsPUAllIdx_.begin(), outGenJetsPUAllIdx_.end());
  filler_genJetPUAllIdx.fill();
  iEvent.put(std::move(genJetPUAllIdxV),"ak4GenJetsNoNuFromPUEventIdx");

  // iEvent.put(std::move(outXYZ0PUAll_),"xyz0PUEvent");
  // iEvent.emplace(ptokenXYZ0PUAll_,outXYZ0PUAll_,);

  edm::OrphanHandle<std::vector<math::XYZPointF>> orphanHandle2 = iEvent.emplace(ptokenXYZ0PUAll_,outXYZ0PUAll_);
  auto t0PUEventV = std::make_unique<edm::ValueMap<float>>();
  edm::ValueMap<float>::Filler filler_t0PUEvent(*t0PUEventV);
  filler_t0PUEvent.insert(orphanHandle2, outt0PUAll_.begin(), outt0PUAll_.end());
  filler_t0PUEvent.fill();
  iEvent.put(std::move(t0PUEventV),"t0PUEvent");
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
      vecOutGenJets_[puEvtCounter_]->emplace_back(prodColl->at(i));
      outGenJetsPUAll_->emplace_back(prodColl->at(i));
      outGenJetsPUAllIdx_.emplace_back(puEvtCounter_);
    }
  }else{
    std::cout << "Warning" << std::endl;
  }

  std::shared_ptr<Wrapper<math::XYZPointF> const> shPtr_xyz0 = getProductByTag<math::XYZPointF>(eventPrincipal, tag_xyz0_, &moduleCallingContext);
  if (shPtr_xyz0) {
    vecOutXYZ0_[puEvtCounter_].reset(new math::XYZPointF(*(shPtr_xyz0->product())));
    // outXYZ0PUAll_->emplace_back(math::XYZPointF(*(shPtr_xyz0->product())));
    outXYZ0PUAll_.emplace_back(math::XYZPointF(*(shPtr_xyz0->product())));
  }else{
    std::cout << "Warning: no shPtr_xyz0" << std::endl;
  }

  std::shared_ptr<Wrapper<float> const> shPtr_t0 = getProductByTag<float>(eventPrincipal, tag_t0_, &moduleCallingContext);
  if (shPtr_xyz0) {
    vecOutt0_[puEvtCounter_].reset(new float(*(shPtr_t0->product())));
    outt0PUAll_.emplace_back(*(shPtr_t0->product()));
  }else{
    std::cout << "Warning: no shPtr_t0" << std::endl;
  }
  // std::cout << (*(shPtr_t0->product())) << std::endl;

  puEvtCounter_++;
  return true;
}

#include "FWCore/Framework/interface/MakerMacros.h"

DEFINE_FWK_MODULE(GenPUJetExtractor);
