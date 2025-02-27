//-------------------------------------------------
//
/**  \class L1MuBMAssignmentUnit
 *
 *   Assignment Unit:
 *
 *   assigns pt, charge, phi
 *   and quality to a muon candidate
 *   found by the Track Assembler
 *
 *
 *
 *   N. Neumeister            CERN EP
 *   J. Troconiz              UAM Madrid
 */
//
//--------------------------------------------------
#ifndef L1MUBM_ASSIGNMENT_UNIT_H
#define L1MUBM_ASSIGNMENT_UNIT_H

//---------------
// C++ Headers --
//---------------

#include <vector>
#include <iosfwd>

//----------------------
// Base Class Headers --
//----------------------

//------------------------------------
// Collaborating Class Declarations --
//------------------------------------
#include "L1Trigger/L1TMuonBarrel/interface/L1MuBMLUTHandler.h"
#include "CondFormats/L1TObjects/interface/L1TMuonBarrelParams.h"
#include "CondFormats/DataRecord/interface/L1TMuonBarrelParamsRcd.h"

#include "FWCore/Framework/interface/ESHandle.h"
#include "FWCore/Framework/interface/EventSetup.h"
#include "DataFormats/L1TMuon/interface/BMTF/L1MuBMAddressArray.h"

class L1MuBMTrackSegPhi;
class L1MuBMSectorProcessor;
class L1MuBMLUTHandler;
//              ---------------------
//              -- Class Interface --
//              ---------------------

class L1MuBMAssignmentUnit {
public:
  /// constructor
  L1MuBMAssignmentUnit(L1MuBMSectorProcessor& sp, int id);

  /// destructor
  ~L1MuBMAssignmentUnit();

  /// run Assignment Unit
  void run(const L1TMuonBarrelParams& mbtfParams);

  /// reset Assignment Unit
  void reset();

  /// assign phi
  void PhiAU(const L1TMuonBarrelParams&);

  /// assign pt and charge
  void PtAU(const L1TMuonBarrelParams&);

  /// assign quality
  void QuaAU();
  unsigned int Quality();

  /// set precision of phi and phib
  void setPrecision();

private:
  /// Track Segment Router
  void TSR();

  /// get track segment from a given station
  const L1MuBMTrackSegPhi* getTSphi(int station) const;

  /// convert sector Id to 8 bit code (= sector center)
  static int convertSector(int);

  /// determine charge
  static int getCharge(L1MuBMLUTHandler::PtAssMethod);

  /// determine pt assignment method
  L1MuBMLUTHandler::PtAssMethod getPtMethod() const;
  L1MuBMLUTHandler::PtAssMethod getPt1Method(L1MuBMLUTHandler::PtAssMethod) const;
  L1MuBMLUTHandler::PtAssMethod getPt2Method(L1MuBMLUTHandler::PtAssMethod) const;

  /// calculate bend angle
  int getPtAddress(L1MuBMLUTHandler::PtAssMethod, int bendcharge = 0) const;
  int getPt1Address(L1MuBMLUTHandler::PtAssMethod) const;
  int getPt2Address(L1MuBMLUTHandler::PtAssMethod) const;

  /// build difference of two phi values
  int phiDiff(int stat1, int stat2) const;

private:
  L1MuBMSectorProcessor& m_sp;
  int m_id;

  L1MuBMAddressArray m_addArray;
  std::vector<const L1MuBMTrackSegPhi*> m_TSphi;
  L1MuBMLUTHandler::PtAssMethod m_ptAssMethod;

  L1MuBMLUTHandler* thePtaLUTs;   ///< pt-assignment look-up tables
  L1MuBMLUTHandler* thePhiLUTs;   ///< phi-assignment look-up tables
  unsigned short nbit_phi = 12;   ///< # of bits used for pt-assignment
  unsigned short nbit_phib = 10;  ///< # of bits used for pt-assignment
};

#endif
