#include "DQMServices/Core/interface/DQMEDAnalyzer.h"
#include "DataFormats/PatCandidates/interface/Jet.h"
#include "FWCore/Framework/interface/Frameworkfwd.h"
#include "FWCore/Framework/interface/MakerMacros.h"
#include "FWCore/Framework/interface/Event.h"


//For DeepJet
#include "DataFormats/BTauReco/interface/DeepFlavourTagInfo.h"
#include "DataFormats/BTauReco/interface/DeepFlavourFeatures.h"
//For ParticleNet
#include "DataFormats/BTauReco/interface/DeepBoostedJetTagInfo.h"
#include "DataFormats/BTauReco/interface/DeepBoostedJetFeatures.h"
//For UParT
#include "DataFormats/BTauReco/interface/UnifiedParticleTransformerAK4TagInfo.h"
#include "DataFormats/BTauReco/interface/UnifiedParticleTransformerAK4Features.h"

/** \class MiniAODTagInfoAnalyzer
 *
 *  TagInfo to run on MiniAOD
 *
 */
class MiniAODTagInfoAnalyzer : public DQMEDAnalyzer {
public:

  explicit MiniAODTagInfoAnalyzer(const edm::ParameterSet& pSet);
  ~MiniAODTagInfoAnalyzer() override = default;

  void analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) override;

private:
  void bookHistograms(DQMStore::IBooker&, edm::Run const&, edm::EventSetup const&) override;

  void analyzeTagInfoDeepJet(const reco::DeepFlavourTagInfo*);
  void analyzeTagInfoParticleNet(const reco::DeepBoostedJetTagInfo*);
  void analyzeTagInfoUnifiedParticleTransformerAK4(const reco::UnifiedParticleTransformerAK4TagInfo*);

  const edm::EDGetTokenT<std::vector<pat::Jet>> jetToken_;
  const std::vector<std::string> jetTagInfos_;
  const double ptMin_;
  const double absEtaMin_;
  const double absEtaMax_;

  std::unordered_map<std::string,MonitorElement*> map_ME_DeepJet_;
  std::unordered_map<std::string,MonitorElement*> map_ME_ParticleNet_;
  std::unordered_map<std::string,MonitorElement*> map_ME_UParT_;
};

MiniAODTagInfoAnalyzer::MiniAODTagInfoAnalyzer(const edm::ParameterSet& pSet)
    : jetToken_(consumes<std::vector<pat::Jet>>(pSet.getParameter<edm::InputTag>("jetSource"))),
      jetTagInfos_(pSet.getParameter<std::vector<std::string>>("jetTagInfos")),
      ptMin_(pSet.getParameter<double>("ptMin")),
      absEtaMin_(pSet.getParameter<double>("absEtaMin")),
      absEtaMax_(pSet.getParameter<double>("absEtaMax")) {}

void MiniAODTagInfoAnalyzer::bookHistograms(DQMStore::IBooker& ibook, edm::Run const& run, edm::EventSetup const& es) {
  for (const auto& jetTagInfoLabel : jetTagInfos_) {

    if (jetTagInfoLabel == "pfDeepFlavour"){
      ibook.setCurrentFolder("Btag/TagInfo_pfDeepFlavour");

      map_ME_DeepJet_["c_pf_btagPf_trackEtaRel"]     = ibook.book1D("c_pf_btagPf_trackEtaRel",     "", 50,   0., 10.);
      map_ME_DeepJet_["c_pf_btagPf_trackPtRel"]      = ibook.book1D("c_pf_btagPf_trackPtRel",      "", 100,  0., 5.);
      map_ME_DeepJet_["c_pf_btagPf_trackPPar"]       = ibook.book1D("c_pf_btagPf_trackPPar",       "", 100,  0., 500.);
      map_ME_DeepJet_["c_pf_btagPf_trackDeltaR"]     = ibook.book1D("c_pf_btagPf_trackDeltaR",     "", 50,   0., 1);
      map_ME_DeepJet_["c_pf_btagPf_trackPParRatio"]  = ibook.book1D("c_pf_btagPf_trackPParRatio",  "", 120, 0.4, 1.);
      map_ME_DeepJet_["c_pf_btagPf_trackSip2dVal"]   = ibook.book1D("c_pf_btagPf_trackSip2dVal",   "", 60, -0.15, 0.15);
      map_ME_DeepJet_["c_pf_btagPf_trackSip2dSig"]   = ibook.book1D("c_pf_btagPf_trackSip2dSig",   "", 120, -5., 25.);
      map_ME_DeepJet_["c_pf_btagPf_trackSip3dVal"]   = ibook.book1D("c_pf_btagPf_trackSip3dVal",   "", 60, -0.15, 0.15);
      map_ME_DeepJet_["c_pf_btagPf_trackSip3dSig"]   = ibook.book1D("c_pf_btagPf_trackSip3dSig",   "", 120, -5., 25.);
      map_ME_DeepJet_["c_pf_btagPf_trackJetDistVal"] = ibook.book1D("c_pf_btagPf_trackJetDistVal", "", 110, -1.0, 0.1);
      map_ME_DeepJet_["c_pf_ptrel"]                  = ibook.book1D("c_pf_ptrel",                  "", 100, -1.2, 0.2);
      map_ME_DeepJet_["c_pf_drminsv"]                = ibook.book1D("c_pf_drminsv",                "", 120, -0.5, 0.1);
      map_ME_DeepJet_["c_pf_vtx_ass"]                = ibook.book1D("c_pf_vtx_ass",                "", 10, 0, 10);
      map_ME_DeepJet_["c_pf_puppiw"]                 = ibook.book1D("c_pf_puppiw",                 "", 50, 0., 1.);
      map_ME_DeepJet_["c_pf_chi2"]                   = ibook.book1D("c_pf_chi2",                   "", 15, 0., 15.);
      map_ME_DeepJet_["c_pf_quality"]                = ibook.book1D("c_pf_quality",                "", 10, 0., 10);
      map_ME_DeepJet_["n_pf_ptrel"]                  = ibook.book1D("n_pf_ptrel",                  "", 50, -1., 0.);
      map_ME_DeepJet_["n_pf_deltaR"]                 = ibook.book1D("n_pf_deltaR",                 "", 50, -1., 0.);
      map_ME_DeepJet_["n_pf_isGamma"]                = ibook.book1D("n_pf_isGamma",                "", 2, -0.5, 1.5);
      map_ME_DeepJet_["n_pf_hadFrac"]                = ibook.book1D("n_pf_hadFrac",                "", 20, 0., 1.);
      map_ME_DeepJet_["n_pf_drminsv"]                = ibook.book1D("n_pf_drminsv",                "", 10, 0., 0.5);
      map_ME_DeepJet_["n_pf_puppiw"]                 = ibook.book1D("n_pf_puppiw",                 "", 50, 0., 1.);
      map_ME_DeepJet_["sv_pt"]                       = ibook.book1D("sv_pt",                       "", 50, 0.,  200.);
      map_ME_DeepJet_["sv_deltaR"]                   = ibook.book1D("sv_deltaR",                   "", 100, -0.8, 0.2);
      map_ME_DeepJet_["sv_mass"]                     = ibook.book1D("sv_mass",                     "", 40, 0., 10.);
      map_ME_DeepJet_["sv_ntracks"]                  = ibook.book1D("sv_ntracks",                  "", 15, 0,  15);
      map_ME_DeepJet_["sv_chi2"]                     = ibook.book1D("sv_chi2",                     "", 40., 0., 20.);
      map_ME_DeepJet_["sv_normchi2"]                 = ibook.book1D("sv_normchi2",                 "", 40., 0., 20.);
      map_ME_DeepJet_["sv_dxy"]                      = ibook.book1D("sv_dxy",                      "", 100,  0., 0.5);
      map_ME_DeepJet_["sv_dxysig"]                   = ibook.book1D("sv_dxysig",                   "", 140, 0., 70.);
      map_ME_DeepJet_["sv_d3d"]                      = ibook.book1D("sv_d3d",                      "", 50,  0., 1.0);
      map_ME_DeepJet_["sv_d3dsig"]                   = ibook.book1D("sv_d3dsig",                   "", 140, 0., 70.);
      map_ME_DeepJet_["sv_costhetasvpv"]             = ibook.book1D("sv_costhetasvpv",             "", 100, -1., 1.);
      map_ME_DeepJet_["sv_enratio"]                  = ibook.book1D("sv_enratio",                  "", 100,  0., 1.);
      map_ME_DeepJet_["max_cpf_n"]                   = ibook.book1D("max_cpf_n",                   "", 31,  -0.5, 30.5);
      map_ME_DeepJet_["max_npf_n"]                   = ibook.book1D("max_npf_n",                   "", 31,  -0.5, 30.5);
      map_ME_DeepJet_["max_sv_n"]                    = ibook.book1D("max_sv_n",                    "",  6,  -0.5, 5.5);
      map_ME_DeepJet_["pv_n"]                        = ibook.book1D("pv_n",                        "", 100, -0.5, 100.5);
    }
    else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiCentral"){
      ibook.setCurrentFolder("Btag/TagInfo_pfParticleNetFromMiniAODAK4PuppiCentral");
    }
    else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiForward"){
      ibook.setCurrentFolder("Btag/TagInfo_pfParticleNetFromMiniAODAK4PuppiForward");
    }
    else if (jetTagInfoLabel == "pfUnifiedParticleTransformerAK4"){
      ibook.setCurrentFolder("Btag/TagInfo_pfUnifiedParticleTransformerAK4");

      map_ME_UParT_["c_pf_btagPf_trackEtaRel"]      = ibook.book1D("c_pf_btagPf_trackEtaRel",     "", 50,   0., 10.);
      map_ME_UParT_["c_pf_btagPf_trackPtRel"]       = ibook.book1D("c_pf_btagPf_trackPtRel",      "", 100,  0., 5.);
      map_ME_UParT_["c_pf_btagPf_trackPPar"]        = ibook.book1D("c_pf_btagPf_trackPPar",       "", 100,  0., 500.);
      map_ME_UParT_["c_pf_btagPf_trackDeltaR"]      = ibook.book1D("c_pf_btagPf_trackDeltaR",     "", 50,   0., 1);
      map_ME_UParT_["c_pf_btagPf_trackPParRatio"]   = ibook.book1D("c_pf_btagPf_trackPParRatio",  "", 120, 0.4, 1.);
      map_ME_UParT_["c_pf_btagPf_trackSip2dVal"]    = ibook.book1D("c_pf_btagPf_trackSip2dVal",   "", 60, -0.15, 0.15);
      map_ME_UParT_["c_pf_btagPf_trackSip2dSig"]    = ibook.book1D("c_pf_btagPf_trackSip2dSig",   "", 120, -5., 25.);
      map_ME_UParT_["c_pf_btagPf_trackSip3dVal"]    = ibook.book1D("c_pf_btagPf_trackSip3dVal",   "", 60, -0.15, 0.15);
      map_ME_UParT_["c_pf_btagPf_trackSip3dSig"]    = ibook.book1D("c_pf_btagPf_trackSip3dSig",   "", 120, -5., 25.);
      map_ME_UParT_["c_pf_btagPf_trackJetDistVal"]  = ibook.book1D("c_pf_btagPf_trackJetDistVal", "", 110, -1.0, 0.1);
      map_ME_UParT_["c_pf_ptrel"]                   = ibook.book1D("c_pf_ptrel",                  "", 100, -1.2, 0.2);
      map_ME_UParT_["c_pf_drminsv"]                 = ibook.book1D("c_pf_drminsv",                "", 120, -0.5, 0.);
      map_ME_UParT_["c_pf_vtx_ass"]                 = ibook.book1D("c_pf_vtx_ass",                "", 10, 0, 10);
      map_ME_UParT_["c_pf_puppiw"]                  = ibook.book1D("c_pf_puppiw",                 "", 50, 0., 1.);
      map_ME_UParT_["c_pf_chi2"]                    = ibook.book1D("c_pf_chi2",                   "", 15, 0., 15);
      map_ME_UParT_["c_pf_quality"]                 = ibook.book1D("c_pf_quality",                "", 10, 0., 10);
      map_ME_UParT_["c_pf_charge"]                  = ibook.book1D("c_pf_charge",                 "", 3, -1.5, 1.5);
      map_ME_UParT_["c_pf_dz"]                      = ibook.book1D("c_pf_dz",                     "", 50, 0., 50.);
      map_ME_UParT_["c_pf_btagPf_trackDecayLen"]    = ibook.book1D("c_pf_btagPf_trackDecayLen",   "", 50, 0., 50.);
      map_ME_UParT_["c_pf_HadFrac"]                 = ibook.book1D("c_pf_HadFrac",                "", 20, 0., 1.);
      map_ME_UParT_["c_pf_CaloFrac"]                = ibook.book1D("c_pf_CaloFrac",               "", 80, 0., 4.);
      map_ME_UParT_["c_pf_pdgID"]                   = ibook.book1D("c_pf_pdgID",                  "", 11, -0.5, 10.5);
      map_ME_UParT_["c_pf_lostInnerHits"]           = ibook.book1D("c_pf_lostInnerHits",          "", 11, -0.5, 10.5);
      map_ME_UParT_["c_pf_numberOfPixelHits"]       = ibook.book1D("c_pf_numberOfPixelHits",      "", 21, -0.5, 20.5);
      map_ME_UParT_["c_pf_numberOfStripHits"]       = ibook.book1D("c_pf_numberOfStripHits",      "", 21, -0.5, 20.5);
      map_ME_UParT_["lt_btagPf_trackEtaRel"]        = ibook.book1D("lt_btagPf_trackEtaRel",       "", 50,0., 10.);
      map_ME_UParT_["lt_btagPf_trackPtRel"]         = ibook.book1D("lt_btagPf_trackPtRel",        "", 100,  0., 5.);
      map_ME_UParT_["lt_btagPf_trackPPar"]          = ibook.book1D("lt_btagPf_trackPPar",         "", 100,  0., 500.);
      map_ME_UParT_["lt_btagPf_trackDeltaR"]        = ibook.book1D("lt_btagPf_trackDeltaR",       "", 50,   0., 1);
      map_ME_UParT_["lt_btagPf_trackPParRatio"]     = ibook.book1D("lt_btagPf_trackPParRatio",    "", 120, 0.4, 1.);
      map_ME_UParT_["lt_btagPf_trackSip2dVal"]      = ibook.book1D("lt_btagPf_trackSip2dVal",     "", 60, -0.15, 0.15);
      map_ME_UParT_["lt_btagPf_trackSip2dSig"]      = ibook.book1D("lt_btagPf_trackSip2dSig",     "", 120, -5., 25.);
      map_ME_UParT_["lt_btagPf_trackSip3dVal"]      = ibook.book1D("lt_btagPf_trackSip3dVal",     "", 60, -0.15, 0.15);
      map_ME_UParT_["lt_btagPf_trackSip3dSig"]      = ibook.book1D("lt_btagPf_trackSip3dSig",     "", 120, -5., 25.);
      map_ME_UParT_["lt_btagPf_trackJetDistVal"]    = ibook.book1D("lt_btagPf_trackJetDistVal",   "", 110, -1.0, 0.1);
      map_ME_UParT_["lt_drminsv"]                   = ibook.book1D("lt_drminsv",                  "", 120, -0.5, 0.);
      map_ME_UParT_["lt_charge"]                    = ibook.book1D("lt_charge",                   "", 3, -1.5, 1.5);
      map_ME_UParT_["lt_puppiw"]                    = ibook.book1D("lt_puppiw",                   "", 50, 0., 1.);
      map_ME_UParT_["lt_chi2"]                      = ibook.book1D("lt_chi2",                     "", 15, 0., 15.);
      map_ME_UParT_["lt_quality"]                   = ibook.book1D("lt_quality",                  "", 10, 0., 10);
      map_ME_UParT_["lt_lostInnerHits"]             = ibook.book1D("lt_lostInnerHits",            "", 11, -0.5, 10.5);
      map_ME_UParT_["lt_numberOfPixelHits"]         = ibook.book1D("lt_numberOfPixelHits",        "", 21, -0.5, 20.5);
      map_ME_UParT_["lt_numberOfStripHits"]         = ibook.book1D("lt_numberOfStripHits",        "", 21, -0.5, 20.5);
      map_ME_UParT_["n_pf_ptrel"]                   = ibook.book1D("n_pf_ptrel",                  "", 100, -1.2, 0.2);
      map_ME_UParT_["n_pf_etarel"]                  = ibook.book1D("n_pf_etarel",                 "", 40, -10., 10.);
      map_ME_UParT_["n_pf_phirel"]                  = ibook.book1D("n_pf_phirel",                 "", 40, -10., 10.);
      map_ME_UParT_["n_pf_deltaR"]                  = ibook.book1D("n_pf_deltaR",                 "", 50, -1., 0.);
      map_ME_UParT_["n_pf_isGamma"]                 = ibook.book1D("n_pf_isGamma",                "", 2, -0.5, 1.5);
      map_ME_UParT_["n_pf_hadFrac"]                 = ibook.book1D("n_pf_hadFrac",                "", 20, 0., 1.);
      map_ME_UParT_["n_pf_drminsv"]                 = ibook.book1D("n_pf_drminsv",                "", 10, 0., 0.5);
      map_ME_UParT_["n_pf_puppiw"]                  = ibook.book1D("n_pf_puppiw",                 "", 50, 0., 1.);
      map_ME_UParT_["sv_pt"]                        = ibook.book1D("sv_pt",                       "", 50, 0.,  200.);
      map_ME_UParT_["sv_deltaR"]                    = ibook.book1D("sv_deltaR",                   "", 100, -0.8, 0.2);
      map_ME_UParT_["sv_mass"]                      = ibook.book1D("sv_mass",                     "", 40, 0., 10.);
      map_ME_UParT_["sv_etarel"]                    = ibook.book1D("sv_etarel",                   "", 40, -10., 10.);
      map_ME_UParT_["sv_phirel"]                    = ibook.book1D("sv_phirel",                   "", 40, -10., 10.);
      map_ME_UParT_["sv_ntracks"]                   = ibook.book1D("sv_ntracks",                  "", 15, 0, 15);
      map_ME_UParT_["sv_chi2"]                      = ibook.book1D("sv_chi2",                     "", 40., 0., 20.);
      map_ME_UParT_["sv_normchi2"]                  = ibook.book1D("sv_normchi2",                 "", 40., 0., 20.);
      map_ME_UParT_["sv_dxy"]                       = ibook.book1D("sv_dxy",                      "", 100,  0., 0.5);
      map_ME_UParT_["sv_dxysig"]                    = ibook.book1D("sv_dxysig",                   "", 140, 0., 70.);
      map_ME_UParT_["sv_d3d"]                       = ibook.book1D("sv_d3d",                      "", 50,  0., 1.0);
      map_ME_UParT_["sv_d3dsig"]                    = ibook.book1D("sv_d3dsig",                   "", 140, 0., 70.);
      map_ME_UParT_["sv_costhetasvpv"]              = ibook.book1D("sv_costhetasvpv",             "", 100, -1., 1.);
      map_ME_UParT_["sv_enratio"]                   = ibook.book1D("sv_enratio",                  "", 100,  0., 1.);
      map_ME_UParT_["max_cpf_n"]                    = ibook.book1D("max_cpf_n",                   "", 31, -0.5, 30);
      map_ME_UParT_["max_lt_n"]                     = ibook.book1D("max_lt_n",                    "", 11, -0.5, 10);
      map_ME_UParT_["max_npf_n"]                    = ibook.book1D("max_npf_n",                   "", 31, -0.5, 30);
      map_ME_UParT_["max_sv_n"]                     = ibook.book1D("max_sv_n",                    "", 11, -0.5, 10);
    }
  }
}

void MiniAODTagInfoAnalyzer::analyze(const edm::Event& iEvent, const edm::EventSetup& iSetup) {
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
        throw cms::Exception("MiniAODTagInfoAnalyzer")
               << "Jet does not have TagInfo: "+jetTagInfoLabel+". Please check!";
        continue;
      }
      if (jetTagInfoLabel == "pfDeepFlavour"){
        const reco::DeepFlavourTagInfo* taginfo  = static_cast<const reco::DeepFlavourTagInfo*>(jet->tagInfo(jetTagInfoLabel));
        analyzeTagInfoDeepJet(taginfo);
      }
      else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiCentral"){
        const reco::DeepBoostedJetTagInfo* taginfo  = static_cast<const reco::DeepBoostedJetTagInfo*>(jet->tagInfo(jetTagInfoLabel));
        analyzeTagInfoParticleNet(taginfo);
      }
      else if (jetTagInfoLabel == "pfParticleNetFromMiniAODAK4PuppiForward"){
        const reco::DeepBoostedJetTagInfo* taginfo  = static_cast<const reco::DeepBoostedJetTagInfo*>(jet->tagInfo(jetTagInfoLabel));
        analyzeTagInfoParticleNet(taginfo);
      }
      else if (jetTagInfoLabel == "pfUnifiedParticleTransformerAK4"){
        const reco::UnifiedParticleTransformerAK4TagInfo* taginfo  = static_cast<const reco::UnifiedParticleTransformerAK4TagInfo*>(jet->tagInfo(jetTagInfoLabel));
        analyzeTagInfoUnifiedParticleTransformerAK4(taginfo);
      }
    }
  }
}

void MiniAODTagInfoAnalyzer::analyzeTagInfoDeepJet(const reco::DeepFlavourTagInfo* taginfo){
  const auto& features = taginfo->features();

  size_t n_cpf_ = (unsigned int)25;
  size_t n_npf_ = (unsigned int)25;
  size_t n_sv_ = (unsigned int)4;

  // c_pf candidates
  auto max_c_pf_n = std::min(features.c_pf_features.size(), n_cpf_);
  for (std::size_t c_pf_n = 0; c_pf_n < max_c_pf_n; c_pf_n++) {
    const auto& c_pf_features = features.c_pf_features.at(c_pf_n);
    map_ME_DeepJet_["c_pf_btagPf_trackEtaRel"]->Fill(c_pf_features.btagPf_trackEtaRel);
    map_ME_DeepJet_["c_pf_btagPf_trackPtRel"]->Fill(c_pf_features.btagPf_trackPtRel);
    map_ME_DeepJet_["c_pf_btagPf_trackPPar"]->Fill(c_pf_features.btagPf_trackPPar);
    map_ME_DeepJet_["c_pf_btagPf_trackDeltaR"]->Fill(c_pf_features.btagPf_trackDeltaR);
    map_ME_DeepJet_["c_pf_btagPf_trackPParRatio"]->Fill(c_pf_features.btagPf_trackPParRatio);
    map_ME_DeepJet_["c_pf_btagPf_trackSip2dVal"]->Fill(c_pf_features.btagPf_trackSip2dVal);
    map_ME_DeepJet_["c_pf_btagPf_trackSip2dSig"]->Fill(c_pf_features.btagPf_trackSip2dSig);
    map_ME_DeepJet_["c_pf_btagPf_trackSip3dVal"]->Fill(c_pf_features.btagPf_trackSip3dVal);
    map_ME_DeepJet_["c_pf_btagPf_trackSip3dSig"]->Fill(c_pf_features.btagPf_trackSip3dSig);
    map_ME_DeepJet_["c_pf_btagPf_trackJetDistVal"]->Fill(c_pf_features.btagPf_trackJetDistVal);
    map_ME_DeepJet_["c_pf_ptrel"]->Fill(c_pf_features.ptrel);
    map_ME_DeepJet_["c_pf_drminsv"]->Fill(c_pf_features.drminsv);
    map_ME_DeepJet_["c_pf_vtx_ass"]->Fill(c_pf_features.vtx_ass);
    map_ME_DeepJet_["c_pf_puppiw"]->Fill(c_pf_features.puppiw);
    map_ME_DeepJet_["c_pf_chi2"]->Fill(c_pf_features.chi2);
    map_ME_DeepJet_["c_pf_quality"]->Fill(c_pf_features.quality);
  }
  // n_pf candidates
  auto max_n_pf_n = std::min(features.n_pf_features.size(), n_npf_);
  for (std::size_t n_pf_n = 0; n_pf_n < max_n_pf_n; n_pf_n++) {
    const auto& n_pf_features = features.n_pf_features.at(n_pf_n);
    map_ME_DeepJet_["n_pf_ptrel"]->Fill(n_pf_features.ptrel);
    map_ME_DeepJet_["n_pf_deltaR"]->Fill(n_pf_features.deltaR);
    map_ME_DeepJet_["n_pf_isGamma"]->Fill(n_pf_features.isGamma);
    map_ME_DeepJet_["n_pf_hadFrac"]->Fill(n_pf_features.hadFrac);
    map_ME_DeepJet_["n_pf_drminsv"]->Fill(n_pf_features.drminsv);
    map_ME_DeepJet_["n_pf_puppiw"]->Fill(n_pf_features.puppiw);
  }
  // sv candidates
  auto max_sv_n = std::min(features.sv_features.size(), n_sv_);
  for (std::size_t sv_n = 0; sv_n < max_sv_n; sv_n++) {
    const auto& sv_features = features.sv_features.at(sv_n);
    map_ME_DeepJet_["sv_pt"]->Fill(sv_features.pt);
    map_ME_DeepJet_["sv_deltaR"]->Fill(sv_features.deltaR);
    map_ME_DeepJet_["sv_mass"]->Fill(sv_features.mass);
    map_ME_DeepJet_["sv_ntracks"]->Fill(sv_features.ntracks);
    map_ME_DeepJet_["sv_chi2"]->Fill(sv_features.chi2);
    map_ME_DeepJet_["sv_normchi2"]->Fill(sv_features.normchi2);
    map_ME_DeepJet_["sv_dxy"]->Fill(sv_features.dxy);
    map_ME_DeepJet_["sv_dxysig"]->Fill(sv_features.dxysig);
    map_ME_DeepJet_["sv_d3d"]->Fill(sv_features.d3d);
    map_ME_DeepJet_["sv_d3dsig"]->Fill(sv_features.d3dsig);
    map_ME_DeepJet_["sv_costhetasvpv"]->Fill(sv_features.costhetasvpv);
    map_ME_DeepJet_["sv_enratio"]->Fill(sv_features.enratio);
  }
  map_ME_DeepJet_["max_cpf_n"]->Fill(max_c_pf_n);
  map_ME_DeepJet_["max_npf_n"]->Fill(max_n_pf_n);
  map_ME_DeepJet_["max_sv_n"]->Fill(max_sv_n);
  map_ME_DeepJet_["pv_n"]->Fill(features.npv);
}

void MiniAODTagInfoAnalyzer::analyzeTagInfoParticleNet(const reco::DeepBoostedJetTagInfo* taginfo){
  const auto& features = taginfo->features();
}

void MiniAODTagInfoAnalyzer::analyzeTagInfoUnifiedParticleTransformerAK4(const reco::UnifiedParticleTransformerAK4TagInfo* taginfo){
  const auto& features = taginfo->features();

  size_t n_cpf_ = (unsigned int)29;
  size_t n_lt_ = (unsigned int)5;
  size_t n_npf_ = (unsigned int)25;
  size_t n_sv_ = (unsigned int)5;

  auto max_c_pf_n = std::min(features.c_pf_features.size(), (std::size_t)n_cpf_);
  for (std::size_t c_pf_n = 0; c_pf_n < max_c_pf_n; c_pf_n++) {

    const auto& c_pf_features = features.c_pf_features.at(c_pf_n);
    map_ME_UParT_["c_pf_btagPf_trackEtaRel"]->Fill(c_pf_features.btagPf_trackEtaRel);
    map_ME_UParT_["c_pf_btagPf_trackPtRel"]->Fill(c_pf_features.btagPf_trackPtRel);
    map_ME_UParT_["c_pf_btagPf_trackPPar"]->Fill(c_pf_features.btagPf_trackPPar);
    map_ME_UParT_["c_pf_btagPf_trackDeltaR"]->Fill(c_pf_features.btagPf_trackDeltaR);
    map_ME_UParT_["c_pf_btagPf_trackPParRatio"]->Fill(c_pf_features.btagPf_trackPParRatio);
    map_ME_UParT_["c_pf_btagPf_trackSip2dVal"]->Fill(c_pf_features.btagPf_trackSip2dVal);
    map_ME_UParT_["c_pf_btagPf_trackSip2dSig"]->Fill(c_pf_features.btagPf_trackSip2dSig);
    map_ME_UParT_["c_pf_btagPf_trackSip3dVal"]->Fill(c_pf_features.btagPf_trackSip3dVal);
    map_ME_UParT_["c_pf_btagPf_trackSip3dSig"]->Fill(c_pf_features.btagPf_trackSip3dSig);
    map_ME_UParT_["c_pf_btagPf_trackJetDistVal"]->Fill(c_pf_features.btagPf_trackJetDistVal);
    map_ME_UParT_["c_pf_ptrel"]->Fill(c_pf_features.ptrel);
    map_ME_UParT_["c_pf_drminsv"]->Fill(c_pf_features.drminsv);
    map_ME_UParT_["c_pf_vtx_ass"]->Fill(c_pf_features.vtx_ass);
    map_ME_UParT_["c_pf_puppiw"]->Fill(c_pf_features.puppiw);
    map_ME_UParT_["c_pf_chi2"]->Fill(c_pf_features.chi2);
    map_ME_UParT_["c_pf_quality"]->Fill(c_pf_features.quality);
    map_ME_UParT_["c_pf_charge"]->Fill(c_pf_features.charge);
    map_ME_UParT_["c_pf_dz"]->Fill(c_pf_features.dz);
    map_ME_UParT_["c_pf_btagPf_trackDecayLen"]->Fill(c_pf_features.btagPf_trackDecayLen);
    map_ME_UParT_["c_pf_HadFrac"]->Fill(c_pf_features.HadFrac);
    map_ME_UParT_["c_pf_CaloFrac"]->Fill(c_pf_features.CaloFrac);
    map_ME_UParT_["c_pf_pdgID"]->Fill(c_pf_features.pdgID);
    map_ME_UParT_["c_pf_lostInnerHits"]->Fill(c_pf_features.lostInnerHits);
    map_ME_UParT_["c_pf_numberOfPixelHits"]->Fill(c_pf_features.numberOfPixelHits);
    map_ME_UParT_["c_pf_numberOfStripHits"]->Fill(c_pf_features.numberOfStripHits);
  }

  auto max_lt_n = std::min(features.lt_features.size(), (std::size_t)n_lt_);
  for (std::size_t lt_n = 0; lt_n < max_lt_n; lt_n++) {

    const auto& lt_features = features.lt_features.at(lt_n);
    map_ME_UParT_["lt_btagPf_trackEtaRel"]->Fill(lt_features.btagPf_trackEtaRel);
    map_ME_UParT_["lt_btagPf_trackPtRel"]->Fill(lt_features.btagPf_trackPtRel);
    map_ME_UParT_["lt_btagPf_trackPPar"]->Fill(lt_features.btagPf_trackPPar);
    map_ME_UParT_["lt_btagPf_trackDeltaR"]->Fill(lt_features.btagPf_trackDeltaR);
    map_ME_UParT_["lt_btagPf_trackPParRatio"]->Fill(lt_features.btagPf_trackPParRatio);
    map_ME_UParT_["lt_btagPf_trackSip2dVal"]->Fill(lt_features.btagPf_trackSip2dVal);
    map_ME_UParT_["lt_btagPf_trackSip2dSig"]->Fill(lt_features.btagPf_trackSip2dSig);
    map_ME_UParT_["lt_btagPf_trackSip3dVal"]->Fill(lt_features.btagPf_trackSip3dVal);
    map_ME_UParT_["lt_btagPf_trackSip3dSig"]->Fill(lt_features.btagPf_trackSip3dSig);
    map_ME_UParT_["lt_btagPf_trackJetDistVal"]->Fill(lt_features.btagPf_trackJetDistVal);
    map_ME_UParT_["lt_drminsv"]->Fill(lt_features.drminsv);
    map_ME_UParT_["lt_charge"]->Fill(lt_features.charge);
    map_ME_UParT_["lt_puppiw"]->Fill(lt_features.puppiw);
    map_ME_UParT_["lt_chi2"]->Fill(lt_features.chi2);
    map_ME_UParT_["lt_quality"]->Fill(lt_features.quality);
    map_ME_UParT_["lt_lostInnerHits"]->Fill(lt_features.lostInnerHits);
    map_ME_UParT_["lt_numberOfPixelHits"]->Fill(lt_features.numberOfPixelHits);
    map_ME_UParT_["lt_numberOfStripHits"]->Fill(lt_features.numberOfStripHits);
  }

  auto max_n_pf_n = std::min(features.n_pf_features.size(), (std::size_t)n_npf_);
  for (std::size_t n_pf_n = 0; n_pf_n < max_n_pf_n; n_pf_n++) {
    const auto& n_pf_features = features.n_pf_features.at(n_pf_n);
    map_ME_UParT_["n_pf_ptrel"]->Fill(n_pf_features.ptrel);
    map_ME_UParT_["n_pf_etarel"]->Fill(n_pf_features.etarel);
    map_ME_UParT_["n_pf_phirel"]->Fill(n_pf_features.phirel);
    map_ME_UParT_["n_pf_deltaR"]->Fill(n_pf_features.deltaR);
    map_ME_UParT_["n_pf_isGamma"]->Fill(n_pf_features.isGamma);
    map_ME_UParT_["n_pf_hadFrac"]->Fill(n_pf_features.hadFrac);
    map_ME_UParT_["n_pf_drminsv"]->Fill(n_pf_features.drminsv);
    map_ME_UParT_["n_pf_puppiw"]->Fill(n_pf_features.puppiw);
  }

  auto max_sv_n = std::min(features.sv_features.size(), (std::size_t)n_sv_);
  for (std::size_t sv_n = 0; sv_n < max_sv_n; sv_n++) {
    const auto& sv_features = features.sv_features.at(sv_n);
    map_ME_UParT_["sv_pt"]->Fill(sv_features.pt);
    map_ME_UParT_["sv_deltaR"]->Fill(sv_features.deltaR);
    map_ME_UParT_["sv_mass"]->Fill(sv_features.mass);
    map_ME_UParT_["sv_etarel"]->Fill(sv_features.etarel);
    map_ME_UParT_["sv_phirel"]->Fill(sv_features.phirel);
    map_ME_UParT_["sv_ntracks"]->Fill(sv_features.ntracks);
    map_ME_UParT_["sv_chi2"]->Fill(sv_features.chi2);
    map_ME_UParT_["sv_normchi2"]->Fill(sv_features.normchi2);
    map_ME_UParT_["sv_dxy"]->Fill(sv_features.dxy);
    map_ME_UParT_["sv_dxysig"]->Fill(sv_features.dxysig);
    map_ME_UParT_["sv_d3d"]->Fill(sv_features.d3d);
    map_ME_UParT_["sv_d3dsig"]->Fill(sv_features.d3dsig);
    map_ME_UParT_["sv_costhetasvpv"]->Fill(sv_features.costhetasvpv);
    map_ME_UParT_["sv_enratio"]->Fill(sv_features.enratio);
  }

  map_ME_UParT_["max_cpf_n"]->Fill(max_c_pf_n);
  map_ME_UParT_["max_lt_n"]->Fill(max_lt_n);
  map_ME_UParT_["max_npf_n"]->Fill(max_n_pf_n);
  map_ME_UParT_["max_sv_n"]->Fill(max_sv_n);
}

//define this as a plug-in
DEFINE_FWK_MODULE(MiniAODTagInfoAnalyzer);
