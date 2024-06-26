/*---------------------------------------------------------------------------*\
  =========                 |
  \\      /  F ield         | OpenFOAM: The Open Source CFD Toolbox
   \\    /   O peration     |
    \\  /    A nd           | www.openfoam.com
     \\/     M anipulation  |
-------------------------------------------------------------------------------
    Copyright (C) 2011-2016 OpenFOAM Foundation
    Copyright (C) 2019-2021 OpenCFD Ltd.
-------------------------------------------------------------------------------
License
    This file is part of OpenFOAM.

    OpenFOAM is free software: you can redistribute it and/or modify it
    under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    OpenFOAM is distributed in the hope that it will be useful, but WITHOUT
    ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
    FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
    for more details.

    You should have received a copy of the GNU General Public License
    along with OpenFOAM.  If not, see <http://www.gnu.org/licenses/>.

Class
    Foam::RASModels::SpalartAllmarasNASA

Group
    grpRASTurbulence

Description
    Spalart-Allmaras one-transport-equation linear-eddy-viscosity turbulence
    closure model for incompressible and compressible external flows.

    Required fields
    \verbatim
        nuTilda   | Modified kinematic viscosity [m2/s]
    \endverbatim

    References:
    \verbatim
        Standard model:
            Spalart, P.R., & Allmaras, S.R. (1994).
            A one-equation turbulence model for aerodynamic flows.
            La Recherche Aerospatiale, 1, 5-21.

        Standard model without trip and ft2 terms (tag:R):
            Rumsey, C. (2020).
            The Spalart-Allmaras Turbulence Model.
            Spalart-Allmaras One-Equation Model without ft2 Term (SA-noft2).
            https://turbmodels.larc.nasa.gov/spalart.html#sanoft2
            (Retrieved:12-01-2021).

        Estimation expression for k and epsilon (tag:B), Eq. 4.50:
            Bourgoin, A. (2019).
            Bathymetry induced turbulence modelling the
            Alderney Race site: regional approach with TELEMAC-LES.
            Normandie Université.

        Estimation expressions for omega (tag:P):
            Pope, S. B. (2000).
            Turbulent flows.
            Cambridge, UK: Cambridge Univ. Press
            DOI:10.1017/CBO9780511840531
    \endverbatim

Usage
    Example by using \c constant/turbulenceProperties:
    \verbatim
    RAS
    {
        // Mandatory entries (unmodifiable)
        RASModel        SpalartAllmarasNASA;

        // Optional entries (runtime modifiable)
        turbulence      on;
        printCoeffs     on;

        SpalartAllmarasNASACoeffs
        {
            sigmaNut    0.66666;
            kappa       0.41;
            Cb1         0.1355;
            Cb2         0.622;
            Cw2         0.3;
            Cw3         2.0;
            Cv1         7.1;
            Cs          0.3;
        }
    }
    \endverbatim

Note
  - The model is implemented without the trip-term since the model has almost
    always been used in fully turbulent applications rather than those where
    laminar-turbulent transition occurs.
  - It has been argued that the \c ft2 term is not needed in the absence of the
    trip-term, hence \c ft2 term is also not implementated.
  - The \c Stilda generation term should never be allowed to be zero or negative
    to avoid potential numerical issues and unphysical results for complex
    flows. To this end, a limiter proposed by Spalart (R:Note-1(b)) is applied
    onto \c Stilda where \c Stilda is clipped at \c Cs*Omega with the default
    value of \c Cs=0.3.
  - The model does not produce \c k, \c epsilon or \c omega. Nevertheless,
    these quantities can be estimated by using an approximate expressions for
    turbulent kinetic energy and dissipation rate reported in (B:Eq. 4.50).

SourceFiles
    SpalartAllmarasNASA.C

\*---------------------------------------------------------------------------*/

#ifndef SpalartAllmarasNASA_H
#define SpalartAllmarasNASA_H

#include "RASModel.H"
#include "eddyViscosity.H"

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace RASModels
{

/*---------------------------------------------------------------------------*\
                      Class SpalartAllmarasNASA Declaration
\*---------------------------------------------------------------------------*/

template<class BasicTurbulenceModel>
class SpalartAllmarasNASA
:
    public eddyViscosity<RASModel<BasicTurbulenceModel>>
{
    // Private Member Functions

        //- No copy construct
        SpalartAllmarasNASA(const SpalartAllmarasNASA&) = delete;

        //- No copy assignment
        void operator=(const SpalartAllmarasNASA&) = delete;


protected:

    // Protected Data

        // Model coefficients

            dimensionedScalar sigmaNut_;
            dimensionedScalar kappa_;
            dimensionedScalar Cb1_;
            dimensionedScalar Cb2_;
            dimensionedScalar Cw1_;
            dimensionedScalar Cw2_;
            dimensionedScalar Cw3_;
            dimensionedScalar Cv1_;
            dimensionedScalar Ct3_;
            dimensionedScalar Ct4_;


        // Fields

            //- Modified kinematic viscosity [m2/s]
            volScalarField nuTilda_;

            //- Wall distance
            //  Note: different to wall distance in parent RASModel
            //  which is for near-wall cells only
            const volScalarField::Internal& y_;


    // Protected Member Functions

        tmp<volScalarField> chi() const;

        tmp<volScalarField> fv1(const volScalarField& chi) const;

        tmp<volScalarField::Internal> fv2
        (
            const volScalarField::Internal& chi,
            const volScalarField::Internal& fv1
        ) const;

        tmp<volScalarField> ft2(const volScalarField& chi) const;

        tmp<volScalarField::Internal> Stilda() const;

        tmp<volScalarField::Internal> fw
        (
            const volScalarField::Internal& Stilda
        ) const;


        //- Update nut with the latest available nuTilda
        virtual void correctNut();


public:

    typedef typename BasicTurbulenceModel::alphaField alphaField;
    typedef typename BasicTurbulenceModel::rhoField rhoField;
    typedef typename BasicTurbulenceModel::transportModel transportModel;


    //- Runtime type information
    TypeName("SpalartAllmarasNASA");


    // Constructors

        //- Construct from components
        SpalartAllmarasNASA
        (
            const alphaField& alpha,
            const rhoField& rho,
            const volVectorField& U,
            const surfaceScalarField& alphaRhoPhi,
            const surfaceScalarField& phi,
            const transportModel& transport,
            const word& propertiesName = turbulenceModel::propertiesName,
            const word& type = typeName
        );


    //- Destructor
    virtual ~SpalartAllmarasNASA() = default;


    // Member Functions

        //- Re-read model coefficients if they have changed
        virtual bool read();

        //- Return the effective diffusivity for nuTilda
        tmp<volScalarField> DnuTildaEff() const;

        //- Return the (estimated) turbulent kinetic energy
        virtual tmp<volScalarField> k() const;

        //- Return the (estimated) turbulent kinetic energy dissipation rate
        virtual tmp<volScalarField> epsilon() const;

        //- Return the (estimated) specific dissipation rate
        virtual tmp<volScalarField> omega() const;

        //- Solve the turbulence equations and correct the turbulent viscosity
        virtual void correct();
};


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace Foam

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#ifdef NoRepository
    #include "SpalartAllmarasNASA.C"
#endif

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

#endif

// ************************************************************************* //
