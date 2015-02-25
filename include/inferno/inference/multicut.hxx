/** \file multicut.hxx 
    \brief  Implementation of inferno::inference::Multicut  

    \warning To include this header one needs cplex 
    (in CMake ''WITH_CPLEX'')
*/
#pragma once
#ifndef INFERNO_INFERENCE_MULTICUT_HXX
#define INFERNO_INFERENCE_MULTICUT_HXX

#include <algorithm>
#include <vector>
#include <list>
#include <queue>
#include <utility>
#include <string>
#include <iostream>
#include <fstream>
#include <typeinfo>
#include <limits> 
#include <map>
#include <unordered_map>
#include <unordered_set>		



#include "inferno/inferno.hxx"
#include "inferno/utilities/timer.hxx"
#include "inferno/utilities/small_vector.hxx"
#include "inferno/inference/base_discrete_inference.hxx"

#include <ilcplex/ilocplex.h>
//ILOSTLBEGIN

namespace inferno {
namespace inference{

/// \cond HIDDEN_SYMBOLS
class HigherOrderTerm
{
public:
   size_t factorID_;
   bool   potts_;
   size_t valueIndex_;
   std::vector<size_t> lpIndices_;
   HigherOrderTerm(size_t factorID, bool  potts, size_t valueIndex) 
      : factorID_(factorID), potts_(potts),valueIndex_(valueIndex) {}
   HigherOrderTerm() 
      : factorID_(0), potts_(false),valueIndex_(0) {}           
};
/// \endcond      



/** \brief Options object for Multicut algorithm
*/
struct MulticutOptions : public InferenceOptions
{
public:
   enum MWCRounding {NEAREST,DERANDOMIZED,PSEUDODERANDOMIZED};

   int numThreads_;
   bool verbose_;
   bool verboseCPLEX_;
   double cutUp_;
   double timeOut_;
   std::string workFlow_;
   size_t maximalNumberOfConstraintsPerRound_;
   double edgeRoundingValue_;
   MWCRounding MWCRounding_;
   size_t reductionMode_;
   std::vector<bool> allowCutsWithin_;

   /// \param numThreads number of threads that should be used (default = 0 [automatic])
   /// \param cutUp value which the optima at least has (helps to cut search-tree)
   MulticutOptions
   (
      int numThreads=0,
      double cutUp=1.0e+75
      )
      : numThreads_(numThreads), verbose_(false),verboseCPLEX_(false), cutUp_(cutUp),
        timeOut_(36000000), maximalNumberOfConstraintsPerRound_(1000000),
        edgeRoundingValue_(0.00000001),MWCRounding_(NEAREST), reductionMode_(3)
      {};
};


/** \brief Multicut Algorithm

    <b>Dependencies:</b>

    - This algorithm defends on Cplex. WITH_CPLEX must be defined within
        CMake to enable this algorithm

    <b>References:</b>


    - [1] J. Kappes, M. Speth, B. Andres, G. Reinelt and C. Schnoerr, "Globally Optimal Image Partitioning by Multicuts", EMMCVPR 2011\n
    - [2] J. Kappes, M. Speth, G. Reinelt and C. Schnoerr, "Higher-order Segmentation via Multicuts", Technical Report (http://ipa.iwr.uni-heidelberg.de/ipabib/Papers/kappes-2013-multicut.pdf)\n

    This code also supports asymetric multyway cuts as discibed in:\n

    - [3] T. Kroeger, J. Kappes, T. Beier, U. Koethe,  and F.A. Hamprecht, "Asymmetric Cuts: Joint Image Labeling and Partitioning", GCPR 2014\n

    This code was also used in

    - [4] J. Kappes, M. Speth, G. Reinelt, and C. Schnoerr, “Towards Efficient and Exact MAP-Inference for Large Scale Discrete Computer Vision Problems via Combinatorial Optimization”. CVPR, 2013\n
    - [5] J. Kappes, B. Andres, F. Hamprecht, C. Schnoerr, S. Nowozin, D. Batra, S. Kim, B. Kausler, J. Lellmann, N. Komodakis, and C. Rother, “A Comparative Study of Modern Inference Techniques for Discrete Energy Minimization Problem”, CVPR, 2013.
  
    <b>Multicut :</b>
    - Cite: [1] and [2]
    - Maximum factor order : potts (oo) generalized potts (4 - can be extended to N)
    - Maximum number of labels : oo
    - Restrictions : functions are arbitrary unary terms or generalized potts terms (positive or negative)
                    all variables have the same labelspace (practical no theoretical restriction) 
                    the number of states is at least as large as the order of a generalized potts function (practical no theoretical restriction)
    - Convergent :   Converge to the global optima if integer and cycle-constraints are enforced
  
    - see [2] for further details.
    
    <b>Usage :</b>
    
    See example???

    \ingroup discrete_inference optimal_discrete_inference
*/
template<class MODEL>
class Multicut : public DiscreteInferenceBase<Multicut<MODEL>, MODEL >
{
public:

   typedef MODEL Model;
   typedef MulticutOptions Options;
   typedef size_t LPIndexType;

   //typedef visitors::VerboseVisitor<Multicut<MODEL> > VerboseVisitorType;
   //typedef visitors::EmptyVisitor<Multicut<MODEL> > EmptyVisitorType;
   //typedef visitors::TimingVisitor<Multicut<MODEL> > TimingVisitorType;



   typedef  std::unordered_map<Vi, LPIndexType> EdgeMapType;
   typedef  std::unordered_set<Vi> MYSET; 



   virtual ~Multicut();
   Multicut(const Model&, Options para=Options());


   void infer();
   template<class VisitorType> 
   void infer(VisitorType&);




   template<class OUT_ITER>
   void conf(OUT_ITER outIter) const;
   ValueType bound() const;
   ValueType value() const;
   ValueType calcBound(){ return 0; }

   template<class LPVariableIndexIterator, class CoefficientIterator>
   void addConstraint(LPVariableIndexIterator, LPVariableIndexIterator,
                        CoefficientIterator, const ValueType&, const ValueType&);
   std::vector<double> getEdgeLabeling() const; 
   std::vector<size_t> getSegmentation() const;

   template<class IT>
   size_t getLPIndex(IT a, IT b) { return neighbours[a][b]; };

   size_t inferenceState_;
   size_t constraintCounter_;
private:
   enum ProblemType {INVALID, MC, MWC};

   const Model& gm_; 
   ProblemType problemType_;
   Options options_;
   double constant_;
   double bound_;
   const double infinity_;

   DiscreteLabel   numberOfTerminals_;
   Vi   numberOfNodes_;
   LPIndexType numberOfTerminalEdges_;
   LPIndexType numberOfInternalEdges_;
   LPIndexType terminalOffset_;
   LPIndexType numberOfHigherOrderValues_;
   LPIndexType numberOfInterTerminalEdges_;

   std::vector<std::vector<size_t> >               workFlow_;
   std::vector<std::pair<Vi,Vi> >    edgeNodes_;

   /// For each variable it contains a map indexed by neighbord nodes giving the index to the LP-variable
   /// e.g. neighbours[a][b] = i means a has the neighbour b and the edge has the index i in the linear objective
   std::vector<EdgeMapType >   neighbours; 

   IloEnv         env_;
   IloModel       model_;
   IloNumVarArray x_;
   IloRangeArray  c_;
   IloObjective   obj_;
   IloNumArray    sol_;
   IloCplex       cplex_;

   bool           integerMode_;
   const double   EPS_;          //small number: for numerical issues constraints are still valid if the not up to EPS_


   void initCplex(); 

   size_t findCycleConstraints(IloRangeArray&, bool = true, bool = true);
   size_t findIntegerCycleConstraints(IloRangeArray&, bool = true);
   size_t findTerminalTriangleConstraints(IloRangeArray&);
   size_t findIntegerTerminalTriangleConstraints(IloRangeArray&, std::vector<DiscreteLabel>& conf);
   size_t findMultiTerminalConstraints(IloRangeArray&);
   size_t findOddWheelConstraints(IloRangeArray&);  
   size_t removeUnusedConstraints();            //TODO: implement
   size_t enforceIntegerConstraints();

   bool readWorkFlow(std::string);
  
   void partition(std::vector<DiscreteLabel>&, std::vector<std::list<size_t> >&, double = 0.5) const;
   ProblemType setProblemType();
   LPIndexType getNeighborhood(const LPIndexType, std::vector<EdgeMapType >&,std::vector<std::pair<Vi,Vi> >&, std::vector<HigherOrderTerm>&);

   template <class DOUBLEVECTOR>
   double shortestPath(const Vi, const Vi, const std::vector<EdgeMapType >&, const DOUBLEVECTOR&, std::vector<Vi>&, const double = std::numeric_limits<double>::infinity(), bool = true) const;

   void derandomizedRounding(std::vector<DiscreteLabel>&) const;
   void pseudoDerandomizedRounding(std::vector<DiscreteLabel>&, size_t = 1000) const;
   double derandomizedRoundingSubProcedure(std::vector<DiscreteLabel>&,const std::vector<DiscreteLabel>&, const double) const;

   //PROTOCOLATION 

   enum{
      Protocol_ID_Solve              = 0,
      Protocol_ID_AddConstraints     = 1,
      Protocol_ID_RemoveConstraints  = 2,
      Protocol_ID_IntegerConstraints = 3,
      Protocol_ID_CC                 = 4, 
      Protocol_ID_TTC                = 5,
      Protocol_ID_MTC                = 6,
      Protocol_ID_OWC                = 7,
      Protocol_ID_Unknown            = 8  
   };
   
   enum{
      Action_ID_RemoveConstraints  = 0,
      Action_ID_IntegerConstraints = 1,
      Action_ID_CC                 = 10, 
      Action_ID_CC_I               = 11, 
      Action_ID_CC_IFD             = 12, 
      Action_ID_CC_FD              = 13, 
      Action_ID_CC_B               = 14, 
      Action_ID_CC_FDB             = 15, 
      Action_ID_TTC                = 20,    
      Action_ID_TTC_I              = 21,   
      Action_ID_MTC                = 30,    
      Action_ID_OWC                = 40
   };    
   
   std::vector<std::vector<double> > protocolateTiming_;
   std::vector<std::vector<size_t> > protocolateConstraints_;
 
};
 
template<class MODEL>
typename Multicut<MODEL>::LPIndexType Multicut<MODEL>::getNeighborhood
(
   const LPIndexType numberOfTerminalEdges,
   std::vector<EdgeMapType >& neighbours,
   std::vector<std::pair<Vi,Vi> >& edgeNodes,
   std::vector<HigherOrderTerm>& higherOrderTerms
   )
{
   //Calculate Neighbourhood
   neighbours.resize(gm_.nVariables());
   LPIndexType numberOfInternalEdges=0;
   LPIndexType numberOfAdditionalInternalEdges=0;
   // Add edges that have to be included

   for(auto fi : gm_.factorIds()) {
      const auto factor = gm_[fi];
      const auto arity = factor->arity();
      if(arity==2) { // Second Order Potts
         const Vi u = factor->vi(1);
         const Vi v = factor->vi(0);
         if(neighbours[u].find(v)==neighbours[u].end()) {
            neighbours[u][v] = numberOfTerminalEdges+numberOfInternalEdges;
            neighbours[v][u] = numberOfTerminalEdges+numberOfInternalEdges;
            edgeNodes.push_back(std::pair<Vi,Vi>(v,u));     
            ++numberOfInternalEdges;
         }
      }
   }
   for(auto fi : gm_.factorIds()) {

      const auto factor = gm_[fi];
      const auto arity = factor->arity();
      ValueType beta;
      if(arity>2 && ! factor->valueTable()->isPotts(beta)){ // Generalized Potts
         higherOrderTerms.push_back(HigherOrderTerm(fi, false, 0));      
         for(size_t i=0; i<arity;++i) {
            for(size_t j=0; j<i;++j) {
              const Vi u = factor->vi(i);
              const Vi v = factor->vi(j);
               if(neighbours[u].find(v)==neighbours[u].end()) {
                  neighbours[u][v] = numberOfTerminalEdges+numberOfInternalEdges;
                  neighbours[v][u] = numberOfTerminalEdges+numberOfInternalEdges;
                  edgeNodes.push_back(std::pair<Vi,Vi>(v,u));     
                  ++numberOfInternalEdges;
                  ++numberOfAdditionalInternalEdges;
               }
            }
         }
      }
   }
   //Add for higher order potts term only neccesary edges 
   for(auto fi : gm_.factorIds()) {

      const auto factor = gm_[fi];
      const auto arity = factor->arity();
      ValueType beta;
      if(arity>2 && factor->valueTable()->isPotts(beta)) { //Higher order Potts
         higherOrderTerms.push_back(HigherOrderTerm(fi, true, 0));  
         std::vector<LPIndexType> lpIndexVector;
         //Find spanning tree vor the variables nb(f) using edges that already exist.
         std::vector<bool> variableInSpanningTree(gm_.nVariables(),true);
         for(size_t i=0; i<arity;++i) {
            variableInSpanningTree[factor->vi(i)]=false;
         }     
         size_t connection = 2; 
         // 1 = find a spanning tree and connect higher order auxilary variable to this
         // 2 = find a spanning subgraph including at least all eges in the subset and connect higher order auxilary variable to this
         if(connection==2){
            // ADD ALL 
            for(size_t i=0; i<arity;++i) {
               const Vi u = factor->vi(i);  
               for(typename EdgeMapType::const_iterator it=neighbours[u].begin() ; it != neighbours[u].end(); ++it){
                  const Vi v = (*it).first;
                  if(variableInSpanningTree[v] == false && u<v){
                    lpIndexVector.push_back((*it).second);
                  }
               }
            }
         }
         else if(connection==1){
            // ADD TREE
            for(size_t i=0; i<arity;++i) {
               const Vi u = factor->vi(i);  
               for(typename EdgeMapType::const_iterator it=neighbours[u].begin() ; it != neighbours[u].end(); ++it){
                  const Vi v = (*it).first;
                  if(variableInSpanningTree[v] == false){
                     variableInSpanningTree[v] = true;
                     lpIndexVector.push_back((*it).second);
                  }
               }
            }
         }
         else{
            INFERNO_ASSERT(false);
         }
         higherOrderTerms.back().lpIndices_=lpIndexVector;

         // Check if edges need to be added to have a spanning subgraph
         //TODO 
      }
   }
   //std::cout << "Additional Edges: "<<numberOfAdditionalInternalEdges<<std::endl;
   return numberOfInternalEdges;
}

template<class MODEL>
Multicut<MODEL>::~Multicut() {
   env_.end();
}

template<class MODEL>
Multicut<MODEL>::Multicut
(
   const Model& gm,
   Options para
   ) : gm_(gm), options_(para) , bound_(-std::numeric_limits<double>::infinity()), infinity_(1e8), integerMode_(false),
       EPS_(1e-7)
{
   if(options_.reductionMode_<0 ||options_.reductionMode_>3) {
      throw RuntimeError("Reduction Mode has to be 1, 2 or 3!");
   } 

   //Set Problem Type
   setProblemType();
   if(problemType_ == INVALID)
      throw RuntimeError("Invalid Model for Multicut-Solver! Solver requires a generalized potts model!");

   //Calculate Neighbourhood 
   std::vector<double> valuesHigherOrder;
   std::vector<HigherOrderTerm> higherOrderTerms;
   numberOfInternalEdges_ = getNeighborhood(numberOfTerminalEdges_, neighbours, edgeNodes_ ,higherOrderTerms);
   numberOfNodes_         = gm_.nVariables(); 


   // Display some info
   if(options_.verbose_ == true) {
      std::cout << "** Multicut Info" << std::endl;
      if(problemType_==MC)
         std::cout << "  problemType_:            Multicut"  << std::endl; 
      if(problemType_==MWC)
         std::cout << "  problemType_:            Multiway Cut"  << std::endl;
      std::cout << "  numberOfInternalEdges_:  " << numberOfInternalEdges_ << std::endl;
      std::cout << "  numberOfNodes_:          " << numberOfNodes_ << std::endl;
      std::cout << "  allowCutsWithin_:        ";
      if(problemType_==MWC && options_.allowCutsWithin_.size() ==  numberOfTerminals_){
         for(size_t i=0; i<options_.allowCutsWithin_.size(); ++i)
            if(options_.allowCutsWithin_[i]) std::cout<<i<<" ";
      }
      else{
         std::cout<<"none";   
      }    
      std::cout << std::endl;
      std::cout << "  higherOrderTerms.size(): " << higherOrderTerms.size() << std::endl;
      std::cout << "  numberOfTerminals_:      " << numberOfTerminals_ << std::endl;
   }      
     
   //Build Objective Value 
   constant_=0;
   size_t valueSize;
   if(numberOfTerminals_==0) valueSize = numberOfInternalEdges_;
   else                      valueSize = numberOfTerminalEdges_+numberOfInternalEdges_+numberOfInterTerminalEdges_;
   std::vector<double> values (valueSize,0); 
   
   INFERNO_CHECK(gm_.denseVariableIds(),"only with dense var ids atm");
   INFERNO_CHECK_OP(gm_.minVarId(),==,0,"var must start at zero atm")


   //for(size_t f=0; f<gm_.nFactors(); ++f) {
   for(auto fi : gm_.factorIds()) {

      const auto factor = gm_[fi];
      const auto arity = factor->arity();

      if(arity == 0) {
         constant_ +=  factor.eval(DiscreteLabel(0));
      }
      else if(arity == 1) {
         const Vi node = factor->vi(0);
         for(DiscreteLabel i=0; i<gm_.nLabels(node); ++i) {
            for(DiscreteLabel j=0; j<gm_.nLabels(node); ++j) {
               if(i==j) values[node*numberOfTerminals_+i] += (1.0/(numberOfTerminals_-1)-1) * factor->eval1(j);
               else     values[node*numberOfTerminals_+i] += (1.0/(numberOfTerminals_-1))   * factor->eval1(j);
            }
         }
      }
      else if(arity == 2) {

         const Vi node0 = factor->vi(0);
         const Vi node1 = factor->vi(1);

         if(factor->shape(0)==2 && factor->shape(1)==2){

            const Vi node0 = factor->vi(0);
            const Vi node1 = factor->vi(1);

            DiscreteLabel cc[] = {0,0}; ValueType a = factor->eval(cc);

            cc[0]=1;cc[1]=1;        ValueType b = factor->eval(cc);
            cc[0]=0;cc[1]=1;        ValueType c = factor->eval(cc);
            cc[0]=1;cc[1]=0;        ValueType d = factor->eval(cc);

            values[neighbours[node0][node1]] += ((c+d-a-a) - (b-a))/2.0; 
            values[node0*numberOfTerminals_+0] += ((b-a)-(-d+c))/2.0;
            values[node1*numberOfTerminals_+0] += ((b-a)-( d-c))/2.0;
            constant_ += a;

         }
         else{
            const DiscreteLabel cc0[] = {0,0};
            const DiscreteLabel cc1[] = {0,1};
            values[neighbours[node0][node1]] += factor->eval(cc1) - factor->eval(cc0); 
            constant_ += factor->eval(cc0);
         }
      }
   }
   for(size_t h=0; h<higherOrderTerms.size();++h){

      const Vi fi = higherOrderTerms[h].factorID_; 
      const auto factor = gm_[fi];
      const auto arity = factor->arity();


      if(higherOrderTerms[h].potts_) {

         higherOrderTerms[h].valueIndex_= valuesHigherOrder.size();
         INFERNO_ASSERT(arity > 2);
         inferno::SmallVector<DiscreteLabel> cc0(arity,0);
         inferno::SmallVector<DiscreteLabel> cc1(arity,0); 
         cc1[0] = 1;
         valuesHigherOrder.push_back(factor->eval(cc1.data()) - factor->eval(cc0.data()) ); 
         constant_ += factor->eval(cc0.data());
      }
      else{

         if(arity == 3) {
            DiscreteLabel i[] = {0, 1, 2 }; 
            valuesHigherOrder.push_back(factor->eval(i)); 
            i[0]=0; i[1]=0; i[2]=1;
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=1; i[2]=0;
            valuesHigherOrder.push_back(factor->eval(i)); 
            i[0]=1; i[1]=0; i[2]=0;
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=0; i[2]=0;
            valuesHigherOrder.push_back(factor->eval(i));
         }
         else if(arity == 4) {
            DiscreteLabel i[] = {0, 1, 2, 3 };//0
            if(numberOfTerminals_>=4){
               valuesHigherOrder.push_back(factor->eval(i));
            }else{
               valuesHigherOrder.push_back(0.0);
            }
            if(numberOfTerminals_>=3){
               i[0]=0; i[1]=0; i[2]=1; i[3] = 2;//1
               valuesHigherOrder.push_back(factor->eval(i));
               i[0]=0; i[1]=1; i[2]=0; i[3] = 2;//2
               valuesHigherOrder.push_back(factor->eval(i));
               i[0]=0; i[1]=1; i[2]=1; i[3] = 2;//4
               valuesHigherOrder.push_back(factor->eval(i));
            }else{
               valuesHigherOrder.push_back(0.0);
               valuesHigherOrder.push_back(0.0);
               valuesHigherOrder.push_back(0.0);
            }
            i[0]=0; i[1]=0; i[2]=0; i[3] = 1;//7
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=1; i[2]=2; i[3] = 0;//8
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=1; i[2]=1; i[3] = 0;//12
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=1; i[1]=0; i[2]=2; i[3] = 0;//16
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=1; i[1]=0; i[2]=1; i[3] = 0;//18
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=0; i[2]=1; i[3] = 0;//25
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=1; i[1]=2; i[2]=0; i[3] = 0;//32
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=1; i[1]=1; i[2]=0; i[3] = 0;//33
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=1; i[2]=0; i[3] = 0;//42
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=1; i[1]=0; i[2]=0; i[3] = 0;//52
            valuesHigherOrder.push_back(factor->eval(i));
            i[0]=0; i[1]=0; i[2]=0; i[3] = 0;//63
            valuesHigherOrder.push_back(factor->eval(i));
         }
         else{
            throw RuntimeError("Generalized Potts Terms of an order larger than 4 a currently not supported. If U really need them let us know!");
         }
      }
   }

   //count auxilary variables
   numberOfHigherOrderValues_ = valuesHigherOrder.size();

   // build LP 
   //std::cout << "Higher order auxilary variables " << numberOfHigherOrderValues_ << std::endl;
   //std::cout << "TerminalEdges " << numberOfTerminalEdges_ << std::endl;
   INFERNO_ASSERT( numberOfTerminalEdges_ == gm_.nVariables()*numberOfTerminals_ );
   //std::cout << "InternalEdges " << numberOfInternalEdges_ << std::endl;

   INFERNO_ASSERT(values.size() == numberOfTerminalEdges_+numberOfInternalEdges_+numberOfInterTerminalEdges_);
   IloInt N = values.size() + numberOfHigherOrderValues_;
   model_ = IloModel(env_);
   x_     = IloNumVarArray(env_);
   c_     = IloRangeArray(env_);
   obj_   = IloMinimize(env_);
   sol_   = IloNumArray(env_,N);

   // set variables and objective
   x_.add(IloNumVarArray(env_, N, 0, 1, ILOFLOAT));

   IloNumArray    obj(env_,N);
   for (size_t i=0; i< values.size();++i) {
      if(values[i]==0)
         obj[i] = 0.0;//1e-50; //for numerical reasons
      else
         obj[i] = values[i];
   }
   {
      size_t count =0;
      for (size_t i=0; i<valuesHigherOrder.size();++i) {
         obj[values.size()+count++] = valuesHigherOrder[i];
      }
      INFERNO_ASSERT(count == numberOfHigherOrderValues_);
   }
   obj_.setLinearCoefs(x_,obj);
 
   // set constraints 
   size_t constraintCounter = 0;
   // multiway cut constraints
   if(problemType_ == MWC) {
      // From each internal-node only one terminal-edge should be 0
      for(Vi var=0; var<gm_.nVariables(); ++var) {
         c_.add(IloRange(env_, numberOfTerminals_-1, numberOfTerminals_-1));
         for(DiscreteLabel i=0; i<gm_.nLabels(var); ++i) {
            c_[constraintCounter].setLinearCoef(x_[var*numberOfTerminals_+i],1);
         }
         ++constraintCounter;
      }
      // Inter-terminal-edges have to be 1
      for(size_t i=0; i<(size_t)(numberOfTerminals_*(numberOfTerminals_-1)/2); ++i) {
         c_.add(IloRange(env_, 1, 1));
         c_[constraintCounter].setLinearCoef(x_[numberOfTerminalEdges_+numberOfInternalEdges_+i],1);
         ++constraintCounter;
      }
   }

   
   // higher order constraints
   size_t count = 0;
   for(size_t i=0; i<higherOrderTerms.size(); ++i) {
      const auto factorID = higherOrderTerms[i].factorID_;
      const auto factor = gm_[factorID];
      size_t numVar   = factor->arity();

      INFERNO_ASSERT(numVar>2);

      if(higherOrderTerms[i].potts_) {
         double b = higherOrderTerms[i].lpIndices_.size();
         

         // Add only one constraint is sufficient with {0,1} constraints
         // ------------------------------------------------------------
         // ** -|E|+1 <= -|E|*y_H+\sum_{e\in H} y_e <= 0 
         if(options_.reductionMode_ % 2 == 1){
            c_.add(IloRange(env_, -b+1 , 0));
            for(size_t i1=0; i1<higherOrderTerms[i].lpIndices_.size();++i1) {
               const LPIndexType edgeID = higherOrderTerms[i].lpIndices_[i1]; 
               c_[constraintCounter].setLinearCoef(x_[edgeID],1);
            }
            c_[constraintCounter].setLinearCoef(x_[values.size()+count],-b);
            constraintCounter += 1;
         }
         // In general this additional contraints and more local constraints leeds to tighter relaxations
         // ---------------------------------------------------------------------------------------------
         if(options_.reductionMode_ % 4 >=2){ 
            // ** y_H <= sum_{e \in H} y_e
            c_.add(IloRange(env_, -2.0*b, 0));
            for(size_t i1=0; i1<higherOrderTerms[i].lpIndices_.size();++i1) {
               const LPIndexType edgeID = higherOrderTerms[i].lpIndices_[i1]; 
               c_[constraintCounter].setLinearCoef(x_[edgeID],-1);
            }
            c_[constraintCounter].setLinearCoef(x_[values.size()+count],1);
            constraintCounter += 1;
         
            // ** forall e \in H : y_H>=y_e
            for(size_t i1=0; i1<higherOrderTerms[i].lpIndices_.size();++i1) {
               const LPIndexType edgeID = higherOrderTerms[i].lpIndices_[i1]; 
               c_.add(IloRange(env_, 0 , 1));
               c_[constraintCounter].setLinearCoef(x_[edgeID],-1);
               c_[constraintCounter].setLinearCoef(x_[values.size()+count],1); 
               constraintCounter += 1;
            }        
         }
         count++;
      }else{
         if(numVar==3) {

            const Vi vis[3] = {factor->vi(0), factor->vi(1), factor->vi(2)};


            INFERNO_ASSERT(higherOrderTerms[i].valueIndex_<=valuesHigherOrder.size());

            LPIndexType edgeIDs[3];
            edgeIDs[0] = neighbours[vis[0]][ vis[1]];
            edgeIDs[1] = neighbours[vis[0]][ vis[2]];
            edgeIDs[2] = neighbours[vis[1]][ vis[2]];
               
            const unsigned int P[] = {0,1,2,4,7,8,12,16,18,25,32,33,42,52,63};
            double c[3];  

            c_.add(IloRange(env_, 1, 1));
            size_t lvc=0;
            for(size_t p=0; p<5; p++){
               if(true || valuesHigherOrder[higherOrderTerms[i].valueIndex_+p]!=0){   
                  c_[constraintCounter].setLinearCoef(x_[values.size()+count+lvc],1);
                  ++lvc;
               }
            }
            ++constraintCounter;  

            for(size_t p=0; p<5; p++){
               if(true || valuesHigherOrder[higherOrderTerms[i].valueIndex_+p]!=0){
                  double ub = 2.0;
                  double lb = 0.0;
                  unsigned int mask = 1;
                  for(size_t n=0; n<3; n++){
                     if(P[p] & mask){
                        c[n] = -1.0;
                        ub--;
                        lb--; 
                     }
                     else{
                        c[n] = 1.0; 
                     }
                     mask = mask << 1;
                  }
                  c_.add(IloRange(env_, lb, ub));
                  for(size_t n=0; n<3; n++){
                     c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],c[n]);
                  }
                  c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
                  ++constraintCounter;  
               
                  for(size_t n=0; n<3; n++){
                     if(c[n]>0){
                        c_.add(IloRange(env_, 0, 1));
                        c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],1);
                        c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
                        ++constraintCounter;     
                     }else{
                        c_.add(IloRange(env_, -1, 0));
                        c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],-1);
                        c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
                        ++constraintCounter;     
                     }     
                  }
                  ++count;
               }
            }
         }
         else if(numVar==4) {                  
            INFERNO_ASSERT(higherOrderTerms[i].valueIndex_<=valuesHigherOrder.size());
            LPIndexType edgeIDs[6];

            const Vi vis[4] = {factor->vi(0), factor->vi(1), factor->vi(2), factor->vi(3)};


            edgeIDs[0] = neighbours[ vis[0] ][ vis[1] ];
            edgeIDs[1] = neighbours[ vis[0] ][ vis[2] ];
            edgeIDs[2] = neighbours[ vis[1] ][ vis[2] ];
            edgeIDs[3] = neighbours[ vis[0] ][ vis[3] ];
            edgeIDs[4] = neighbours[ vis[1] ][ vis[3] ];
            edgeIDs[5] = neighbours[ vis[2] ][ vis[3] ];
          
               
            const unsigned int P[] = {0,1,2,4,7,8,12,16,18,25,32,33,42,52,63};
            double c[6];

            c_.add(IloRange(env_, 1, 1));
            size_t lvc=0;
            for(size_t p=0; p<15; p++){
               if(true ||valuesHigherOrder[higherOrderTerms[i].valueIndex_+p]!=0){   
                  c_[constraintCounter].setLinearCoef(x_[values.size()+count+lvc],1);
                  ++lvc;
               }
            }
            ++constraintCounter;  


            for(size_t p=0; p<15; p++){
               double ub = 5.0;
               double lb = 0.0;
               unsigned int mask = 1;
               for(size_t n=0; n<6; n++){
                  if(P[p] & mask){
                     c[n] = -1.0;
                     ub--;
                     lb--; 
                  }
                  else{
                     c[n] = 1.0; 
                  }
                  mask = mask << 1;
               }
               c_.add(IloRange(env_, lb, ub));
               for(size_t n=0; n<6; n++){
                  c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],c[n]);
               }
               c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
               ++constraintCounter;  
               
               for(size_t n=0; n<6; n++){
                  if(c[n]>0){
                     c_.add(IloRange(env_, 0, 1));
                     c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],1);
                     c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
                     ++constraintCounter;     
                  }else{
                     c_.add(IloRange(env_, -1, 0));
                     c_[constraintCounter].setLinearCoef(x_[edgeIDs[n]],-1);
                     c_[constraintCounter].setLinearCoef(x_[values.size()+count],-1);
                     ++constraintCounter;     
                  }     
               }
               ++count;
            }  
         }
         else{
            INFERNO_ASSERT(false);
         }
      }
   } 


   model_.add(obj_);
   if(constraintCounter>0) {
      model_.add(c_);
   }

   // initialize solver
   cplex_ = IloCplex(model_);

}

template<class MODEL>
typename Multicut<MODEL>::ProblemType Multicut<MODEL>::setProblemType() {
   problemType_ = MC;

   //for(size_t f=0; f<gm_.nFactors();++f) {
   for(auto fi : gm_.factorIds()) {

      const auto factor = gm_[fi];
      const auto arity = factor->arity();

      if(arity==1) {
         problemType_ = MWC;
      }
      if(arity>1) {
         for(size_t i=0; i<arity;++i) {
            if(factor->shape(i)<gm_.nVariables()) {
               problemType_ = MWC;
            }
         }
      }
      if(arity==2 && factor->shape(0)==2 && factor->shape(1)==2){
         problemType_ = MWC; //OK - can be reparmetrized
      }
      else if(arity>1 && ! factor->valueTable()->isGeneralizedPotts()) {
         problemType_ = INVALID;
         break;
      }
   } 
 
   // set member variables
   if(problemType_ == MWC) {
      numberOfTerminals_ = gm_.nLabels(0); 
      numberOfInterTerminalEdges_ = (numberOfTerminals_*(numberOfTerminals_-1))/2; 
      numberOfTerminalEdges_ = 0;
      for(Vi i=0; i<gm_.nVariables(); ++i) {
         for(DiscreteLabel j=0; j<gm_.nLabels(i); ++j) {
            ++numberOfTerminalEdges_;
         }
      } 
   }
   else{
      numberOfTerminalEdges_ = 0;
      numberOfTerminals_     = 0;
      numberOfInterTerminalEdges_ = 0;
   } 

   return problemType_;
}

//**********************************************
//**
//** Functions that find violated Constraints
//**
//**********************************************

template<class MODEL>
size_t Multicut<MODEL>::removeUnusedConstraints()
{ 
   std::cout << "Not Implemented " <<std::endl ; 
   return 0;
}



template<class MODEL>
size_t Multicut<MODEL>::enforceIntegerConstraints()
{
   bool enforceIntegerConstraintsOnTerminalEdges = true;
   bool enforceIntegerConstraintsOnInternalEdges = false;

   if(numberOfTerminalEdges_ == 0 ||  options_.allowCutsWithin_.size() == numberOfTerminals_) {
      enforceIntegerConstraintsOnInternalEdges = true;
   }

   size_t N = 0;
   if (enforceIntegerConstraintsOnTerminalEdges)
      N += numberOfTerminalEdges_;
   if (enforceIntegerConstraintsOnInternalEdges)
      N += numberOfInternalEdges_;

   for(size_t i=0; i<N; ++i)
      model_.add(IloConversion(env_, x_[i], ILOBOOL));

   for(size_t i=0; i<numberOfHigherOrderValues_; ++i)
      model_.add(IloConversion(env_, x_[numberOfTerminalEdges_+numberOfInternalEdges_+numberOfInterTerminalEdges_+i], ILOBOOL));

   integerMode_ = true;

   return N+numberOfHigherOrderValues_;
}

/// Find violated terminal triangle constrains
///  * Only for Multi Way Cut
///  * can be used for fractional and integer case
///  * check |E_I|*3*|L| constrains
///
///  (a,b)-(a,l)-(b,l) must be consistent for all l in L
template<class MODEL>
size_t Multicut<MODEL>::findTerminalTriangleConstraints(IloRangeArray& constraint)
{
   INFERNO_ASSERT(problemType_ == MWC);
   if(!(problemType_ == MWC)) return 0;
   size_t tempConstrainCounter = constraintCounter_;

   size_t u,v;
   if(options_.allowCutsWithin_.size()!=numberOfTerminals_){
      for(size_t i=0; i<numberOfInternalEdges_;++i) {
         u = edgeNodes_[i].first;//[0];
         v = edgeNodes_[i].second;//[1];
         for(size_t l=0; l<numberOfTerminals_;++l) {
            if(-sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+l]+sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],-1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+l]+sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+l]-sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],-1);
               ++constraintCounter_;
            }
         }
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
   }
   else{
      for(size_t i=0; i<numberOfInternalEdges_;++i) {
         u = edgeNodes_[i].first;//[0];
         v = edgeNodes_[i].second;//[1];
         for(size_t l=0; l<numberOfTerminals_;++l) {
            if(options_.allowCutsWithin_[l])
               continue;
            if(-sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+l]+sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],-1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+l]+sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+l]-sol_[v*numberOfTerminals_+l]<-EPS_) {
               constraint.add(IloRange(env_, 0 , 2));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+l],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+l],-1);
               ++constraintCounter_;
            }
         }
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
   }
   return constraintCounter_-tempConstrainCounter;
}

/// Find violated multi terminal constrains
///  * Only for Multi Way Cut
///  * can be used for fractional and integer case
///  * check |E_I| constrains
///
///  x(u,v) >= \sum_{s\in S \subset T} x(u,t)-x(v,t)
template<class MODEL>
size_t Multicut<MODEL>::findMultiTerminalConstraints(IloRangeArray& constraint)
{
   INFERNO_ASSERT(problemType_ == MWC);
   if(!(problemType_ == MWC)) return 0;
   size_t tempConstrainCounter = constraintCounter_;
   if(options_.allowCutsWithin_.size()==numberOfTerminals_){
      for(size_t i=0; i<options_.allowCutsWithin_.size();++i)
         if(options_.allowCutsWithin_[i])
            return 0; //Can not gurantee that Multi Terminal Constraints are valid cuts
   }

   size_t u,v;  
   for(size_t i=0; i<numberOfInternalEdges_;++i) {
      u = edgeNodes_[i].first;//[0];
      v = edgeNodes_[i].second;//[1];
      std::vector<size_t> terminals1;
      std::vector<size_t> terminals2;
      double sum1 = 0;
      double sum2 = 0;
      for(size_t l=0; l<numberOfTerminals_;++l) {
         if(sol_[u*numberOfTerminals_+l]-sol_[v*numberOfTerminals_+l] > EPS_) {
            terminals1.push_back(l);
            sum1 += sol_[u*numberOfTerminals_+l]-sol_[v*numberOfTerminals_+l];
         }
         if(sol_[v*numberOfTerminals_+l]-sol_[u*numberOfTerminals_+l] > EPS_) {
            terminals2.push_back(l);
            sum2 +=sol_[v*numberOfTerminals_+l]-sol_[u*numberOfTerminals_+l];
         }
      }
      if(sol_[numberOfTerminalEdges_+i]-sum1<-EPS_) {
         constraint.add(IloRange(env_, 0 , 200000));
         constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
         for(size_t k=0; k<terminals1.size(); ++k) {
            constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+terminals1[k]],-1);
            constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+terminals1[k]],1);
         }
         ++constraintCounter_;
      }
      if(sol_[numberOfTerminalEdges_+i]-sum2<-EPS_) {
         constraint.add(IloRange(env_, 0 , 200000));
         constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
         for(size_t k=0; k<terminals2.size(); ++k) {
            constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+terminals2[k]],1);
            constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+terminals2[k]],-1);
         }
         ++constraintCounter_;
      } 
      if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
         break;
   }      
   return constraintCounter_-tempConstrainCounter;
}

/// Find violated integer terminal triangle constrains
///  * Only for Multi Way Cut
///  * can be used for integer case only
///  * check |E_I|*3*|L| constrains
///
///  (a,b)-(a,l)-(b,l) must be consistent for all l in L
template<class MODEL>
size_t Multicut<MODEL>::findIntegerTerminalTriangleConstraints(IloRangeArray& constraint, std::vector<DiscreteLabel>& conf)
{ 
   INFERNO_ASSERT(integerMode_);
   INFERNO_ASSERT(problemType_ == MWC);
   if(!(problemType_ == MWC)) return 0;
   size_t tempConstrainCounter = constraintCounter_;

   size_t u,v;
   if(options_.allowCutsWithin_.size()!=numberOfTerminals_){
      for(size_t i=0; i<numberOfInternalEdges_;++i) {
         u = edgeNodes_[i].first;//[0];
         v = edgeNodes_[i].second;//[1];
         if(sol_[numberOfTerminalEdges_+i]<EPS_ && (conf[u]!=conf[v]) ) {
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[u]]+sol_[v*numberOfTerminals_+conf[u]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[u]],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[u]]+sol_[v*numberOfTerminals_+conf[u]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[u]],-1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[v]]+sol_[v*numberOfTerminals_+conf[v]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[v]],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+conf[v]]-sol_[v*numberOfTerminals_+conf[v]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[v]],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],-1);
               ++constraintCounter_;
            }
         }
         if(sol_[numberOfTerminalEdges_+i]>1-EPS_ && (conf[u]==conf[v]) ) {
            constraint.add(IloRange(env_, 0 , 10));
            constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],-1);
            constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],1);
            constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],1);
            ++constraintCounter_;
         }
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
   }
   else{ // Allow Cuts within classes
      for(size_t i=0; i<numberOfInternalEdges_;++i) {
         u = edgeNodes_[i].first;//[0];
         v = edgeNodes_[i].second;//[1];
         if(sol_[numberOfTerminalEdges_+i]<EPS_ && (conf[u]!=conf[v]) ) {
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[u]]+sol_[v*numberOfTerminals_+conf[u]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[u]],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[u]]+sol_[v*numberOfTerminals_+conf[u]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[u]],-1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]-sol_[u*numberOfTerminals_+conf[v]]+sol_[v*numberOfTerminals_+conf[v]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[v]],-1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],1);
               ++constraintCounter_;
            }
            if(sol_[numberOfTerminalEdges_+i]+sol_[u*numberOfTerminals_+conf[v]]-sol_[v*numberOfTerminals_+conf[v]]<=0) {
               constraint.add(IloRange(env_, 0 , 10));
               constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],1);
               constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[v]],1);
               constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],-1);
               ++constraintCounter_;
            }
         }
         if(sol_[numberOfTerminalEdges_+i]>1-EPS_ && (conf[u]==conf[v])  && !options_.allowCutsWithin_[conf[u]] ) {
            constraint.add(IloRange(env_, 0 , 10));
            constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],-1);
            constraint[constraintCounter_].setLinearCoef(x_[u*numberOfTerminals_+conf[u]],1);
            constraint[constraintCounter_].setLinearCoef(x_[v*numberOfTerminals_+conf[v]],1);
            ++constraintCounter_;
         }
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
   }

   return constraintCounter_-tempConstrainCounter;
}

/// Find violate cycle constrains
///  * add at most |E_I| constrains
///
///  
template<class MODEL>
size_t Multicut<MODEL>::findCycleConstraints(
   IloRangeArray& constraint,
   bool addOnlyFacetDefiningConstraints,
   bool usePreBounding
)
{ 
   std::vector<DiscreteLabel> partit;
   std::vector<std::list<size_t> > neighbours0;

   size_t tempConstrainCounter = constraintCounter_;
 
   if(usePreBounding){
      partition(partit,neighbours0,1-EPS_);
   }
  
   std::map<std::pair<Vi,Vi>,size_t> counter;
   for(size_t i=0; i<numberOfInternalEdges_;++i) {
      
      Vi u = edgeNodes_[i].first;//[0];
      Vi v = edgeNodes_[i].second;//[1]; 
      
      if(usePreBounding && partit[u] != partit[v])
         continue;

      INFERNO_ASSERT(i+numberOfTerminalEdges_ == neighbours[u][v]);
      INFERNO_ASSERT(i+numberOfTerminalEdges_ == neighbours[v][u]);
      
      if(sol_[numberOfTerminalEdges_+i]>EPS_){
         //search for cycle
         std::vector<Vi> path;
         const double pathLength = shortestPath(u,v,neighbours,sol_,path,sol_[numberOfTerminalEdges_+i],addOnlyFacetDefiningConstraints);
         if(sol_[numberOfTerminalEdges_+i]-EPS_>pathLength){
            INFERNO_ASSERT(path.size()>2);
            constraint.add(IloRange(env_, 0  , 1000000000)); 
            //negative zero seemed to be required for numerical reasons, even CPlex handel this by its own, too.
            constraint[constraintCounter_].setLinearCoef(x_[numberOfTerminalEdges_+i],-1);
            for(size_t n=0;n<path.size()-1;++n){
               constraint[constraintCounter_].setLinearCoef(x_[neighbours[path[n]][path[n+1]]],1);
            }
            ++constraintCounter_; 
         }
      } 
      if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
         break;
   }
   return constraintCounter_-tempConstrainCounter;
}

template<class MODEL>
size_t Multicut<MODEL>::findOddWheelConstraints(IloRangeArray& constraints){
   size_t tempConstrainCounter = constraintCounter_;
   std::vector<Vi> var2node(gm_.nVariables(),std::numeric_limits<Vi>::max());
   for(size_t center=0; center<gm_.nVariables();++center){
      var2node.assign(gm_.nVariables(),std::numeric_limits<Vi>::max());
      size_t N = neighbours[center].size();
      std::vector<Vi> node2var(N);
      std::vector<EdgeMapType> E(2*N);
      std::vector<double>     w;
      typename EdgeMapType::const_iterator it;
      size_t id=0;
      for(it=neighbours[center].begin() ; it != neighbours[center].end(); ++it) {
         Vi var = (*it).first;
         node2var[id]  = var;
         var2node[var] = id++;
      } 
     
      for(it=neighbours[center].begin() ; it != neighbours[center].end(); ++it) { 
         const Vi var1 = (*it).first;
         const LPIndexType u = var2node[var1];   
         typename EdgeMapType::const_iterator it2;
         for(it2=neighbours[var1].begin() ; it2 != neighbours[var1].end(); ++it2) {
            const Vi var2 = (*it2).first; 
            const LPIndexType v = var2node[var2];   
            if( v !=  std::numeric_limits<Vi>::max()){
               if(u<v){
                  E[2*u][2*v+1]=w.size();
                  E[2*v+1][2*u]=w.size(); 
                  E[2*u+1][2*v]=w.size();
                  E[2*v][2*u+1]=w.size();
                  double weight = 0.5-sol_[neighbours[var1][var2]]+0.5*(sol_[neighbours[center][var1]]+sol_[neighbours[center][var2]]); 
                  //INFERNO_ASSERT(weight>-1e-7);
                  if(weight<0) weight=0;
                  w.push_back(weight);
                 
               }
            }
         }
      }
     
      //Search for odd wheels
      for(size_t n=0; n<N;++n) {
         std::vector<Vi> path;
         const double pathLength = shortestPath(2*n,2*n+1,E,w,path,1e22,false);
         if(pathLength<0.5-EPS_*path.size()){// && (path.size())>3){
            INFERNO_ASSERT((path.size())>3);
            INFERNO_ASSERT(((path.size())/2)*2 == path.size() );

            constraints.add(IloRange(env_, -100000 , (path.size()-2)/2 ));
            for(size_t k=0;k<path.size()-1;++k){
               constraints[constraintCounter_].setLinearCoef(x_[neighbours[center][node2var[path[k]/2]]],-1);
            }
            for(size_t k=0;k<path.size()-1;++k){
               const Vi u= node2var[path[k]/2];
               const Vi v= node2var[path[k+1]/2];
               constraints[constraintCounter_].setLinearCoef(x_[neighbours[u][v]],1);
            }
            ++constraintCounter_; 
         } 
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
 
      //Reset var2node
      for(it=neighbours[center].begin() ; it != neighbours[center].end(); ++it) {
         var2node[(*it).first] = std::numeric_limits<Vi>::max();
      }
    
   }
   
   return constraintCounter_-tempConstrainCounter;
}  
  
/// Find violate integer cycle constrains
///  * can be used for integer case only
///  * add at most |E_I| constrains
///
///  
template<class MODEL>
size_t Multicut<MODEL>::findIntegerCycleConstraints(
   IloRangeArray& constraint,
   bool addFacetDefiningConstraintsOnly
)
{
   INFERNO_ASSERT(integerMode_);
   std::vector<DiscreteLabel> partit(numberOfNodes_,0);
   std::vector<std::list<size_t> > neighbours0;
   partition(partit,neighbours0);
   size_t tempConstrainCounter = constraintCounter_;
  
   //Find Violated Cycles inside a Partition
   size_t u,v;
   for(size_t i=0; i<numberOfInternalEdges_;++i) {
      u = edgeNodes_[i].first;//[0];
      v = edgeNodes_[i].second;//[1];
      INFERNO_ASSERT(partit[u] >= 0);
      if(sol_[numberOfTerminalEdges_+i]>=EPS_ && partit[u] == partit[v]) {
         //find shortest path from u to v by BFS
         std::queue<size_t> nodeList;
         std::vector<size_t> path(numberOfNodes_,std::numeric_limits<size_t>::max());
         size_t n = u;
         while(n!=v) {
            std::list<size_t>::iterator it;
            for(it=neighbours0[n].begin() ; it != neighbours0[n].end(); ++it) {
               if(path[*it]==std::numeric_limits<size_t>::max()) {
                  //Check if this path is chordless 
                  if(addFacetDefiningConstraintsOnly) {
                     bool isCordless = true;
                     size_t s = n;
                     const size_t c = *it;
                     while(s!=u){
                        s = path[s];
                        if(s==u && c==v) continue;
                        if(neighbours[c].find(s)!=neighbours[c].end()) {
                           isCordless = false;
                           break;
                        } 
                     }
                     if(isCordless){
                        path[*it]=n;
                        nodeList.push(*it);
                     }
                  }
                  else{
                     path[*it]=n;
                     nodeList.push(*it);
                  }
               }
            }
            if(nodeList.size()==0)
               break;
            n = nodeList.front(); nodeList.pop();
         }
         if(path[v] != std::numeric_limits<size_t>::max()){
            if(!integerMode_){//check if it is realy violated
               double w=0;
               while(n!=u) {
                  w += sol_[neighbours[n][path[n]]];
                  n=path[n];
               }
               if(sol_[neighbours[u][v]]-EPS_<w)//constraint is not violated
                  continue;
            }

            constraint.add(IloRange(env_, 0 , 1000000000));
            constraint[constraintCounter_].setLinearCoef(x_[neighbours[u][v]],-1);
            while(n!=u) {
               constraint[constraintCounter_].setLinearCoef(x_[neighbours[n][path[n]]],1);
               n=path[n];
            }
            ++constraintCounter_;
         } 
         if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
            break;
      }
      if(constraintCounter_-tempConstrainCounter >= options_.maximalNumberOfConstraintsPerRound_)
         break;
   }
   return constraintCounter_-tempConstrainCounter;
}
//************************************************

template<class MODEL>
void
Multicut<MODEL>::infer()
{
   float mcv;
   return infer(mcv);
}

template<class MODEL>
void
Multicut<MODEL>::initCplex()
{

   cplex_.setParam(IloCplex::Threads, options_.numThreads_);
   cplex_.setParam(IloCplex::CutUp, options_.cutUp_);
   cplex_.setParam(IloCplex::MIPDisplay,0);
   cplex_.setParam(IloCplex::BarDisplay,0);
   cplex_.setParam(IloCplex::NetDisplay,0);
   cplex_.setParam(IloCplex::SiftDisplay,0);
   cplex_.setParam(IloCplex::SimDisplay,0);

   cplex_.setParam(IloCplex::EpOpt,1e-9);
   cplex_.setParam(IloCplex::EpRHS,1e-8); //setting this to 1e-9 seemed to be to agressive!
   cplex_.setParam(IloCplex::EpInt,0);
   cplex_.setParam(IloCplex::EpAGap,0);
   cplex_.setParam(IloCplex::EpGap,0);

   if(options_.verbose_ == true && options_.verboseCPLEX_) {
      cplex_.setParam(IloCplex::MIPDisplay,2);
      cplex_.setParam(IloCplex::BarDisplay,1);
      cplex_.setParam(IloCplex::NetDisplay,1);
      cplex_.setParam(IloCplex::SiftDisplay,1);
      cplex_.setParam(IloCplex::SimDisplay,1);
   }
}


template<class MODEL>
template<class VisitorType>
void
Multicut<MODEL>::infer(VisitorType& mcv)
{
   std::vector<DiscreteLabel> config(gm_.nVariables());
   initCplex();
   //cplex_.setParam(IloCplex::RootAlg, IloCplex::Primal);
    
   if(problemType_ == INVALID){ 
      throw RuntimeError("Error:  Model can not be solved!"); 
   }
   else if(!readWorkFlow(options_.workFlow_)){//Use given workflow if posible
      if(options_.workFlow_.size()>0){
         std::cout << "Warning: can not parse workflow : " << options_.workFlow_ <<std::endl;
         std::cout << "Using default workflow ";
      }
      if(problemType_ == MWC){
         //std::cout << "(TTC)(MTC)(IC)(CC-IFD,TTC-I)" <<std::endl;
         readWorkFlow("(TTC)(MTC)(IC)(CC-IFD,TTC-I)");
      }
      else if(problemType_ == MC){
         //std::cout << "(CC-FDB)(IC)(CC-I)" <<std::endl;
         readWorkFlow("(CC-FDB)(IC)(CC-I)");
      }
      else{
         throw RuntimeError("Error:  Model can not be solved!"); 
      }
   }

   Timer timer,timer2;
   timer.tic();     
   //mcv.begin(*this);    
   size_t workingState = 0;
   while(workingState<workFlow_.size()){
      protocolateTiming_.push_back(std::vector<double>(20,0));
      protocolateConstraints_.push_back(std::vector<size_t>(20,0));
      std::vector<double>& T = protocolateTiming_.back();
      std::vector<size_t>& C = protocolateConstraints_.back();

      // Check for timeout
      timer.toc();
      if(timer.elapsedTime()>options_.timeOut_) {
         break;
      }
      //check for integer constraints   
      for (size_t it=1; it<10000000000; ++it) { 
         cplex_.setParam(IloCplex::Threads, options_.numThreads_); 
         cplex_.setParam(IloCplex::TiLim, options_.timeOut_-timer.elapsedTime());
         timer2.tic();
         if(!cplex_.solve()) {
            if(cplex_.getStatus() != IloAlgorithm::Unbounded){ 
               std::cout << "failed to optimize. " <<cplex_.getStatus()<< std::endl; 
               //Serious problem -> exit
               //mcv(*this);  
            }  
            else{ 
               //undbounded ray - most likely numerical problems
            }
         }
         if(cplex_.getStatus()!= IloAlgorithm::Unbounded){
            if(!integerMode_)
               bound_ = cplex_.getObjValue()+constant_;
            else{
               bound_ = cplex_.getBestObjValue()+constant_;
               if(!cplex_.solveFixed()) {
                  std::cout << "failed to fixed optimize." << std::endl; 
                  //mcv(*this);
               }
            } 
         }
         else{
            //bound is not set - todo
         }
         try{ cplex_.getValues(sol_, x_);}
         catch(IloAlgorithm::NotExtractedException e)  {
            //std::cout << "UPS: solution not extractable due to unbounded dual ... solving this"<<std::endl;
            // The following code is very ugly -> todo:  using rays instead
            sol_.clear();
            for(Vi v=0; v<numberOfTerminalEdges_+numberOfInternalEdges_+numberOfInterTerminalEdges_ + numberOfHigherOrderValues_; ++v){ 
               try{ 
                  sol_.add(cplex_.getValue(x_[v]));
               } catch(IloAlgorithm::NotExtractedException e)  {
                  sol_.add(0);          
               }
            } 
         }
        
         timer2.toc();
         T[Protocol_ID_Solve] += timer2.elapsedTime();

         if(false){
         //if(mcv(*this)!=0){
            workingState = workFlow_.size(); // go to the end of the workflow
            break;
         }         
 
         //std::cout << "... done."<<std::endl;
         
         //Find Violated Constraints
         IloRangeArray constraint = IloRangeArray(env_);
         constraintCounter_ = 0;
         
         //std::cout << "Search violated constraints ..." <<std::endl; 
       

         size_t cycleConstraints = std::numeric_limits<size_t>::max();
         bool   constraintAdded = false;
         for(std::vector<size_t>::iterator it=workFlow_[workingState].begin() ; it != workFlow_[workingState].end(); it++ ){
            size_t n = 0;
            size_t protocol_ID = Protocol_ID_Unknown;
            timer2.tic();
            if(*it == Action_ID_TTC){
               if(options_.verbose_) std::cout << "* Add  terminal triangle constraints: " << std::flush;
               n = findTerminalTriangleConstraints(constraint);
               if(options_.verbose_) std::cout << n << std::endl;
               protocol_ID = Protocol_ID_TTC;
            } 
            else if(*it == Action_ID_TTC_I){ 
               if(!integerMode_){
                  throw RuntimeError("Error: Calling integer terminal triangle constraint (TTC-I) seperation provedure before switching in integer mode (IC)"); 
               }
               if(options_.verbose_) std::cout << "* Add integer terminal triangle constraints: " << std::flush;
               conf(config.begin());
               n = findIntegerTerminalTriangleConstraints(constraint, config);
               if(options_.verbose_) std::cout << n  << std::endl;
               protocol_ID = Protocol_ID_TTC;
        
            }
            else if(*it == Action_ID_MTC){
               if(options_.verbose_) std::cout << "* Add multi terminal constraints: " << std::flush;
               n = findMultiTerminalConstraints(constraint);
               if(options_.verbose_) std::cout <<  n << std::endl;
               protocol_ID = Protocol_ID_MTC;
        
            }
            else if(*it == Action_ID_CC_I){
               if(!integerMode_){
                  throw RuntimeError("Error: Calling integer cycle constraints (CC-I) seperation provedure before switching in integer mode (IC)"); 
               }
               if(options_.verbose_) std::cout << "Add integer cycle constraints: " << std::flush;
               n = findIntegerCycleConstraints(constraint, false);
               if(options_.verbose_) std::cout << n  << std::endl; 
               protocol_ID = Protocol_ID_CC;
            } 
            else if(*it == Action_ID_CC_IFD){ 
               if(!integerMode_){
                  throw RuntimeError("Error: Calling facet defining integer cycle constraints (CC-IFD) seperation provedure before switching in integer mode (IC)"); 
               }
               if(options_.verbose_) std::cout << "Add facet defining integer cycle constraints: " << std::flush;
               n = findIntegerCycleConstraints(constraint, true);
               if(options_.verbose_) std::cout << n  << std::endl; 
               protocol_ID = Protocol_ID_CC;
            }
            else if(*it == Action_ID_CC){
               if(options_.verbose_) std::cout << "Add cycle constraints: " << std::flush;     
               n = findCycleConstraints(constraint, false, false);
               cycleConstraints=n;
               if(options_.verbose_) std::cout  << n << std::endl; 
               protocol_ID = Protocol_ID_CC;
            } 
            else if(*it == Action_ID_CC_FD){
               if(options_.verbose_) std::cout << "Add facet defining cycle constraints: " << std::flush;     
               n = findCycleConstraints(constraint, true, false);
               cycleConstraints=n;
               if(options_.verbose_) std::cout  << n << std::endl; 
               protocol_ID = Protocol_ID_CC;
            } 
            else if(*it == Action_ID_CC_FDB){
               if(options_.verbose_) std::cout << "Add facet defining cycle constraints (with bounding): " << std::flush;     
               n = findCycleConstraints(constraint, true, true);
               cycleConstraints=n;
               if(options_.verbose_) std::cout  << n << std::endl; 
               protocol_ID = Protocol_ID_CC;
            }
            else if(*it == Action_ID_CC_B){
               if(options_.verbose_) std::cout << "Add cycle constraints (with bounding): " << std::flush;     
               n = findCycleConstraints(constraint, false, true);
               cycleConstraints=n;
               if(options_.verbose_) std::cout  << n << std::endl; 
               protocol_ID = Protocol_ID_CC;
            } 
            else  if(*it == Action_ID_RemoveConstraints){ 
               if(options_.verbose_) std::cout << "Remove unused constraints: " << std::flush;            
               n = removeUnusedConstraints();
               if(options_.verbose_) std::cout  << n  << std::endl; 
               protocol_ID = Protocol_ID_RemoveConstraints;  
            }
            else  if(*it == Action_ID_IntegerConstraints){
               if(integerMode_) continue;
               if(options_.verbose_) std::cout << "Add  integer constraints: " << std::flush;
               n = enforceIntegerConstraints();
               if(options_.verbose_) std::cout  << n << std::endl; 
               protocol_ID = Protocol_ID_IntegerConstraints;  
            } 
            else  if(*it == Action_ID_OWC){
               if(cycleConstraints== std::numeric_limits<size_t>::max()){
                  std::cout << "WARNING: using Odd Wheel Contraints without Cycle Constrains! -> Use CC,OWC!"<<std::endl;
                  n=0;
               }
               else if(cycleConstraints==0){             
                  if(options_.verbose_) std::cout << "Add odd wheel constraints: " << std::flush;
                  n = findOddWheelConstraints(constraint);
                  if(options_.verbose_) std::cout  << n << std::endl;
               }
               else{
                  //since cycle constraints are found we have to search for more violated cycle constraints first
               }
               protocol_ID = Protocol_ID_OWC;  
            }
            else{
               std::cout <<"Unknown Inference State "<<*it<<std::endl;
            } 
            timer2.toc();
            T[protocol_ID] += timer2.elapsedTime();
            C[protocol_ID] += n;
            if(n>0) constraintAdded = true;
         }
         //std::cout <<"... done!"<<std::endl;
         
       
         
         if(!constraintAdded){
            //Switch to next working state
            ++workingState;
            if(workingState<workFlow_.size())
               if(options_.verbose_) std::cout <<std::endl<< "** Switching into next state ( "<< workingState << " )**" << std::endl;
            break;
         }
         else{
            timer2.tic();
            //Add Constraints
            model_.add(constraint);
            //cplex_.addCuts(constraint);
            timer2.toc();
            T[Protocol_ID_AddConstraints] += timer2.elapsedTime();
         }
         
         // Check for timeout
         timer.toc();
         if(timer.elapsedTime()>options_.timeOut_) {
            break;
         }
         
      } //end inner loop over one working state
   } //end loop over all working states
   
   //mcv.end(*this); 
   if(options_.verbose_){
      std::cout << "end normal"<<std::endl;
      std::cout <<"Protokoll:"<<std::endl;
      std::cout <<"=========="<<std::endl;
      std::cout << "  i  |   SOLVE  |   ADD    |    CC    |    OWC   |    TTC   |    MTV   |     IC    " <<std::endl;
      std::cout << "-----+----------+----------+----------+----------+----------+----------+-----------" <<std::endl;
   }
   std::vector<size_t> IDS;
   IDS.push_back(Protocol_ID_Solve);
   IDS.push_back(Protocol_ID_AddConstraints);
   IDS.push_back(Protocol_ID_CC);
   IDS.push_back(Protocol_ID_OWC);
   IDS.push_back(Protocol_ID_TTC);
   IDS.push_back(Protocol_ID_MTC);
   IDS.push_back(Protocol_ID_IntegerConstraints);
 
   if(options_.verbose_){
      for(size_t i=0; i<protocolateTiming_.size(); ++i){
         std::cout << std::setw(5)<<   i  ;
         for(size_t n=0; n<IDS.size(); ++n){
            std::cout << "|"<< std::setw(10) << setiosflags(std::ios::fixed)<< std::setprecision(4) << protocolateConstraints_[i][IDS[n]];
         }
         std::cout << std::endl; 
         std::cout << "     "  ; 
         for(size_t n=0; n<IDS.size(); ++n){ 
            std::cout << "|"<< std::setw(10) << setiosflags(std::ios::fixed)<< std::setprecision(4) << protocolateTiming_[i][IDS[n]];
         }
         std::cout << std::endl;
         std::cout << "-----+----------+----------+----------+----------+----------+----------+-----------" <<std::endl;
      }
      std::cout << "sum  ";
      double t_all=0;
      for(size_t n=0; n<IDS.size(); ++n){
         double t_one=0;
         for(size_t i=0; i<protocolateTiming_.size(); ++i){
            t_one += protocolateTiming_[i][IDS[n]];
         }
         std::cout << "|"<< std::setw(10) << setiosflags(std::ios::fixed)<< std::setprecision(4) << t_one;
         t_all += t_one;
      }
      std::cout << " | " <<t_all <<std::endl;
      std::cout << "-----+----------+----------+----------+----------+----------+----------+-----------" <<std::endl;
   }
}

template<class MODEL>
template<class OUT_ITER>
void Multicut<MODEL>::conf
(
   OUT_ITER outIter
)const
{
   INFERNO_CHECK(gm_.denseVariableIds(),"only with dense var ids atm");
   INFERNO_CHECK_OP(gm_.minVarId(),==,0,"var must start at zero atm")

   std::vector<LabelType> x(gm_.nVariables());
   if(problemType_ == MWC) {
      if(options_.MWCRounding_== options_.NEAREST){
         for(Vi node = 0; node<gm_.nVariables(); ++node) {
            double v = sol_[numberOfTerminals_*node+0];
            x[node] = 0;
            for(DiscreteLabel i=0; i<gm_.nLabels(node); ++i) {
               if(sol_[numberOfTerminals_*node+i]<v) {
                  x[node]=i;
               }
            }
         }
      }
      else if(options_.MWCRounding_==options_.DERANDOMIZED){
         this->derandomizedRounding(x);
      }
      else if(options_.MWCRounding_==options_.PSEUDODERANDOMIZED){
         this->pseudoDerandomizedRounding(x,1000);
      }
      else{
         //return UNKNOWN;
      }
   }
   else{
      std::vector<std::list<size_t> > neighbours0;
      partition(x, neighbours0,options_.edgeRoundingValue_);
   }
   std::copy(x.begin(), x.end(), outIter);
}
template<class MODEL>
std::vector<size_t>
Multicut<MODEL>::getSegmentation() const {
   std::vector<size_t> seg;
   std::vector<std::list<size_t> > neighbours0;
   partition(seg, neighbours0, 0.3);
   return seg;
}


template<class MODEL>
void Multicut<MODEL>::pseudoDerandomizedRounding
(
   std::vector<DiscreteLabel>& x,
   size_t bins
   ) const
{
   std::vector<bool>      processedBins(bins+1,false);
   std::vector<DiscreteLabel> temp;
   double                 value = 1000000000000.0;
   std::vector<DiscreteLabel> labelorder1(numberOfTerminals_,numberOfTerminals_-1);
   std::vector<DiscreteLabel> labelorder2(numberOfTerminals_,numberOfTerminals_-1);
   for(DiscreteLabel i=0; i<numberOfTerminals_-1;++i){
      labelorder1[i]=i;
      labelorder2[i]=numberOfTerminals_-2-i;
   } 
   for(size_t i=0; i<numberOfTerminals_*gm_.nVariables();++i){
      size_t bin;
      double t,d;
      if(sol_[i]<=0){
         bin=0;
         t=0;
      }
      else if(sol_[i]>=1){
         bin=bins;
         t=1;
      }
      else{
         bin = (size_t)(sol_[i]*bins)%bins;
         t = sol_[i];
      }
      if(!processedBins[bin]){
         processedBins[bin]=true;
         if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder1,sol_[i]))){
            value=d;
            x=temp;
         }
         if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder2,sol_[i]))){
            value=d;
            x=temp;
         }
      }
   }
}

template<class MODEL>
void Multicut<MODEL>::derandomizedRounding
(
   std::vector<DiscreteLabel>& x
   ) const
{
   std::vector<DiscreteLabel> temp;
   double                 value = 1000000000000.0;
   std::vector<DiscreteLabel> labelorder1(numberOfTerminals_,numberOfTerminals_-1);
   std::vector<DiscreteLabel> labelorder2(numberOfTerminals_,numberOfTerminals_-1);
   for(DiscreteLabel i=0; i<numberOfTerminals_-1;++i){
      labelorder1[i]=i;
      labelorder2[i]=numberOfTerminals_-2-i;
   }
   // Test primitives
   double d;
   if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder1,1e-8))){
      value=d;
      x=temp;
   }
   if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder2,1e-8))){
      value=d;
      x=temp;
   } 
   if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder1,1-1e-8))){
      value=d;
      x=temp;
   }
   if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder2,1-1e-8))){
      value=d;
      x=temp;
   }
   for(size_t i=0; i<numberOfTerminals_*gm_.nVariables();++i){
      if( sol_[i]>1e-8 &&  sol_[i]<1-1e-8){
         if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder1,sol_[i]))){
            value=d;
            x=temp;
         }
         if(value>(d=derandomizedRoundingSubProcedure(temp,labelorder2,sol_[i]))){
            value=d;
            x=temp;
         }
      }
   }
}

template<class MODEL>
double
Multicut<MODEL>::derandomizedRoundingSubProcedure
(
   std::vector<DiscreteLabel>& x,
   const std::vector<DiscreteLabel>& labelorder,
   const double threshold
   ) const
{ 
   const DiscreteLabel lastLabel = labelorder.back();
   const Vi numVar    = gm_.nVariables();

   x.assign(numVar,lastLabel);
  
   for(size_t i=0; i<labelorder.size()-1; ++i){
      for(Vi v=0; v<numVar; ++v){
         if(x[v]==lastLabel && sol_[numberOfTerminals_*v+i]<=threshold){
            x[v]=labelorder[i];
         }
      }
   }
   return gm_.eval(x.begin());
}




template<class MODEL>
void Multicut<MODEL>::partition
(
   std::vector<DiscreteLabel>& partit,
   std::vector<std::list<size_t> >& neighbours0,
   double threshold
) const
{

   partit.resize(numberOfNodes_,0);
   neighbours0.resize(numberOfNodes_, std::list<size_t>());

   size_t counter=0;
   for(size_t i=0; i<numberOfInternalEdges_; ++i) {
      Vi u = edgeNodes_[i].first;//variableIndex(0);
      Vi v = edgeNodes_[i].second;//variableIndex(1);
      if(sol_[numberOfTerminalEdges_+counter] <= threshold) {
         neighbours0[u].push_back(v);
         neighbours0[v].push_back(u);
      }
      ++counter;
   }

   DiscreteLabel p=0;
   std::vector<bool> assigned(numberOfNodes_,false);
   for(size_t node=0; node<numberOfNodes_; ++node) {
      if(assigned[node])
         continue;
      else{
         std::list<size_t> nodeList;
         partit[node]   = p;
         assigned[node] = true;
         nodeList.push_back(node);
         while(!nodeList.empty()) {
            size_t n=nodeList.front(); nodeList.pop_front();
            std::list<size_t>::const_iterator it;
            for(it=neighbours0[n].begin() ; it != neighbours0[n].end(); ++it) {
               if(!assigned[*it]) {
                  partit[*it] = p;
                  assigned[*it] = true;
                  nodeList.push_back(*it);
               }
            }
         }
         ++p;
      }
   }
}




template<class MODEL>
ValueType Multicut<MODEL>::bound() const
{
   return bound_;
}

template<class MODEL>
ValueType Multicut<MODEL>::value() const
{
   std::vector<DiscreteLabel> c(gm_.nVariables());
   conf(c);  
   ValueType value = gm_.eval(c.begin());
   return value;
}

template<class MODEL>
bool Multicut<MODEL>::readWorkFlow(std::string s)
{
   if(s[0]!='(' || s[s.size()-1] !=')')
      return false;
   workFlow_.clear();
   size_t n=0;
   std::string sepString;
   if(s.size()<2)
      return false;
   while(n<s.size()){
      if(s[n]==',' || s[n]==')'){//End of sepString
         if(sepString.compare("CC")==0)
            workFlow_.back().push_back(Action_ID_CC);
         else if(sepString.compare("CC-I")==0)
            workFlow_.back().push_back(Action_ID_CC_I);
         else if(sepString.compare("CC-IFD")==0)
            workFlow_.back().push_back(Action_ID_CC_IFD);
         else if(sepString.compare("CC-B")==0)
            workFlow_.back().push_back(Action_ID_CC_B);
         else if(sepString.compare("CC-FDB")==0)
            workFlow_.back().push_back(Action_ID_CC_FDB);
         else if(sepString.compare("CC-FD")==0)
            workFlow_.back().push_back(Action_ID_CC_FD);
         else if(sepString.compare("TTC")==0)
            workFlow_.back().push_back(Action_ID_TTC);
         else if(sepString.compare("TTC-I")==0)
            workFlow_.back().push_back(Action_ID_TTC_I);
         else if(sepString.compare("MTC")==0)
            workFlow_.back().push_back(Action_ID_MTC);
         else if(sepString.compare("OWC")==0)
            workFlow_.back().push_back(Action_ID_OWC);
         else if(sepString.compare("IC")==0)
            workFlow_.back().push_back(Action_ID_IntegerConstraints);
         else
            std::cout << "WARNING:  Unknown Seperation Procedure ' "<<sepString<<"' is skipped!"<<std::endl;
         sepString.clear();
      }
      else if(s[n]=='('){//New Round
         workFlow_.push_back(std::vector<size_t>()); 
      }
      else{
         sepString += s[n];
      }
      ++n;
   }
   return true;
}
  

///
/// computed sigle shortest path by the Dijkstra algorithm with following modifications:
/// * stop when target node (endNode) is reached
/// * optional avoid chordal paths (cordless = true)
/// * avoid paths that are longer than a threshold (maxLength)
template<class MODEL>
template<class DOUBLEVECTOR>
inline double Multicut<MODEL>::shortestPath(
   const Vi startNode, 
   const Vi endNode, 
   const std::vector<EdgeMapType >& E, //E[n][i].first/.second are the i-th neighbored node and weight-index (for w), respectively. 
   const DOUBLEVECTOR& w,
   std::vector<Vi>& shortestPath,
   const double maxLength,
   bool cordless
) const
{ 
   
   const Vi numberOfNodes = E.size();
   const double    inf           = std::numeric_limits<double>::infinity();
   const Vi nonePrev      = endNode;

   std::vector<Vi>  prev(numberOfNodes,nonePrev);
   std::vector<double>     dist(numberOfNodes,inf);
   MYSET                   openNodes;
   
   openNodes.insert(startNode);
   dist[startNode]=0.0;

   while(!openNodes.empty()){ 
      Vi node;
      //Find smallest open node
      {
         typename MYSET::iterator it, itMin;
         double v = std::numeric_limits<double>::infinity();
         for(it = openNodes.begin(); it!= openNodes.end(); ++it){
            if( dist[*it]<v ){
               v = dist[*it];
               itMin = it;
            }
         }
         node = *itMin;
         openNodes.erase(itMin);
      }
      // Check if target is reached
      if(node == endNode)
         break;
      // Update all neigbors of node
      {
         typename EdgeMapType::const_iterator it;
         for(it=E[node].begin() ; it != E[node].end(); ++it) {
            const Vi node2      = (*it).first;  //second edge-node
            const LPIndexType weighId  = (*it).second; //index in weigh-vector w
            double cuttedWeight        = std::max(0.0,w[weighId]); //cut up negative edge-weights
            const double weight2       = dist[node]+cuttedWeight;
           

            if(dist[node2] > weight2 && weight2 < maxLength){
               //check chordality
               bool updateNode = true;
               if(cordless) {
                  Vi s = node;
                  while(s!=startNode){
                     s= prev[s];
                     if(s==startNode && node2==endNode) continue;
                     if(neighbours[node2].find(s)!=neighbours[node2].end()) {
                        updateNode = false; // do not update node if path is chordal
                        break;
                     } 
                  }
               } 
               if(updateNode){
                  prev[node2] = node;
                  dist[node2] = weight2;
                  openNodes.insert(node2);
               } 
            }
         }
      }
   }
   
   if(prev[endNode] != nonePrev){//find path?
      shortestPath.clear();
      shortestPath.push_back(endNode);
      Vi n = endNode;
      do{
         n=prev[n];
         shortestPath.push_back(n);
      }while(n!=startNode);
      INFERNO_ASSERT(shortestPath.back() == startNode);
   }
   
   return dist[endNode];
}


template<class MODEL>
std::vector<double>
Multicut<MODEL>::getEdgeLabeling
() const
{
   std::vector<double> l(numberOfInternalEdges_,0);
   for(size_t i=0; i<numberOfInternalEdges_; ++i) {
      l[i] = sol_[numberOfTerminalEdges_+i];
   }
   return l;
}

// WARNING: this function is considered experimental.
// variable indices refer to variables of the LP set up
// in the constructor of the class template LPCplex,
// NOT to the variables of the graphical model.
template<class MODEL>
template<class LPVariableIndexIterator, class CoefficientIterator>
void Multicut<MODEL>::addConstraint
(
   LPVariableIndexIterator viBegin,
   LPVariableIndexIterator viEnd,
   CoefficientIterator coefficient,
   const ValueType& lowerBound,
   const ValueType& upperBound)
{
   IloRange constraint(env_, lowerBound, upperBound);
   while(viBegin != viEnd) {
      constraint.setLinearCoef(x_[*viBegin], *coefficient);
      ++viBegin;
      ++coefficient;
   }
   model_.add(constraint);
   // this update of model_ does not require a
   // re-initialization of the object cplex_.
   // cplex_ is initialized in the constructor.
}

} // end namespace inference
} // end namespace inferno

#endif
