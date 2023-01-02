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

\*---------------------------------------------------------------------------*/

#include "SpalartAllmarasDNS.H"
#include "fvOptions.H"
#include "bound.H"
#include "wallDist.H"
#include <iostream>

// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

namespace Foam
{
namespace RASModels
{

// * * * * * * * * * * * * Protected Member Functions  * * * * * * * * * * * //

template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::chi() const
{
    return nuTilda_/this->nu();
}


template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::fv1
(
    const volScalarField& chi
) const
{
    const volScalarField chi3(pow3(chi));
    //Info << "begin fv1" << chi.dimensions() << nl;

    IOdictionary DNSDict_
    (
        IOobject
        (
            "modifiedFunc",
            this->runTime_.constant(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    List<scalar> chitmp(DNSDict_.lookup("chitmp"));
    List<scalar> fv1tmp(DNSDict_.lookup("fv1tmp"));

    volScalarField fv1v(chi3);

    int j;

    // internal points
    forAll(fv1v,k)
    {
        for (j = 0; ( j < chitmp.size())&&(chi[k]>chitmp[j] ); j++)
        {
        }

        //j = static_cast<int>(chi[k]/0.1) + 1;
        //Pout << chi[k]/0.1 << static_cast<int>(chi[k]/0.1) << nl;

        if (j >= chitmp.size()-1)
        {
            fv1v[k] = 1.0;
        }
        else
        {
            fv1v[k] = (fv1tmp[j] - fv1tmp[j-1]) * (chi[k] - chitmp[j-1])/(chitmp[j] - chitmp[j-1]) + fv1tmp[j-1];
            //Pout << "fv1: " << k << " , " << chi[k] << " , " << fv1v[k] << nl;
        }

		if (chi[k] > 50)
		{
			//Pout << chi[k] << ','  << fv1v[k] << nl;
		}
    }

    forAll(fv1v.boundaryField(), patchID) 
    {
        forAll (fv1v.boundaryField()[patchID],facei) 
        {
            for (j = 0; ( j < chitmp.size())&&(chi.boundaryField()[patchID][facei]>chitmp[j] ); j++)
            {
            }
            if (j >= chitmp.size()-1)
            {
                fv1v.boundaryFieldRef()[patchID][facei] = 1.0;
            }
            else
            {
                fv1v.boundaryFieldRef()[patchID][facei] = (fv1tmp[j] - fv1tmp[j-1]) * (chi.boundaryField()[patchID][facei] - chitmp[j-1])/(chitmp[j] - chitmp[j-1]) + fv1tmp[j-1];
            }

		    if (chi.boundaryField()[patchID][facei] > 50)
		    {
			    //Pout << chi.boundaryField()[patchID][facei] << ','  << fv1v.boundaryField()[patchID][facei] << nl;
		    }
        }
    }

    tmp<volScalarField> fv1vv
    (
        new volScalarField
        (   
            IOobject
            (   
                "fv1vv",
                this->runTime_.constant(),
                this->mesh_,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),  
            fv1v
        )
    );

    //Info << "end fv1"<< nl;
    return fv1vv;
}


template<class BasicTurbulenceModel>
tmp<volScalarField::Internal> SpalartAllmarasDNS<BasicTurbulenceModel>::fv2
(
    const volScalarField::Internal& chi,
    const volScalarField::Internal& fv1
) const
{

    //Info << "begin fv2"<< nl;

    IOdictionary DNSDict_
    (
        IOobject
        (
            "modifiedFunc",
            this->runTime_.constant(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    List<scalar> chitmp(DNSDict_.lookup("chitmp"));
    List<scalar> fv2tmp(DNSDict_.lookup("fv2tmp"));

    volScalarField::Internal fv2v(fv1);

    int j;

    forAll(fv2v,k)
    {
        for (j = 0; ( j < chitmp.size())&&(chi[k]>chitmp[j] ); j++)
        {
        }

        //j = static_cast<int>(chi[k]/0.1) + 1;

        if (j >= chitmp.size()-1)
        {
            fv2v[k] = 0;
        }
		else if (j == 0)
		{
			fv2v[k] = 1;
		}
        else
        {
            fv2v[k] = (fv2tmp[j] - fv2tmp[j-1]) * (chi[k] - chitmp[j-1])/(chitmp[j] - chitmp[j-1]) + fv2tmp[j-1];
            //Pout << "fv2: " << k << " , " << chi[k] << " , " << fv2v[k] << nl;
        }
    }
    
    //tmp<volScalarField::Internal> fv2vv = fv2v;

    tmp<volScalarField::Internal> fv2vv
    (
        new volScalarField::Internal
        (   
            IOobject
            (   
                "fv2vv",
                this->runTime_.constant(),
                this->mesh_,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),  
            fv2v
        )
    );

    //Info << "end fv2"<< nl;
    return fv2vv;
}

template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::ft2
(
    const volScalarField& chi
) const
{
    const volScalarField chi2(pow(chi,2));

    return Ct3_*exp(-1.0*Ct4_*chi2);
}


template<class BasicTurbulenceModel>
tmp<volScalarField::Internal> SpalartAllmarasDNS<BasicTurbulenceModel>::Stilda()
const
{
    const volScalarField chi(this->chi());

    const volScalarField fv1(this->fv1(chi));

    const volScalarField::Internal Omega
    (
        ::sqrt(scalar(2))*mag(skew(fvc::grad(this->U_)().v()))
    );

    return
    (
        Omega + fv2(chi(), fv1())*nuTilda_()/sqr(kappa_*y_)
    );
}


template<class BasicTurbulenceModel>
tmp<volScalarField::Internal> SpalartAllmarasDNS<BasicTurbulenceModel>::fw
(
    const volScalarField::Internal& Stilda
) const
{
    //Info << "begin fw"<< nl;

    const volScalarField::Internal r
    (
        min
        (
            nuTilda_
           /(
               max
               (
                   Stilda,
                   dimensionedScalar(Stilda.dimensions(), 1e-10)
               )
              *sqr(kappa_*y_)
            ),
            scalar(10)
        )
    );

    const volScalarField::Internal g(r + Cw2_*(pow6(r) - r));

    IOdictionary DNSDict_
    (
        IOobject
        (
            "modifiedFunc",
            this->runTime_.constant(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::NO_WRITE
        )
    );

    List<scalar> rtmp(DNSDict_.lookup("rtmp"));
    List<scalar> fwtmp(DNSDict_.lookup("fwtmp"));

    volScalarField::Internal fwv(g);

    int j;

    forAll(fwv,k)
    {
        for (j = 0; ( j < rtmp.size())&&(r[k]>rtmp[j] ); j++)
        {
        }

        //j =static_cast<int>(r[k]/0.01) + 1;

        if (j >= rtmp.size())
        {
            fwv[k] = fwtmp[ rtmp.size()-1 ];
        }
        else
        {
            fwv[k] = (fwtmp[j] - fwtmp[j-1]) * (r[k] - rtmp[j-1])/(rtmp[j] - rtmp[j-1]) + fwtmp[j-1];
            //Pout << "fw: " << k << " , " << r[k] << " , " << fwv[k] << nl;
        }
        
    }
    
    //tmp<volScalarField::Internal> fwvv = fwv;

    tmp<volScalarField::Internal> fwvv
    (
        new volScalarField::Internal
        (   
            IOobject
            (   
                "fwvv",
                this->runTime_.constant(),
                this->mesh_,
                IOobject::NO_READ,
                IOobject::AUTO_WRITE
            ),  
            fwv
        )
    );

    //Info << "end fw"<< nl;
    return fwvv;
}


template<class BasicTurbulenceModel>
void SpalartAllmarasDNS<BasicTurbulenceModel>::correctNut()
{
    this->nut_ = nuTilda_*this->fv1(this->chi());
    this->nut_.correctBoundaryConditions();
    fv::options::New(this->mesh_).correct(this->nut_);

    BasicTurbulenceModel::correctNut();
}


// * * * * * * * * * * * * * * * * Constructors  * * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
SpalartAllmarasDNS<BasicTurbulenceModel>::SpalartAllmarasDNS
(
    const alphaField& alpha,
    const rhoField& rho,
    const volVectorField& U,
    const surfaceScalarField& alphaRhoPhi,
    const surfaceScalarField& phi,
    const transportModel& transport,
    const word& propertiesName,
    const word& type
)
:
    eddyViscosity<RASModel<BasicTurbulenceModel>>
    (
        type,
        alpha,
        rho,
        U,
        alphaRhoPhi,
        phi,
        transport,
        propertiesName
    ),

    sigmaNut_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "sigmaNut",
            this->coeffDict_,
            scalar(2)/scalar(3)
        )
    ),
    kappa_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "kappa",
            this->coeffDict_,
            0.41
        )
    ),

    Cb1_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cb1",
            this->coeffDict_,
            0.1355
        )
    ),
    Cb2_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cb2",
            this->coeffDict_,
            0.622
        )
    ),
    Cw1_(Cb1_/sqr(kappa_) + (scalar(1) + Cb2_)/sigmaNut_),
    Cw2_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cw2",
            this->coeffDict_,
            0.3
        )
    ),
    Cw3_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cw3",
            this->coeffDict_,
            2.0
        )
    ),
    Cv1_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Cv1",
            this->coeffDict_,
            7.1
        )
    ),
    Ct3_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Ct3",
            this->coeffDict_,
            1.2
        )
    ),
    Ct4_
    (
        dimensioned<scalar>::getOrAddToDict
        (
            "Ct4",
            this->coeffDict_,
            0.5
        )
    ),
    nuTilda_
    (
        IOobject
        (
            "nuTilda",
            this->runTime_.timeName(),
            this->mesh_,
            IOobject::MUST_READ,
            IOobject::AUTO_WRITE
        ),
        this->mesh_
    ),

    y_(wallDist::New(this->mesh_).y())
{
    if (type == typeName)
    {
        this->printCoeffs(type);
    }
}


// * * * * * * * * * * * * * * * Member Functions  * * * * * * * * * * * * * //

template<class BasicTurbulenceModel>
bool SpalartAllmarasDNS<BasicTurbulenceModel>::read()
{
    if (eddyViscosity<RASModel<BasicTurbulenceModel>>::read())
    {
        sigmaNut_.readIfPresent(this->coeffDict());
        kappa_.readIfPresent(this->coeffDict());

        Cb1_.readIfPresent(this->coeffDict());
        Cb2_.readIfPresent(this->coeffDict());
        Cw1_ = Cb1_/sqr(kappa_) + (scalar(1) + Cb2_)/sigmaNut_;
        Cw2_.readIfPresent(this->coeffDict());
        Cw3_.readIfPresent(this->coeffDict());
        Cv1_.readIfPresent(this->coeffDict());
        Ct3_.readIfPresent(this->coeffDict());
        Ct4_.readIfPresent(this->coeffDict());

        return true;
    }

    return false;
}


template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::DnuTildaEff() const
{
    return tmp<volScalarField>::New
    (
        "DnuTildaEff",
        (nuTilda_ + this->nu())/sigmaNut_
    );
}


template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::k() const
{
    // (B:Eq. 4.50)
    const scalar Cmu = 0.09;

    return tmp<volScalarField>::New
    (
        IOobject
        (
            IOobject::groupName("k", this->alphaRhoPhi_.group()),
            this->runTime_.timeName(),
            this->mesh_
        ),
        cbrt(this->fv1(this->chi()))
        *nuTilda_
        *::sqrt(scalar(2)/Cmu)
        *mag(symm(fvc::grad(this->U_))),
        this->nut_.boundaryField().types()
    );
}


template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::epsilon() const
{
    // (B:Eq. 4.50)
    const scalar Cmu = 0.09;
    const dimensionedScalar nutSMALL(sqr(dimLength)/dimTime, SMALL);

    return tmp<volScalarField>::New
    (
        IOobject
        (
            IOobject::groupName("epsilon", this->alphaRhoPhi_.group()),
            this->runTime_.timeName(),
            this->mesh_
        ),
        pow(this->fv1(this->chi()), 0.5)
        *pow(::sqrt(Cmu)*this->k(), 2)
        /(nuTilda_ + this->nut_ + nutSMALL),
        this->nut_.boundaryField().types()
    );
}


template<class BasicTurbulenceModel>
tmp<volScalarField> SpalartAllmarasDNS<BasicTurbulenceModel>::omega() const
{
    // (P:p. 384)
    const scalar betaStar = 0.09;
    const dimensionedScalar k0(sqr(dimLength/dimTime), SMALL);

    return tmp<volScalarField>::New
    (
        IOobject
        (
            IOobject::groupName("omega", this->alphaRhoPhi_.group()),
            this->runTime_.timeName(),
            this->mesh_
        ),
        this->epsilon()/(betaStar*(this->k() + k0)),
        this->nut_.boundaryField().types()
    );
}


template<class BasicTurbulenceModel>
void SpalartAllmarasDNS<BasicTurbulenceModel>::correct()
{
    if (!this->turbulence_)
    {
        return;
    }

    {
        // Construct local convenience references
        const alphaField& alpha = this->alpha_;
        const rhoField& rho = this->rho_;
        const surfaceScalarField& alphaRhoPhi = this->alphaRhoPhi_;
        fv::options& fvOptions(fv::options::New(this->mesh_));

        eddyViscosity<RASModel<BasicTurbulenceModel>>::correct();

        const volScalarField::Internal Stilda(this->Stilda());
        const volScalarField::Internal ft2(this->ft2(this->chi()));

        tmp<fvScalarMatrix> nuTildaEqn
        (
            fvm::ddt(alpha, rho, nuTilda_)
          + fvm::div(alphaRhoPhi, nuTilda_)
          - fvm::laplacian(alpha*rho*DnuTildaEff(), nuTilda_)
          - Cb2_/sigmaNut_*alpha*rho*magSqr(fvc::grad(nuTilda_))
         ==
            Cb1_*alpha()*rho()*Stilda*nuTilda_()
          - fvm::Sp( alpha()*rho()*( Cw1_*fw(Stilda)*nuTilda_()/sqr(y_) ), nuTilda_)
          + fvOptions(alpha, rho, nuTilda_)
        );

        nuTildaEqn.ref().relax();
        fvOptions.constrain(nuTildaEqn.ref());
        solve(nuTildaEqn);
        fvOptions.correct(nuTilda_);
        bound(nuTilda_, dimensionedScalar(nuTilda_.dimensions(), Zero));
        nuTilda_.correctBoundaryConditions();
    }

    // Update nut with latest available nuTilda
    correctNut();
}


// * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * //

} // End namespace RASModels
} // End namespace Foam
/*
#include "addToRunTimeSelectionTable.H"
#include "makeTurbulenceModel.H"
#include "RASModel.H"
#include "transportModel.H"
#include "incompressibleTurbulenceModel.H"
#include "IncompressibleTurbulenceModel.H"

namespace Foam
{
		    typedef IncompressibleTurbulenceModel<transportModel> transportModelIncompressibleTurbulenceModel;
			typedef RASModel<transportModelIncompressibleTurbulenceModel> RAStransportModelIncompressibleTurbulenceModel;
}

makeTemplatedTurbulenceModel(transportModelIncompressibleTurbulenceModel, RAS, SpalartAllmarasDNS)
*/

// ************************************************************************* //
