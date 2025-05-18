/////////////////////////////// MuonNoTriggerSelector //////////////////////////
//
//

#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/Event.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/stream/EDProducer.h"
#include "FWCore/ParameterSet/interface/ParameterSet.h"
#include "FWCore/Utilities/interface/StreamID.h"
#include "FWCore/Common/interface/TriggerNames.h"

#include "DataFormats/Common/interface/TriggerResults.h"
#include "DataFormats/PatCandidates/interface/Muon.h"
#include "DataFormats/PatCandidates/interface/TriggerObjectStandAlone.h"
#include "DataFormats/PatCandidates/interface/PackedTriggerPrescales.h"
#include "DataFormats/PatCandidates/interface/TriggerPath.h"
#include "DataFormats/PatCandidates/interface/TriggerEvent.h"
#include "DataFormats/PatCandidates/interface/TriggerAlgorithm.h"

#include "MagneticField/Engine/interface/MagneticField.h"
#include "MagneticField/Records/interface/IdealMagneticFieldRecord.h"

#include "CommonTools/Utils/interface/StringCutObjectSelector.h"
#include "helper.h"

using namespace std;

constexpr bool debug = false;

class MuonNoTriggerSelector : public edm::stream::EDProducer<> {
public:
  explicit MuonNoTriggerSelector(const edm::ParameterSet &iConfig);
  ~MuonNoTriggerSelector() override {};

private:
  void produce(edm::Event &, const edm::EventSetup &) override;
  const edm::ESGetToken<MagneticField, IdealMagneticFieldRecord> bFieldToken_;
  edm::EDGetTokenT<std::vector<pat::Muon>> muonSrc_;
  const StringCutObjectSelector<pat::Muon> muon_selection_;
};

MuonNoTriggerSelector::MuonNoTriggerSelector(const edm::ParameterSet &iConfig)
    : bFieldToken_(esConsumes<MagneticField, IdealMagneticFieldRecord>()),
      muonSrc_(consumes<std::vector<pat::Muon>>(iConfig.getParameter<edm::InputTag>("muonCollection"))),
      muon_selection_{iConfig.getParameter<std::string>("muonSelection")}
{
  // outputs
  produces<pat::MuonCollection>("SelectedMuons");
  produces<TransientTrackCollection>("SelectedTransientMuons");
}

void MuonNoTriggerSelector::produce(edm::Event &iEvent, const edm::EventSetup &iSetup) {
  const auto &bField = iSetup.getData(bFieldToken_);

  std::unique_ptr<pat::MuonCollection> muons_out(new pat::MuonCollection);
  std::unique_ptr<TransientTrackCollection> trans_muons_out(new TransientTrackCollection);

  edm::Handle<std::vector<pat::Muon>> muons;
  iEvent.getByToken(muonSrc_, muons);

  for (const pat::Muon &muon : *muons) {
    unsigned int iMuo(&muon - &(muons->at(0)));
    if (!muon_selection_(muon))
      continue;  // selection cuts

    const reco::TransientTrack muonTT((*(muon.bestTrack())), &bField);
    if (!muonTT.isValid())
      continue;

    muons_out->emplace_back(muon);
    trans_muons_out->emplace_back(muonTT);
  }

  iEvent.put(std::move(muons_out), "SelectedMuons");
  iEvent.put(std::move(trans_muons_out), "SelectedTransientMuons");
}

DEFINE_FWK_MODULE(MuonNoTriggerSelector);
