//-------------------------------------------------------------------
// $Id: formula-parser.h 1373 2009-07-22 12:25:22Z gems $
//
// Declaration of TFormula - gems chemical formulas parser
//
// Rewritten from C to C++ by S.Dmytriyeva
// Copyright (C) 1995-2016  D.Kulik, S.Dmytriyeva
//
// This file is part of a GEM-Selektor library for thermodynamic
// modelling by Gibbs energy minimization
//
// This file may be distributed under the terms of GEMS4 Development
// Quality Assurance Licence (GEMS4.QAL)
//
// See http://gems.web.psi.ch/ for more information
// E-mail: gems2.support@psi.ch
//-------------------------------------------------------------------
//

#include <string>
#include <list>
#include <vector>

namespace FormulaParser {

extern const char* NOISOTOPE_CLASS;
extern const char* CHARGE_CLASS;
extern const char* CHARGE_NAME;


class BaseParser  // stack for analyzing formula
{

protected:

    enum BRACKETS
    {
      LBRACKET1     ='(', RBRACKET1     =')',
      LBRACKET2     ='[', RBRACKET2     =']',
      LBRACKET3     ='{',  RBRACKET3     ='}',
      B_ISOTOPE     = '/', B_VALENT      ='|',
      PSUR_L_PLUS  =':'
    };

    /// Is capital letter
    inline bool isUpperCaseLetter( char ch )
    {
        return( (ch>='A' && ch<='Z') || ch=='$' );
    }
    /// Is lowercase letter
    inline bool isLowerCaseLetter( char ch )
    {
        return( (ch>='a' && ch<='z') ||  ch == '_' );
    }

    /// Skip escape sequences  (" \n\t\r")
    virtual void xblanc( std::string& str );

    /// Read next real from string
    virtual void getReal( double& real, std::string& cur);

public:

    BaseParser(){}
    virtual ~BaseParser(){}

};

/// Description of parsed Element
struct Element
{
    std::string symbol;
    std::string symbol_isotope;
    int valence;              // valence IC
    double stoich;          // stoich. coef.

    Element( const char* aIck, const char* aIso, int aVal, double aStoc ):
            symbol(aIck), symbol_isotope(aIso), valence(aVal), stoich(aStoc)
    {}
    Element( const Element& data ):
            valence(data.valence), stoich(data.stoich)
    {
        symbol = data.symbol;
        symbol_isotope = data.symbol_isotope;
    }
};

/// Parser for Chemical Formula
class ChemicalFormulaParser : public BaseParser
{
    std::string formula;
    std::string charge;

    const char* CHARGE_TOK    ="+-@";

    enum  CHARGE_FIELD
    {
      CHARGE_NUL    ='@',
      CHARGE_PLUS   ='+',
      CHARGE_MINUS  ='-'
    };

protected:

    void icadd(  std::list<Element>& itt_, const char *icn,
                 const char *iso, int val, double csto );
    void icadd(  std::list<Element>& itt_, Element& it );
    int  ictcomp( std::list<Element>::iterator& it, std::string& ick, int val );
    void addCharge(std::list<Element>& tt);
    void scanCharge();
    void scanFterm( std::list<Element>& itt_, std::string& startPos, char endSimb );
    void scanElem( std::list<Element>& itt_, std::string& cur );
    void scanValence( int& val, std::string& cur);
    void scanIsotope( std::string& isotop, std::string& cur);
    void scanICsymb(  std::string& icName, std::string& cur);

public:

    ChemicalFormulaParser(){}
    ~ChemicalFormulaParser();

    std::list<Element> parse( const std::string& formula );

};


/// Description of Moiety Element
struct Moiety
{
    std::string name;
    int  site;          // sublattice site
    double nj;          // Moiety-site occupancy.

    Moiety( const char* aName, int aSite, double aNj ):
       site(aSite), nj(aNj)
    {
       name = "{" + std::string(aName) + "}" + std::to_string(site);
    }

    Moiety( const Moiety& data ):
       site(data.site), nj(data.nj)
    {
        name = data.name;
    }
};


/// Parser for Moiety
class MoietyParser : public BaseParser
{
    std::string formula;
    int nSites;             // number of sites in formula

public:

    MoietyParser(){}
    ~MoietyParser();

   int parse( const std::string& aformula, std::vector<Moiety>&  moit_ );

};

}
