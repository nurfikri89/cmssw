#include <string>

#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"

#include "DataFormats/Common/interface/Association.h"
#include "DataFormats/PatCandidates/interface/PackedCandidate.h"
#include "DataFormats/Candidate/interface/CandidateFwd.h"

#include "DataFormats/Math/interface/deltaR.h"

typedef edm::Ptr<pat::PackedCandidate> PackedCandidatePtr;

class PackedPFCandIsoProducer : public edm::stream::EDProducer<> {
  public:
    explicit PackedPFCandIsoProducer(const edm::ParameterSet&);
    ~PackedPFCandIsoProducer() override;

    void produce(edm::Event&, const edm::EventSetup&) override;

private:
  const edm::EDGetTokenT<std::vector<PackedCandidatePtr>> packedPFCandsPtrSelectedToken_;
  const edm::EDGetTokenT<pat::PackedCandidateCollection> packedPFCandsToken_;
  const double maxDeltaR_;
  double maxDeltaR2_;
};

PackedPFCandIsoProducer::PackedPFCandIsoProducer(const edm::ParameterSet& iConfig):
    packedPFCandsPtrSelectedToken_(consumes<std::vector<PackedCandidatePtr>>(iConfig.getParameter<edm::InputTag>("packedPFCandidatesSelected"))),
    packedPFCandsToken_(consumes<pat::PackedCandidateCollection>(iConfig.getParameter<edm::InputTag>("packedPFCandidates"))),
    maxDeltaR_(iConfig.getParameter<double>("maxDeltaR")){

  maxDeltaR2_ = maxDeltaR_ * maxDeltaR_;
  produces<std::vector<PackedCandidatePtr>>();
  // produces<edm::ValueMap<std::vector<int>>>();
}

PackedPFCandIsoProducer::~PackedPFCandIsoProducer() {}

void PackedPFCandIsoProducer::produce(edm::Event& iEvent, const edm::EventSetup& iSetup) {

  //
  // selected packedPFCandidate collection
  //
  edm::Handle<std::vector<PackedCandidatePtr>> packedPFCandsPtrSelectedHandle;
  iEvent.getByToken(packedPFCandsPtrSelectedToken_, packedPFCandsPtrSelectedHandle);
  const std::vector<PackedCandidatePtr>* packedPFCandsPtrSelected = packedPFCandsPtrSelectedHandle.product();

  //
  // packedPFCandidate collection
  //
  edm::Handle<pat::PackedCandidateCollection> packedPFCandsHandle;
  iEvent.getByToken(packedPFCandsToken_, packedPFCandsHandle);
  const pat::PackedCandidateCollection* packedPFCands = packedPFCandsHandle.product();

  //
  //
  //
  auto outPackedPFCandidatesPtr = std::make_unique<std::vector<PackedCandidatePtr>>();


  //
  //
  //
  // std::vector<std::vector<int>> packedPFCandsSelectedVec(
  //   packedPFCandsPtrSelected->size(),std::vector<int>()
  // );

  //
  // Loop over
  //
  std::vector<PackedCandidatePtr>::const_iterator pfCandSelPtr_Itr;
  pat::PackedCandidateCollection::const_iterator pfCand_Itr;
  unsigned int i_pfSel = 0;
  unsigned int pfCandIdx = 0;
  for (i_pfSel = 0, pfCandSelPtr_Itr = packedPFCandsPtrSelected->begin(); pfCandSelPtr_Itr != packedPFCandsPtrSelected->end(); i_pfSel++, pfCandSelPtr_Itr++) {
    auto pfCandSel = (*pfCandSelPtr_Itr).get();
    auto pfCandSelKey = (*pfCandSelPtr_Itr).key();
    //
    // Loop over the whole packed candidate collection
    //
    for (pfCandIdx = 0, pfCand_Itr = packedPFCands->begin(); pfCand_Itr != packedPFCands->end(); pfCandIdx++, pfCand_Itr++) {
      if(pfCandSelKey == pfCandIdx) {
        // std::cout << "Same particle: pfCandSelKey = " << pfCandSelKey << " pfCandIdx = " << pfCandIdx << std::endl;
        // std::cout << "pfCandSel pt = " << pfCandSel->pt() << " eta = " << pfCandSel->eta() << " phi = " << pfCandSel->phi() << " pdgId = " << pfCandSel->pdgId() << std::endl;
        // std::cout << "pfCand_Itr pt = " << pfCand_Itr->pt() << " eta = " << pfCand_Itr->eta() << " phi = " << pfCand_Itr->phi() << " pdgId = " << pfCand_Itr->pdgId() << std::endl;
        continue;
      }
      float dR2 = deltaR2(pfCandSel->p4(), pfCand_Itr->p4());
      if (dR2 <= maxDeltaR2_){
        // packedPFCandsSelectedVec[i_pfSel].push_back(pfCandIdx);
        // std::cout << "LAAA: pfCandSelKey = " << pfCandSelKey << " pfCandIdx = " << pfCandIdx << std::endl;
        outPackedPFCandidatesPtr->push_back(edm::Ptr<pat::PackedCandidate>(packedPFCandsHandle,pfCandIdx));
      }
    }
    // for (size_t ipc = 0; ipc < packedPFCandsHandle->size(); ++ipc) {
    //   auto packedPFPtr = packedPFCandsHandle->ptrAt(ipc);
    //   if(pfCandSelPtrKey == packedPFPtr->key()) continue;
    // }
  }

  iEvent.put(std::move(outPackedPFCandidatesPtr));

  // auto selPackedPFMapAssocMap = std::make_unique<edm::ValueMap<std::vector<int>>>();
  // edm::ValueMap<std::vector<int>>::Filler fillerSelPackedPFMapAssocMap(*selPackedPFMapAssocMap);
  // fillerSelPackedPFMapAssocMap.insert(packedPFCandsPtrSelectedHandle,packedPFCandsSelectedVec.begin(), packedPFCandsSelectedVec.end());
  // fillerSelPackedPFMapAssocMap.fill();
  // iEvent.put(std::move(selPackedPFMapAssocMap));

}

#include "FWCore/Framework/interface/MakerMacros.h"
DEFINE_FWK_MODULE(PackedPFCandIsoProducer);
