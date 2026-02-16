#ifndef DataFormats_JetMatching_JetFlavourInfo_H
#define DataFormats_JetMatching_JetFlavourInfo_H

#include <array>
#include <string>
#include <vector>
#include "DataFormats/HepMCCandidate/interface/GenParticle.h"
#include "DataFormats/HepMCCandidate/interface/GenParticleFwd.h"

/// For fastjet::contrib::FlavInfo handling
#include "fastjet/contrib/FlavInfo.hh"

namespace reco {
  /**\class JetFlavourInfo JetFlavourInfo.h DataFormats/JetMatching/interface/JetFlavourInfo.h
 * \brief Class storing the jet flavour information
 *
 * JetFlavourInfo class stores the jet flavour information based on hadrons
 * and partons clustered inside the jet. It also provides vectors of
 * EDM references to clustered hadrons and partons. The hadron- and parton-based
 * flavours are defined in the JetFlavourClustering producer.
 *
 * The flavour definition algorithms introduced in fastjet::constrib outputs
 * the flavours in the type of fastjet::contrib::FlavInfo, which is fully
 * characterized by its member _flav_content, an int[7] array. To preserve the
 * info inside a FlavInfo object, we define a type reco::FJContribFlavArray
 * which is a typedef of std::vector<int>.
 *  * (For full info see https://github.com/cms-externals/fastjet-contrib/blob/cms/v1.101/IFNPlugin/include/fastjet/contrib/IFNPlugin.hh)
 * 
 * For the result of each flavour definition algorithm, an std::vector is
 * always assigned and will remain empty if no flavour information given.
 *
 * 
 */
  /// typedef for
  /// enumeration class for the fastjet::contrib flavour definition algorithms
  enum class FlavAlgo { kCMP, kGHS, kIFN };
  // Number of fastjet::contrib flavour definition algorithms
  constexpr size_t kAlgoFlavCount = 3;
  constexpr size_t kAlgoArrayLen = 7;

  /// Get algorithm name as string
  inline std::string getAlgoName(const FlavAlgo& algo) {
    switch (algo) {
      case FlavAlgo::kCMP:
        return "CMP";
      case FlavAlgo::kGHS:
        return "GHS";
      case FlavAlgo::kIFN:
        return "IFN";
      default:
        return "Unknown";
    }
  }

  class JetFlavourInfo {
  public:
    JetFlavourInfo(void) : m_hadronFlavour(0), m_partonFlavour(0) {}
    JetFlavourInfo(const int hadronFlavour, const int partonFlavour)
        : m_hadronFlavour(hadronFlavour), m_partonFlavour(partonFlavour) {}
    JetFlavourInfo(const GenParticleRefVector& bHadrons,
                   const GenParticleRefVector& cHadrons,
                   const GenParticleRefVector& partons,
                   const GenParticleRefVector& leptons,
                   const int hadronFlavour,
                   const int partonFlavour)
        : m_bHadrons(bHadrons),
          m_cHadrons(cHadrons),
          m_partons(partons),
          m_leptons(leptons),
          m_hadronFlavour(hadronFlavour),
          m_partonFlavour(partonFlavour) {}

    /// Return a vector of GenParticleRef's to b hadrons clustered inside the jet
    const GenParticleRefVector& getbHadrons() const { return m_bHadrons; }
    /// Return a vector of GenParticleRef's to c hadrons clustered inside the jet
    const GenParticleRefVector& getcHadrons() const { return m_cHadrons; }
    /// Return a vector of GenParticleRef's to partons clustered inside the jet
    const GenParticleRefVector& getPartons() const { return m_partons; }
    /// Return a vector of GenParticleRef's to leptons clustered inside the jet
    const GenParticleRefVector& getLeptons() const { return m_leptons; }
    /// Return the hadron-based flavour
    const int getHadronFlavour() const { return m_hadronFlavour; }
    /// Return the parton-based flavour
    const int getPartonFlavour() const { return m_partonFlavour; }

    /// Set the hadron-based flavour
    void setHadronFlavour(const int hadronFlavour) { m_hadronFlavour = hadronFlavour; }
    /// Set the parton-based flavour
    void setPartonFlavour(const int partonFlavour) { m_partonFlavour = partonFlavour; }

    /// view if flavour by some fastjet::contrib algorithm is defined
    bool haveAlgoFlav(const FlavAlgo& algo) const { return !m_fjContribFlav[static_cast<size_t>(algo)].empty(); }
    /// the casting is important to ensure the algoNum is valid
    bool haveAlgoFlav(const uint8_t& algoNum) const { return haveAlgoFlav(static_cast<FlavAlgo>(algoNum)); }
    /// the casting is important to ensure the algoNum is valid
    bool haveAlgoFlav(const size_t& algoNum) const { return haveAlgoFlav(static_cast<FlavAlgo>(algoNum)); }
    /// Set the flavour defined by some fastjet::contrib algorithm, on one digit, only if an array already exists.
    void setAlgoFlav(const FlavAlgo& algo, const fastjet::contrib::FlavInfo& fjFlavInfo) {
      m_fjContribFlav[static_cast<int>(algo)].assign(fjFlavInfo._flav_content,
                                                     fjFlavInfo._flav_content + kAlgoArrayLen);
    }
    /// Set the flavour defined by some fastjet::contrib algorithm, from one array representing flav.
    void setAlgoFlav(const FlavAlgo& algo, const std::vector<int>& flav) {
      assert(flav.size() == kAlgoArrayLen);
      m_fjContribFlav[static_cast<int>(algo)].assign(flav.begin(), flav.end());
    }
    /// Decode the flavour defined by some fastjet::contrib algorithm, from an uint32_t code.
    /// When "odd && >= 7" or "even && >= 6" is encountered, they are decoded as +/-7 or +/-6. ("as-is")
    void setAlgoFlav(const FlavAlgo& algo, const uint32_t& flavCode) {
      std::vector<int>& flavArray = m_fjContribFlav[static_cast<int>(algo)];
      flavArray.resize(kAlgoArrayLen, 0);
      flavArray[0] = static_cast<int>(flavCode & 0x7);
      uint32_t flavAbs = 0;
      for (unsigned int i = 1; i < kAlgoArrayLen; ++i) {
        flavAbs = (flavCode >> (i * 4 - 1)) & 0xF;
        if (flavAbs == 0)
          flavArray[i] = 0;
        else {
          if (flavAbs & (1 << 3)) {  // qbar
            flavAbs &= ~(1 << 3);
            flavArray[i] = -static_cast<int>(flavAbs);
          } else {  // q
            flavArray[i] = static_cast<int>(flavAbs);
          }
        }
      }
    }
    void setAlgoFlav(const uint8_t& algoNum, const fastjet::contrib::FlavInfo& fjFlavInfo) {
      setAlgoFlav(static_cast<FlavAlgo>(algoNum), fjFlavInfo);
    }
    void setAlgoFlav(const uint8_t& algoNum, const uint32_t& flavCode) {
      setAlgoFlav(static_cast<FlavAlgo>(algoNum), flavCode);
    }
    /// Obtain the flavour defined by some fastjet::contrib algorithm, either by digit or by array.
    int getAlgoFlav(const FlavAlgo& algo, const unsigned int& iflav) const {
      if (m_fjContribFlav[static_cast<int>(algo)].empty()) {
        throw cms::Exception("JetFlavourInfo")
            << "The flavour defined by the fastjet::contrib algorithm " << static_cast<int>(algo) << " is not set.";
      }
      return m_fjContribFlav[static_cast<int>(algo)][iflav];
    }
    const std::vector<int>& getAlgoFlav(const FlavAlgo& algo) const {
      if (m_fjContribFlav[static_cast<int>(algo)].empty()) {
        throw cms::Exception("JetFlavourInfo")
            << "The flavour defined by the fastjet::contrib algorithm " << static_cast<int>(algo) << " is not set.";
      }
      return m_fjContribFlav[static_cast<int>(algo)];
    }
    /// Getting the heaviest flavour of the flavour defined by some fastjet::contrib algorithm.
    int getAlgoFlavLeading(const FlavAlgo& algo) const {
      if (m_fjContribFlav[static_cast<int>(algo)].empty()) {
        throw cms::Exception("JetFlavourInfo")
            << "The flavour defined by the fastjet::contrib algorithm " << static_cast<int>(algo) << " is not set.";
      }
      const std::vector<int>& flavArray = m_fjContribFlav[static_cast<int>(algo)];
      int flavRes = 0;
      if (!flavArray.empty()) {
        for (size_t i = flavArray.size() - 1; i > 0; --i) {
          if (flavArray[i] != 0) {
            flavRes = flavArray[i] > 0 ? i : -i;
            break;
          }
        }
        if (flavRes == 0) {  // Assigned "flavourless"
          flavRes = 21;      // Store as gluon
        }
      }
      return flavRes;
    }
    /// Encoding the full flavour into uint32_t
    /// Coding rule: lowest 3 bits for 0th element from the array, the flag;
    ///              then 4 bits from lowest to highest, in the order "d, u, s, c, b, t"
    ///              For each flavour, highest bit 0/1 is q/qbar,
    ///              and next 3 bits are "none, 1, 2, 3, 4, 5, even and >=6, odd and >=7"
    uint32_t getAlgoFlavCode(const FlavAlgo& algo) const {
      if (m_fjContribFlav[static_cast<int>(algo)].empty()) {
        throw cms::Exception("JetFlavourInfo")
            << "The flavour defined by the fastjet::contrib algorithm " << static_cast<int>(algo) << " is not set.";
      }
      const std::vector<int>& flavArray = m_fjContribFlav[static_cast<int>(algo)];
      uint32_t result = static_cast<uint32_t>(flavArray[0] & 0x7);
      uint32_t flavAbs = 0;
      for (unsigned int i = 1; i < flavArray.size(); ++i) {
        flavAbs = static_cast<uint32_t>(flavArray[i] > 0 ? flavArray[i] : -flavArray[i]);
        if (flavAbs > 7)
          flavAbs = (6 | ((flavArray[i] & 1)));  // even or odd and >= 6
        // Handle the sign.
        if (flavArray[i] < 0)
          flavAbs |= (1 << 3);  // set the sign bit for qbar
        result |= (flavAbs << (i * 4 - 1));
      }
      return result;
    }
    /// Obtain the flavour defined by some fastjet::contrib algorithm, either by digit or by array.
    /// Support uint8_t input as indicator for the algorithm to allow NanoAOD-level reading.
    int getAlgoFlav(const uint8_t& algoNum, const unsigned int& iflav) const {
      return getAlgoFlav(static_cast<FlavAlgo>(algoNum), iflav);
    }
    const std::vector<int>& getAlgoFlav(const uint8_t& algoNum) const { return getAlgoFlav(static_cast<FlavAlgo>(algoNum)); }
    int getAlgoFlavLeading(const uint8_t& algoNum) const { return getAlgoFlavLeading(static_cast<FlavAlgo>(algoNum)); }
    int getAlgoFlavLeading(const size_t& algoNum) const { return getAlgoFlavLeading(static_cast<FlavAlgo>(algoNum)); }
    uint32_t getAlgoFlavCode(const uint8_t& algoNum) const { return getAlgoFlavCode(static_cast<FlavAlgo>(algoNum)); }
    uint32_t getAlgoFlavCode(const size_t& algoNum) const { return getAlgoFlavCode(static_cast<FlavAlgo>(algoNum)); }
    /// When needed, clear the flavour defined by some fastjet::contrib algorithm.
    void clearAlgoFlav(const FlavAlgo& algo) { m_fjContribFlav[static_cast<int>(algo)].clear(); }
    void clearAlgoFlav(const uint8_t& algoNum) { clearAlgoFlav(static_cast<FlavAlgo>(algoNum)); }

  private:
    GenParticleRefVector m_bHadrons;
    GenParticleRefVector m_cHadrons;
    GenParticleRefVector m_partons;
    GenParticleRefVector m_leptons;
    /// Original hadronFlavour and partonFlavour definition.
    int m_hadronFlavour;
    int m_partonFlavour;
    /// fastjet::contrib algorithm flavour definition (arXiv:2205.01109)
    std::array<std::vector<int>, kAlgoFlavCount> m_fjContribFlav;
  };

}  // namespace reco
#endif