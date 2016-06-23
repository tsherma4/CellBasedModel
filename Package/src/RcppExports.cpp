// This file was generated by Rcpp::compileAttributes
// Generator token: 10BE3573-1514-4C36-9D1C-5A225CD40393

#include <Rcpp.h>

using namespace Rcpp;

// CellModel
Rcpp::NumericMatrix CellModel(int initialNum, int runTime, double density, double meanGrowth, double varGrowth, double maxMigration, double maxDeform, double maxRotate, double epsilon, double delta, int outIncrement, int randSeed);
RcppExport SEXP CellModel_CellModel(SEXP initialNumSEXP, SEXP runTimeSEXP, SEXP densitySEXP, SEXP meanGrowthSEXP, SEXP varGrowthSEXP, SEXP maxMigrationSEXP, SEXP maxDeformSEXP, SEXP maxRotateSEXP, SEXP epsilonSEXP, SEXP deltaSEXP, SEXP outIncrementSEXP, SEXP randSeedSEXP) {
BEGIN_RCPP
    Rcpp::RObject __result;
    Rcpp::RNGScope __rngScope;
    Rcpp::traits::input_parameter< int >::type initialNum(initialNumSEXP);
    Rcpp::traits::input_parameter< int >::type runTime(runTimeSEXP);
    Rcpp::traits::input_parameter< double >::type density(densitySEXP);
    Rcpp::traits::input_parameter< double >::type meanGrowth(meanGrowthSEXP);
    Rcpp::traits::input_parameter< double >::type varGrowth(varGrowthSEXP);
    Rcpp::traits::input_parameter< double >::type maxMigration(maxMigrationSEXP);
    Rcpp::traits::input_parameter< double >::type maxDeform(maxDeformSEXP);
    Rcpp::traits::input_parameter< double >::type maxRotate(maxRotateSEXP);
    Rcpp::traits::input_parameter< double >::type epsilon(epsilonSEXP);
    Rcpp::traits::input_parameter< double >::type delta(deltaSEXP);
    Rcpp::traits::input_parameter< int >::type outIncrement(outIncrementSEXP);
    Rcpp::traits::input_parameter< int >::type randSeed(randSeedSEXP);
    __result = Rcpp::wrap(CellModel(initialNum, runTime, density, meanGrowth, varGrowth, maxMigration, maxDeform, maxRotate, epsilon, delta, outIncrement, randSeed));
    return __result;
END_RCPP
}
