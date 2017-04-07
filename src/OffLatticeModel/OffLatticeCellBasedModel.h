#ifndef CIS_OFF_LATTICE_CELL_BASED_MODEL_H
#define CIS_OFF_LATTICE_CELL_BASED_MODEL_H

#include "../Core/CellBasedModel.h"
#include "../Core/SquareLattice.h"
#include "OffLatticeParameters.h"
#include "OffLatticeCell.h"

typedef SquareLattice<OffLatticeCell>::iterator CellIterator;
typedef SquareLattice<OffLatticeCell>::local_iterator LocalCellIterator;

// second element is true if infinite
typedef std::pair<double, bool> Energy;

#define OL_PARAMS  static_cast<OffLatticeParameters*>(mParams)

class OffLatticeCellBasedModel : public CellBasedModel
{
protected:

    // holds all of the cells
    SquareLattice<OffLatticeCell> mCellPopulation;
    
public:

    // constructor
    OffLatticeCellBasedModel() {}
    OffLatticeCellBasedModel(OffLatticeParameters*);

    // single time step, Monte Carlo step
    void oneTimeStep(double);
    void oneMCStep();

    // update system
    void updateDrugs(double);
    void doTrial(OffLatticeCell&);
    void checkMitosis(OffLatticeCell&);

    // relevant functions for attempting/accepting trials
    virtual void attemptTrial(OffLatticeCell&) = 0;
    virtual bool acceptTrial(Energy, Energy, unsigned, unsigned) const = 0;
    virtual Energy calculateHamiltonian(const OffLatticeCell&) = 0;
    virtual unsigned numNeighbors(const OffLatticeCell&) = 0;

    // check hard conditions on cell placement
    bool checkOverlap(const OffLatticeCell&);
    bool checkBoundary(const OffLatticeCell&);

    // do trials
    void growth(OffLatticeCell&);
    void translation(OffLatticeCell&);
    void deformation(OffLatticeCell&);
    void rotation(OffLatticeCell&);    

    // record the current stat of the population
    void recordPopulation();

    // size of cell population
    unsigned size() const {return mCellPopulation.size();}
};

#endif
