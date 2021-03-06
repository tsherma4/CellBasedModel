% Generated by roxygen2: do not edit by hand
% Please edit documentation in R/class-CellModel.R
\docType{methods}
\name{cellSummary}
\alias{cellSummary}
\alias{cellSummary,CellModel-method}
\title{summary of cell model at a given time}
\usage{
cellSummary(model, time)

\S4method{cellSummary}{CellModel}(model, time)
}
\arguments{
\item{model}{cell model object}

\item{time}{hour of the model to query}
}
\value{
string containing summary of model
}
\description{
summary of cell model at a given time
}
\examples{
data(SampleModels)
cellSummary(modDefault, modDefault@runTime)
}
