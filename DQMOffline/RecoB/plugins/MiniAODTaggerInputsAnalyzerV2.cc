#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"

#include "CommonTools/Utils/interface/StringObjectFunction.h"
#include "DataFormats/PatCandidates/interface/Jet.h"

#include "DataFormats/BTauReco/interface/JetFeatures.h"
#include "DataFormats/BTauReco/interface/SecondaryVertexFeatures.h"
#include "DataFormats/BTauReco/interface/ChargedCandidateFeatures.h"
#include "DataFormats/BTauReco/interface/NeutralCandidateFeatures.h"
#include "DataFormats/BTauReco/interface/LostTracksFeatures.h"

//For DeepJet
#include "DataFormats/BTauReco/interface/DeepFlavourTagInfo.h"
#include "DataFormats/BTauReco/interface/DeepFlavourFeatures.h"
//For ParticleNet
#include "DataFormats/BTauReco/interface/DeepBoostedJetTagInfo.h"
#include "DataFormats/BTauReco/interface/DeepBoostedJetFeatures.h"
//For UParT
#include "DataFormats/BTauReco/interface/UnifiedParticleTransformerAK4TagInfo.h"
#include "DataFormats/BTauReco/interface/UnifiedParticleTransformerAK4Features.h"


/** \class MiniAODTaggerInputsAnalyzerV2
 *
 *  TagInfo to run on MiniAOD
 *
 */
class MiniAODTaggerInputsAnalyzerV2 : public DQMEDAnalyzer {
public:
  typedef StringObjectFunction<btagbtvdeep::ChargedCandidateFeatures> fn_chargedcandfeat;

  explicit MiniAODTaggerInputsAnalyzerV2(const edm::ParameterSet& pSet);
  ~MiniAODTaggerInputsAnalyzerV2() override = default;

  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;
  void analyzeTagInfoDeepJet(const reco::DeepFlavourTagInfo*);
  void analyzeTagInfoParticleNet(const reco::DeepBoostedJetTagInfo*);
  void analyzeTagInfoUnifiedParticleTransformerAK4(const reco::UnifiedParticleTransformerAK4TagInfo*);

  const edm::EDGetTokenT<std::vector<pat::Jet>> jetToken_;
  const std::vector<std::string> jetTagInfos_;
  const std::string folderName_;
  const double ptMin_;
  const double absEtaMin_;
  const double absEtaMax_;

  // std::map<std::string,StringObjectFunction<pat::Jet>> map_func_features;
  std::map<std::string, std::unique_ptr<const StringObjectFunction<pat::Jet> > > map_func_features;
  std::map<std::string, std::unique_ptr<const StringObjectFunction<btagbtvdeep::ChargedCandidateFeatures>>> map_func_c_pf_features;
};

MiniAODTaggerInputsAnalyzerV2::MiniAODTaggerInputsAnalyzerV2(const edm::ParameterSet& pSet)
    : jetToken_(consumes<std::vector<pat::Jet>>(pSet.getParameter<edm::InputTag>("JetTag"))),
      jetTagInfos_(pSet.getParameter<std::vector<std::string>>("jetTagInfos")),
      folderName_(pSet.getParameter<std::string>("folderName")),
      ptMin_(pSet.getParameter<double>("ptMin")),
      absEtaMin_(pSet.getParameter<double>("absEtaMin")),
      absEtaMax_(pSet.getParameter<double>("absEtaMax")) {

      map_func_c_pf_features.insert(std::make_pair("c_pf_features.btagPf_trackEtaRel",
        std::unique_ptr<StringObjectFunction<btagbtvdeep::ChargedCandidateFeatures> >(new StringObjectFunction<btagbtvdeep::ChargedCandidateFeatures>("btagPf_trackEtaRel"))
        )
      );
        // map_func_features["UParT.c_pf_features"] = StringObjectFunction<pat::Jet>("tagInfo(\'pfUnifiedParticleTransformerAK4\').features().c_pf_features");
        // map_func_features["UParT.c_pf_features"] = StringObjectFunction<pat::Jet>(std::string("pt"));
        // map_func_c_pf_features["UParT.c_pf_features.btagPf_trackEtaRel"] = StringObjectFunction<btagbtvdeep::ChargedCandidateFeatures>("btagPf_trackEtaRel");
      }

void MiniAODTaggerInputsAnalyzerV2::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& es) {
  ibook.setCurrentFolder("Btag/"+folderName_);

  for (const auto& jetTagInfoLabel : jetTagInfos_) {

    if (jetTagInfoLabel == "pfDeepFlavour"){
    }
    else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiCentral"){
    }
    else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiForward"){
    }
    else if (jetTagInfoLabel == "pfUnifiedParticleTransformerAK4"){
    }
  }
}

void MiniAODTaggerInputsAnalyzerV2::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
  //
  //
  //
  edm::Handle<std::vector<pat::Jet>> jetCollection;
  iEvent.getByToken(jetToken_, jetCollection);

  // Loop over the pat::Jets
  for (std::vector<pat::Jet>::const_iterator jet = jetCollection->begin(); jet != jetCollection->end(); ++jet) {

    if (jet->pt() < ptMin_) continue;
    if (std::abs(jet->eta()) > absEtaMax_ || std::abs(jet->eta()) < absEtaMin_) continue;

    for (const auto& jetTagInfoLabel : jetTagInfos_) {
      if (!(jet->hasTagInfo(jetTagInfoLabel))) {
        // TODO: Add an CMS exception here
        continue;
      }
      if (jetTagInfoLabel == "pfUnifiedParticleTransformerAK4"){
        analyzeTagInfoUnifiedParticleTransformerAK4(static_cast<const reco::UnifiedParticleTransformerAK4TagInfo*>(jet->tagInfo(jetTagInfoLabel)));
      }
    }
  }
}

void MiniAODTaggerInputsAnalyzerV2::analyzeTagInfoDeepJet(const reco::DeepFlavourTagInfo* taginfo){
  // const auto& features = taginfo->features();
}

void MiniAODTaggerInputsAnalyzerV2::analyzeTagInfoParticleNet(const reco::DeepBoostedJetTagInfo* taginfo){
  // const auto& features = taginfo->features();
}

void MiniAODTaggerInputsAnalyzerV2::analyzeTagInfoUnifiedParticleTransformerAK4(const reco::UnifiedParticleTransformerAK4TagInfo* taginfo){
  const auto& features = taginfo->features();

  size_t n_cpf_ = (unsigned int)29;
  size_t n_lt_ = (unsigned int)5;
  size_t n_npf_ = (unsigned int)25;
  size_t n_sv_ = (unsigned int)5;

  auto& c_pf_features = features.c_pf_features;
  auto& n_pf_features = features.n_pf_features;
  auto& lt_features   = features.lt_features;
  auto& sv_features   = features.sv_features;

  auto max_c_pf_n = std::min(c_pf_features.size(), (std::size_t)n_cpf_);
  for (std::size_t c_pf_n = 0; c_pf_n < max_c_pf_n; c_pf_n++) {
    auto feature_value = (*map_func_c_pf_features["btagPf_trackEtaRel"])(c_pf_features.at(c_pf_n));
    std::cout << feature_value << std::endl;
  }
}

//define this as a plug-in
DEFINE_FWK_MODULE(MiniAODTaggerInputsAnalyzerV2);
