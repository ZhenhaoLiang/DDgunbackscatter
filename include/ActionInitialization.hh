/// \file ActionInitialization.hh
/// \brief Definition of the ActionInitialization class

#ifndef ActionInitialization_h
#define ActionInitialization_h 1
#include "G4String.hh"
#include "G4VUserActionInitialization.hh"

/// Action initialization class.

class ActionInitialization : public G4VUserActionInitialization
{
  public:
    ActionInitialization();
    virtual ~ActionInitialization();

    virtual void BuildForMaster() const;
    virtual void Build() const;
    void SetDataFilenamemy(const G4String &hFilename)
    {
      m_hDataFilename = hFilename;
    }
  private:
    G4String m_hDataFilename = "ac.root"; //default out file
};

#endif

    
