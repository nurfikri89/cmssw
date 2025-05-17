#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"

#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"

#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"

#include "DataFormats/PatCandidates/interface/Electron.h"
#include "DataFormats/PatCandidates/interface/Photon.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"

#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "DataFormats/NanoAOD/interface/FlatTable.h"

template<typename T>
class SimplePatObjectPFCandTableProducer : public edm::stream::EDProducer<> {
public:
  explicit SimplePatObjectPFCandTableProducer(const edm::ParameterSet &);
  ~SimplePatObjectPFCandTableProducer() override;

  static void fillDescriptions(edm::ConfigurationDescriptions &descriptions);

private:
  void produce(edm::Event &, const edm::EventSetup &) override;

  const std::string name_;
  const std::string candIdxName_;
  const std::string candIdxDoc_;

  edm::EDGetTokenT<edm::View<T>> object_token_;
  edm::EDGetTokenT<reco::CandidateView> cand_token_;

  const StringCutObjectSelector<T> objectCut_;

  edm::Handle<reco::CandidateView> cands_;
};

//
// constructors and destructor
//
template<typename T>
SimplePatObjectPFCandTableProducer<T>::SimplePatObjectPFCandTableProducer(const edm::ParameterSet &iConfig):
  name_(iConfig.getParameter<std::string>("name")),
  candIdxName_(iConfig.getParameter<std::string>("candIdxName")),
  candIdxDoc_(iConfig.getParameter<std::string>("candIdxDoc")),
  object_token_(consumes<edm::View<T>>(iConfig.getParameter<edm::InputTag>("objects"))),
  cand_token_(consumes<reco::CandidateView>(iConfig.getParameter<edm::InputTag>("candidates"))),
  objectCut_(iConfig.getParameter<std::string>("objectCut"))
{
  produces<nanoaod::FlatTable>(name_);
  produces<std::vector<reco::CandidatePtr>>();
}

template<typename T>
SimplePatObjectPFCandTableProducer<T>::~SimplePatObjectPFCandTableProducer() {}

template<typename T>
void SimplePatObjectPFCandTableProducer<T>::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  // elements in all these collections must have the same order!
  auto outCands = std::make_unique<std::vector<reco::CandidatePtr>>();

  auto objects = iEvent.getHandle(object_token_);

  iEvent.getByToken(cand_token_, cands_);
  auto candPtrs = cands_->ptrs();

  //
  // First, select objects
  //
  std::vector<T> objectsPassCut;
  for (unsigned objectIdx = 0; objectIdx < objects->size(); ++objectIdx) {
    const auto &object = objects->at(objectIdx);
    if (!objectCut_(object)) continue;
    objectsPassCut.push_back(objects->at(objectIdx));
  }

  //
  // Then loop over selected objects
  //
  std::vector<int> parentObjectIdx;
  std::vector<int> candIdx;
  for (unsigned objectIdx = 0; objectIdx < objectsPassCut.size(); ++objectIdx) {
    const auto &object = objectsPassCut.at(objectIdx);
    for(size_t ipf=0; ipf < object.numberOfSourceCandidatePtrs(); ipf++){
      auto cand = object.sourceCandidatePtr(ipf);
      auto candInNewList = std::find( candPtrs.begin(), candPtrs.end(), cand );
      if ( candInNewList == candPtrs.end() ) {
        continue;
      }
      outCands->push_back(cand);
      parentObjectIdx.push_back(objectIdx);
      candIdx.push_back(candInNewList - candPtrs.begin());
    }
  }// end object loop

  auto candTable = std::make_unique<nanoaod::FlatTable>(outCands->size(), name_, false);
  // We fill from here only stuff that cannot be created with the SimpleFlatTableProducer
  candTable->addColumn<int>(candIdxName_, candIdx,      candIdxDoc_);
  std::string parentObjectIdxName("objectIdx");
  std::string parentObjectIdxDoc("Index of the parent object");
  if constexpr (std::is_same<T,pat::Electron>::value){
    parentObjectIdxName = "electronIdx";
    parentObjectIdxDoc = "Index of the parent electron";
  }
  if constexpr (std::is_same<T,pat::Photon>::value){
    parentObjectIdxName = "photonIdx";
    parentObjectIdxDoc = "Index of the parent photon";
  }
  if constexpr (std::is_same<T,pat::Muon>::value){
    parentObjectIdxName = "muonIdx";
    parentObjectIdxDoc = "Index of the parent muon";
  }

  candTable->addColumn<int>(parentObjectIdxName, parentObjectIdx, parentObjectIdxDoc);

  iEvent.put(std::move(candTable), name_);
  iEvent.put(std::move(outCands));
}

template<typename T>
void SimplePatObjectPFCandTableProducer<T>::fillDescriptions(edm::ConfigurationDescriptions &descriptions) {
  edm::ParameterSetDescription desc;
  desc.add<std::string>("name", "ObjectPFCand");
  desc.add<std::string>("candIdxName", "PFCandIdx");
  desc.add<std::string>("candIdxDoc", "Index in PFCand table");
  desc.add<edm::InputTag>("objects", edm::InputTag("slimmedObjects"));
  desc.add<edm::InputTag>("candidates", edm::InputTag("packedPFCandidates"));
  desc.add<std::string>("objectCut", "");
  descriptions.addWithDefaultLabel(desc);
}

typedef SimplePatObjectPFCandTableProducer<pat::Electron> SimplePatElectronPFCandTableProducer;
typedef SimplePatObjectPFCandTableProducer<pat::Photon>   SimplePatPhotonPFCandTableProducer;
typedef SimplePatObjectPFCandTableProducer<pat::Muon>     SimplePatMuonPFCandTableProducer;

DEFINE_FWK_MODULE(SimplePatElectronPFCandTableProducer);
DEFINE_FWK_MODULE(SimplePatPhotonPFCandTableProducer);
DEFINE_FWK_MODULE(SimplePatMuonPFCandTableProducer);
