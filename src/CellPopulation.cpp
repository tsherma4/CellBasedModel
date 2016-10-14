#include <cmath>
#include <Rcpp.h>

#include "CellPopulation.h"

/* constructor; takes a Parameters object, initial size and density of the population */
CellPopulation::CellPopulation(Parameters *par, unsigned int size, double density) {

	/* store the parameters */
    m_param = par;

	/* initalize the data structure to hold the cells */
    m_population = SpatialHash<Cell>(1.0);

    /* calculate the initial seeding radius */
    double disk_radius = pow(size / density, 0.5);
   
    /* create the cell boundary: cells cannot go outside the boundary */
    CreateBoundary();
  
    /* create dummy cell object to temporarily hold cells */
    Cell* temp;

     /* set the intial growth rates of the cells */
    SetGrowthRates();

    /* seed cells throughout the cell cycle */
    SeedCells();

    /* intialzation; haven't added the drug yet */
    m_drug_added = false;

}

/* create the cell boundary */
CellPopulation::CreateBoundary() {

    /* if boundary is zero (no boundary) */
    if (m_param->GetBoundary() == 0.0) {

        /* set boundary value to maximum allowed */
        m_param->SetBoundary(std::numeric_limits<double>::max());

    /* otherwise check if boundary is too small (must be bigger than initial seeding radius) */
    } else if (m_param->GetBoundary() < disk_radius + 2) {

        /* set boundary to the minimum value */
        m_param->SetBoundary(disk_radius + 2);

    }

}

/* create all the cells */
CellPopulation::CreateCells(int number, double radius) {

   /* create and seed cells in random locations inside the inital radius */
    for (unsigned int i = 0; i < size; i++) {

        /* create new cell at point (0,0) */
        temp = new Cell(Point(0,0), m_param);

        /* get random location inside the radius and the cell there */
        GetRandomLocation(temp, disk_radius);

        /* add cell to the cell population */
        m_population.Insert(temp->GetCoord(), temp);

        /* check if the user wants to cancel the simulation (done from R console) */
        Rcpp::checkUserInterrupt();

    }

}

/* deconstructor */
CellPopulation::~CellPopulation() {
	
    /* iterate through entire cell population */
    SpatialHash<Cell>::full_iterator iter = m_population.begin();
    for (; iter != m_population.end(); ++iter) {

        /* delete every cell */
        delete &iter;

    }

}


Point CellPopulation::GetRandomLocation(Cell* cell, double rad) {

    double dist, ang, x, y;

    do {

        dist = R::runif(0, 1);
        ang = R::runif(0, 2 * M_PI);
        x = rad * pow(dist, 0.5) * cos(ang);
        y = rad * pow(dist, 0.5) * sin(ang);
        cell->SetCoord(Point(x,y));

    } while (!ValidCellPlacement(cell));

    return Point(x, y);

}

bool CellPopulation::ValidCellPlacement(Cell* cell) {

    SpatialHash<Cell>::full_iterator iter = m_population.begin();

	for (; iter != m_population.end(); ++iter) {

        if ((*iter).CellDistance(*cell) < 2) {

            return false;

        }

    }

    return true;

}

void CellPopulation::SetGrowthRates() {

	SpatialHash<Cell>::full_iterator iter = m_population.begin();

	for (; iter != m_population.end(); ++iter) {

		(*iter).SetGrowth(m_param->GetRandomGrowthRate());

    }

}

void CellPopulation::SeedCells() {

	double cycle_time, unif;
	SpatialHash<Cell>::full_iterator iter = m_population.begin();
	for (; iter != m_population.end(); ++iter) {

		unif = R::runif(0,1);
		
		if (unif < 0.75) { //interphase

			(*iter).SetRadius(R::runif(1,m_param->GetMaxRadius()));
            (*iter).SetAxisLength((*iter).GetRadius() * 2);
	
		} else { //mitosis

			(*iter).EnterRandomPointOfMitosis();

		}

	}

}

void CellPopulation::AddDrug() {

    SpatialHash<Cell>::full_iterator iter = m_population.begin();
    for (; iter != m_population.end(); ++iter) {

        double gr = (*iter).GetGrowth();
        (*iter).SetGrowth(gr * m_param->GetDrugEffect(gr));

    }

    m_drug_added = true;

}

void CellPopulation::OneTimeStep() {

    int sz = m_population.size();

    for (int i = 0; i < sz; ++i) {

        Update();

    }

}

void CellPopulation::Update() {

    Cell* rand_cell = m_population.GetRandomValue();
    AttemptTrial(rand_cell);
	CheckMitosis(rand_cell);

}

void CellPopulation::CheckMitosis(Cell* cell) {

	if (cell->ReadyToDivide()) {

		double gr_rate;	
		if (m_param->InheritGrowth()) {

			gr_rate = cell->GetGrowth();

		} else {

			gr_rate = m_param->GetRandomGrowthRate();

            if (m_drug_added) {

                gr_rate *= m_param->GetDrugEffect(gr_rate);

            }

		}

		Point old_key = cell->GetCoord();
		Cell* daughter_cell = new Cell(cell->Divide(), gr_rate);
		m_population.Insert(daughter_cell->GetCoord(), daughter_cell);
		m_population.Update(old_key, cell->GetCoord());

	}

}

void CellPopulation::AttemptTrial(Cell *cell) {

    double interaction = CalculateTotalInteraction(cell);
    int num_neighbors = CalculateNumberOfNeighbors(cell);

    Cell orig = *cell;
    bool growth = cell->DoTrial();

    bool overlap = CheckForCellOverlap(orig.GetCoord(), cell);

    if (overlap || CheckBoundary(cell)) {

        *cell = orig;

    } else if (!growth) {

        m_population.Update(orig.GetCoord(), cell->GetCoord());    
        
        if (!AcceptTrial(interaction, num_neighbors, cell)) {

            m_population.Update(cell->GetCoord(), orig.GetCoord());    
            *cell = orig;

        }

    }

}

bool CellPopulation::CheckForCellOverlap(Point center, Cell* cell) {

	double max_search = std::max(m_param->GetMaxTranslation(), m_param->GetMaxRadius());

	SpatialHash<Cell>::circular_iterator iter
		= m_population.begin(center, max_search);

	for (; iter != m_population.end(center, max_search); ++iter) {

		if ((*iter).CellDistance(*cell) < 0) {

			return true;

		}

	}

    return false;

}

bool CellPopulation::CheckBoundary(Cell* cell) {

    double x_dist = (0.5 * cell->GetAxisLength() - cell->GetRadius()) * cos(cell->GetAxisAngle());
    double y_dist = (0.5 * cell->GetAxisLength() - cell->GetRadius()) * sin(cell->GetAxisAngle());

    Point center_1 = Point(cell->GetCoord().x + x_dist, cell->GetCoord().y + y_dist);
    Point center_2 = Point(cell->GetCoord().x - x_dist, cell->GetCoord().y - y_dist);

    return (center_1.dist(Point(0,0)) + cell->GetRadius() > m_param->GetBoundary()
            || center_2.dist(Point(0,0)) + cell->GetRadius() > m_param->GetBoundary());

}

bool CellPopulation::AcceptTrial(double prev_interaction, double prev_num_neighbors, Cell* cell) {

    double delta_interaction = CalculateTotalInteraction(cell) - prev_interaction;

    if (CalculateNumberOfNeighbors(cell) < prev_num_neighbors) {

        return false;

    } else if (delta_interaction <= 0.0) {

        return true;

    } else {

        double unif = R::runif(0, 1);
        double prob = exp(-1 * delta_interaction);
        return unif < prob;

    }

}

int CellPopulation::CalculateNumberOfNeighbors(Cell *cell) {

    int neighbors = 0;
	SpatialHash<Cell>::circular_iterator iter
		= m_population.begin(cell->GetCoord(), m_param->GetCompressionDELTA() + 1);

	for (; iter != m_population.end(cell->GetCoord(), m_param->GetCompressionDELTA() + 1); ++iter) {

        if (cell->CellDistance(*iter) <= m_param->GetCompressionDELTA()) {

            neighbors++;

        }

    }

    return neighbors;

}
    
double CellPopulation::CalculateTotalInteraction(Cell *cell) {

    double inter, sum = 0.0;

	SpatialHash<Cell>::circular_iterator iter
		= m_population.begin(cell->GetCoord(), m_param->GetCompressionDELTA() + 1);

	for (; iter != m_population.end(cell->GetCoord(), m_param->GetCompressionDELTA() + 1); ++iter) {

        sum += CalculateInteraction(&iter, cell);

    }

    return sum;

}

double CellPopulation::CalculateInteraction(Cell* a, Cell* b) {

    double dist = a->CellDistance(*b);

    if (dist > m_param->GetCompressionDELTA()) {

        return 0.0;

    } else {

        double part = pow(2 * dist / m_param->GetCompressionDELTA() - 1, 2);
        return m_param->GetResistanceEPSILON() * (part - 1);

    }

}

int CellPopulation::size() {

    return m_population.size();

}

/* records the current state of the cell population */
void CellPopulation::RecordPopulation() {

    /* the vector to hold the current population */
    std::vector<double> current_pop;
	
    /* cell population iterator */
    SpatialHash<Cell>::full_iterator iter = m_population.begin();
  
    /* loop through each cell */  
	for (; iter != m_population.end(); ++iter) {

        /* store cell information */
        current_pop.push_back((*iter).GetCoord().x);
        current_pop.push_back((*iter).GetCoord().y);
        current_pop.push_back((*iter).GetRadius());
        current_pop.push_back((*iter).GetAxisLength());
        current_pop.push_back((*iter).GetAxisAngle());
        current_pop.push_back((*iter).GetGrowth());
        current_pop.push_back((*iter).GetCellType());

    }

    m_population_record.push_back(current_pop);

}

Rcpp::List CellPopulation::GetPopulationAsList() {

	return Rcpp::wrap(m_population_record);

}

