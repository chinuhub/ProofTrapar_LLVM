/*
 * DotToAFA.cpp
 *
 *  Created on: 25-Aug-2015
 *      Author: adityaverma
 */

#include "AFA/AFAState.h"
#include "AFA/AFAut.h"
#include <map>
#include <stdlib.h>
#include "MetaState.h"

typedef boost::graph_traits<Graph>::vertex_iterator vertex_iterator;
typedef boost::graph_traits<Graph>::edge_iterator edge_iterator;
typedef boost::graph_traits<Graph>::in_edge_iterator in_edge_iterator;
typedef boost::graph_traits<Graph>::out_edge_iterator out_edge_iterator;
typedef boost::graph_traits<Graph>::adjacency_iterator adjacency_iterator;
//using namespace boost;

std::string underscore = "_";

//  bool read_graphviz(std::istream& in, MutableGraph& graph,
//                     dynamic_properties& dp,
//                     const std::string& node_id = "node_id");

//struct EdgeProp{std::string label;};
//struct VertexProp{ std::string vertlabel; std::string xlabel; std::string shape; std::string color;};
////
//////Define the graph using those classes
//typedef boost::adjacency_list<boost::listS, boost::vecS, boost::directedS, VertexProp, EdgeProp > Graph;
//
//////Some typedefs for simplicity
//typedef boost::graph_traits<Graph>::vertex_descriptor vertex_t;
//typedef boost::graph_traits<Graph>::edge_descriptor edge_t;


AFAStatePtr AFAState::ConstructAFA(const Graph &graph,std::map<vertex_t,AFAStatePtr>& mAllStates, vertex_t vertex){
//    std::pair<out_edge_iterator,out_edge_iterator> oeit = boost::out_edges(vertex,graph);
//    if(oeit.first==oeit.second) return;
//  if(seenset.find(vertex)==seenset.end()) return nullptr;

    VertexProp vertex_properties = graph[vertex];
    z3::context c;
    z3::expr exp = c.bool_val(true);

   AFAStatePtr node = new AFAState(ORLit,exp);

//   std::cout<<vertex_properties.xlabel<<std::endl;

   mAllStates[vertex] = node;

   // check is if it is an accepting state
   if(vertex_properties.shape == "doubleoctagon") node->mIsAccepted =true;

   // add other properties, only to verify the algorithm with PrintToDot function
   if(vertex_properties.color == "green" && (vertex_properties.shape=="doubleoctagon" || vertex_properties.shape=="octagon")) node->mType = ORLit;
   else if(vertex_properties.color == "cyan" && (vertex_properties.shape=="doubleoctagon" || vertex_properties.shape=="rectangle")) node->mType = OR;
   else if(vertex_properties.color == "yellow" && vertex_properties.shape == "rectangle") node->mType = AND;
   node->mAssumeSym = vertex_properties.vertlabel + "_" + vertex_properties.xlabel;

   std::pair<out_edge_iterator,out_edge_iterator> edges = boost::out_edges(vertex,graph);

   std::map<std::pair<std::string,std::string>,std::set<vertex_t>> transitions;

   for(;edges.first!=edges.second;edges.first++){
       std::string ed1 = graph[*edges.first].label;
       int i=0;
       for(; i<ed1.size();i++){
           if(ed1[i]=='_') break;
       }
       if(i==ed1.size()) { std::cout<<"Invalid Edge Label"<<std::endl; exit(0);}

       std::string sym = ed1.substr(0,i);
       std::string index = ed1.substr(i+1);

       transitions[{sym,index}].insert(boost::target(*edges.first,graph));
   }

   for(std::pair<std::pair<std::string,std::string>,std::set<vertex_t>> transition : transitions){
       SetAFAStatesPtr set;
       for(vertex_t vert : transition.second){
           if(mAllStates.find(vert)!=mAllStates.end()){
               set.insert(mAllStates[vert]);
           }
           else {
               set.insert(ConstructAFA(graph,mAllStates,vert));
           }
       }
       node->mTransitions.insert(std::pair<std::string,SetAFAStatesPtr>(transition.first.first,set));
   }

    return node;
}



AFAut* AFAut::MakeAFAutFromDot(std::string filename){

    std::ifstream inf(filename);

    if(inf.is_open()) std::cout<<"dotfile opened"<<std::endl;
    else std::cout<<"dotfile not opened"<<std::endl;

    boost::adjacency_list<boost::listS,boost::vecS,boost::directedS,VertexProp,EdgeProp> graph;
    boost::dynamic_properties dp(boost::ignore_other_properties);

    dp.property("vertlabel",boost::get(&VertexProp::vertlabel,graph));
    dp.property("label",boost::get(&VertexProp::xlabel,graph));
    dp.property("shape",boost::get(&VertexProp::shape,graph));
    dp.property("color",boost::get(&VertexProp::color,graph));
    dp.property("label",boost::get(&EdgeProp::label,graph));

    const std::string node_id = "vertlabel";
    bool read_graphviz_status = boost::read_graphviz(inf,graph,dp,node_id);

    if(read_graphviz_status) std::cout<<"read_graphviz successful"<<std::endl;
    else std::cout<<"read_graphviz unsuccessful"<<std::endl;

    // Finding Initial State
    std::pair<vertex_iterator,vertex_iterator> vit = boost::vertices(graph);
    vertex_t init_vertex;

//    ctx.bool_val(false); look for how to initialise z3 context

    for (vit.first; vit.first != vit.second; ++vit.first){
        if(graph[*vit.first].vertlabel=="init") init_vertex = *vit.first;
    }

    AFAut* afa = new AFAut();
    std::map<vertex_t,AFAStatePtr> mAllStates;
    afa->mInit = AFAState::ConstructAFA(graph,mAllStates,boost::target(*(boost::out_edges(init_vertex,graph).first),graph));
    return afa;
}


faudes::Generator MakePAFromDot(const std::string &filename) {

    std::ifstream inf(filename);

    if(inf.is_open()) std::cout<<"dotfile opened"<<std::endl;
    else std::cout<<"dotfile not opened"<<std::endl;

    boost::adjacency_list<boost::listS,boost::vecS,boost::directedS,VertexProp,EdgeProp> graph;
    boost::dynamic_properties dp(boost::ignore_other_properties);

    dp.property("vertlabel",boost::get(&VertexProp::vertlabel,graph));
    dp.property("label",boost::get(&VertexProp::xlabel,graph));
    dp.property("shape",boost::get(&VertexProp::shape,graph));
    dp.property("color",boost::get(&VertexProp::color,graph));
    dp.property("label",boost::get(&EdgeProp::label,graph));

    const std::string node_id = "vertlabel";
    bool read_graphviz_status = boost::read_graphviz(inf,graph,dp,node_id);

    if(read_graphviz_status) std::cout<<"read_graphviz successful"<<std::endl;
    else std::cout<<"read_graphviz unsuccessful"<<std::endl;


    faudes::Generator generator;

    // Finding Initial State
    std::pair<vertex_iterator,vertex_iterator> vit = boost::vertices(graph);

//    for(vit.first;vit.first!=vit.second;vit.first++) { VertexProp vv = graph[*vit.first]; std::cout<<vv.vertlabel<<" "<<vv.xlabel<<" "<<vv.shape<<" "<<vv.color<<std::endl;}

    for (vit.first; vit.first != vit.second; ++vit.first){
        VertexProp vv = graph[*vit.first];
        if(vv.shape=="none") {
            VertexProp vinit = graph[boost::target(*(boost::out_edges(*vit.first,graph).first),graph)];
//            generator.InsState(vinit.vertlabel);
            generator.SetInitState(vinit.vertlabel);
        }
        else generator.InsState(vv.vertlabel);
        if(vv.shape=="doublecircle") generator.SetMarkedState(vv.vertlabel);
    }
//
//    for(auto it = generator.StatesBegin();it!=generator.StatesEnd();it++){
//        std::cout<<it<<endl;
//    }
    vit = boost::vertices(graph);

    for (vit.first; vit.first != vit.second; ++vit.first) {
        VertexProp vv = graph[*vit.first];
        std::pair<out_edge_iterator,out_edge_iterator> edges = boost::out_edges(*vit.first,graph);
        for (;edges.first!=edges.second;edges.first++){
            EdgeProp ee = graph[*edges.first];
            std::string symstr = ee.label;
            if(symstr=="") continue;
//            std::cout<<symstr<<std::endl;
            generator.InsEvent(symstr);
            //States corresponding to source and destination have already been added in the generator by previous loop.
            // so just insert an edge between those states.
            generator.SetTransition(vv.vertlabel, symstr, graph[boost::target(*edges.first,graph)].vertlabel);
        }
    }
    return generator;
}



void PassThree(AFAStatePtr afa, Graph& g, std::map<AFAStatePtr, vertex_t>& indmap){//imp not to have mapstatecomparator here just compare raw pointers
    if(indmap.find(afa)!=indmap.end())
        return;

    vertex_t vthis = boost::add_vertex(g);
    indmap.insert(std::make_pair(afa,vthis));

    int x=0;
    int y=0;

//    std::cout<<afa->mAssumeSym<<std::endl;

    if(afa->mType==AND){
        SetAFAStatesPtr nextset;
        //create a new vertes for this state..
        VertexProp vp;
        int i=0;
//        for(; i<afa->mAssumeSym.size();i++){
//            if(afa->mAssumeSym[i]=='_') break;
//        }
//        vp.xlabel = afa->mAssumeSym.substr(0,i);
//        vp.vertlabel = afa->mAssumeSym.substr(i+1);
//        std::cout<<vp.vertlabel<<" "<<vp.xlabel<<std::endl;

       vp.vertlabel = std::to_string(x++);
        vp.xlabel = std::to_string(y++);

        vp.shape="rectangle";
        vp.color="yellow";
        g[vthis]=vp;
        int count=0;
//		std::cout<<"Total number of transition sout from this stat is "<<mTransitions.size()<<std::endl;
        BOOST_FOREACH(auto t, afa->mTransitions)
                    {
                        nextset=t.second;
                        std::string sym = t.first;
                        EdgeProp edgep;
                        sym.append(underscore);
                        edgep.label=sym.append((std::to_string(count)));

                        BOOST_FOREACH(auto st, nextset)
                                    {
                                        PassThree(st,g,indmap);
                                        BOOST_ASSERT_MSG(indmap.find(st)!=indmap.end(),"Some problem as by now this must have been added");
                                        vertex_t vert = indmap.find(st)->second;
                                        if(st!=afa)
                                            BOOST_ASSERT_MSG(vert!=vthis,"Some serious issue");
                                        //creat a dot transitions from index to dotnum to the file..
                                        edge_t e;
                                        bool added;
                                        boost::tie(e,added)=boost::add_edge(vthis,vert,g);
                                        BOOST_ASSERT_MSG(added,"Some issue");
                                        g[e]=edgep;
                                    }
                        count++;
                    }


    }else if (afa->mType==OR){
        SetAFAStatesPtr nextset;
        //create a new vertes for this state..

        VertexProp vp;
//        int i=0;
//        for(; i<afa->mAssumeSym.size();i++){
//            if(afa->mAssumeSym[i]=='_') break;
//        }
//        vp.xlabel = afa->mAssumeSym.substr(0,i);
//        vp.vertlabel = afa->mAssumeSym.substr(i+1);
//        std::cout<<vp.vertlabel<<" "<<vp.xlabel<<std::endl;

        vp.vertlabel = std::to_string(x++);
        vp.xlabel = std::to_string(y++);


        vp.shape="rectangle";
        if(afa->mIsAccepted){
            vp.shape="doubleoctagon";
        }
        vp.color="cyan";
        g[vthis]=vp;
        int count=0;
//		std::cout<<"Total number of transition sout from this"<< this<<" stat is "<<mTransitions.size()<<std::endl;
        BOOST_FOREACH(auto t, afa->mTransitions)
                    {
                        nextset=t.second;
                        std::string sym = t.first;
                        EdgeProp edgep;
                        sym.append(underscore);
                        edgep.label=sym.append((std::to_string(count)));
                        BOOST_FOREACH(auto st, nextset)
                                    {
//					std::cout<<"Transition to "<<st<<" from "<<this<<" on sym"<<std::endl;
                                        PassThree(st,g,indmap);
                                        BOOST_ASSERT_MSG(indmap.find(st)!=indmap.end(),"Some problem as by now this must have been added");
                                        vertex_t vert = indmap.find(st)->second;
                                        if(st!=afa)
                                            BOOST_ASSERT_MSG(vert!=vthis,"Some serious issue");

                                        //creat a dot transitions from index to dotnum to the file..
                                        edge_t e;
                                        bool added;
                                        boost::tie(e,added)=boost::add_edge(vthis,vert,g);
                                        BOOST_ASSERT_MSG(added,"Some issue");
                                        g[e]=edgep;
                                    }
                        count++;
                    }

    }else if(afa->mType==ORLit){
        SetAFAStatesPtr nextset;
        //create a new vertes for this state..

        VertexProp vp;
//        int i=0;
//        for(; i<afa->mAssumeSym.size();i++){
//            if(afa->mAssumeSym[i]=='_') break;
//        }
//        vp.xlabel = afa->mAssumeSym.substr(0,i);
//        vp.vertlabel = afa->mAssumeSym.substr(i+1);
//        std::cout<<vp.vertlabel<<" "<<vp.xlabel<<std::endl;

            vp.vertlabel = std::to_string(x++);
            vp.xlabel = std::to_string(y++);


        if(afa->mIsAccepted){
            vp.shape="doubleoctagon";
            vp.color="green";
        }else
        {
            vp.shape="octagon";
            vp.color="green";
        }
        g[vthis]=vp;
        int count=0;

//		std::cout<<"Total number of transition sout from this stat is "<<mTransitions.size()<<std::endl;
        BOOST_FOREACH(auto t, afa->mTransitions)
                    {
                        nextset=t.second;
                        std::string sym = t.first;
                        EdgeProp edgep;
                        sym.append(underscore);
                        edgep.label=sym.append((std::to_string(count)));
                        BOOST_FOREACH(auto st, nextset)
                                    {
                                        PassThree(st,g,indmap);
                                        BOOST_ASSERT_MSG(indmap.find(st)!=indmap.end(),"Some problem as by now this must have been added");
                                        vertex_t vert = indmap.find(st)->second;
                                        if(st!=afa)
                                            BOOST_ASSERT_MSG(vert!=vthis,"Some serious issue");

                                        //creat a dot transitions from index to dotnum to the file..
                                        edge_t e;
                                        bool added;
                                        boost::tie(e,added)=boost::add_edge(vthis,vert,g);
                                        BOOST_ASSERT_MSG(added,"Some issue");
                                        g[e]=edgep;
                                    }
                        count++;
                    }

    }else
        BOOST_ASSERT_MSG(false,"Serious error, control must not have reached here");
}


template <class Name>
class myEdgeWriter {
public:
    myEdgeWriter(Name _name) : name(_name) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
        out << "[label=\"" << name[v].label << "\"]";
    }
private:
    Name name;
};


template <class Name>
class myVertWriter {
public:
    myVertWriter(Name _name) : name(_name) {}
    template <class VertexOrEdge>
    void operator()(std::ostream& out, const VertexOrEdge& v) const {
//                                   out << "[label=\"" << name[v].vertlabel << "\", xlabel=\" "<<name[v].xlabel<<"\", color="<<name[v].color<<",shape="<<name[v].shape<<"]";
        out << "[label=\"" << name[v].vertlabel << "\", color="<<name[v].color<<",shape="<<name[v].shape<<"]";
    }
private:
    Name name;
};

void PrintToDot(AFAStatePtr afa, std::string filename){
    Graph graph;
    std::map<AFAStatePtr,vertex_t> mapindex;
    PassThree(afa,graph,mapindex);//fill the graph object

    myEdgeWriter<Graph> ew(graph);
    myVertWriter<Graph> vw(graph);
    //IMPO we need to put writers after graph is filled.

    //	#ifdef DEBUG_FILE

    std::ofstream outf(filename);
    write_graphviz(outf, graph, vw,ew);

}


int main(){

//    std::ifstream dotfile("../../Pass1.dot");
 //  std::string file2 = "Pass2AfterDeletion";

//
//    std::string input = "ec4";
//
//    AFAut* afa = AFAut::MakeAFAutFromDot(input+".dot");
//
//    afa->NewEpsilonClosure(afa->mInit);
//
//    std::cout<<"Correct working"<<std::endl;
//
//    //afa->PrintToDot(input+"_out.dot");
//
//    PrintToDot(afa->mInit,input+"_out.dot");
//



//    std::string pa1 = "pa1";
//    std::string pa2 = "pa2";
//    std::string pa3 = "auto1";

//    std::string pa = "pa1";
//
//    faudes::Generator gen = MakePAFromDot(pa+".dot");
//    //gen.DotWrite(pa+"_out.dot");
//
//    MetaState::generator = gen;
//
//    std::string ans = MetaState::Verify(MetaState::generator);
//
//    std::cout<<"Accepted word is : "<<ans<<std::endl;
//        std::cout<<"Done";



    //PA created for on the fly testing
    std::string pa = "pa1";
    faudes::Generator gen = MakePAFromDot(pa+".dot");
    MetaState::generator = gen;
    //MetaState::generator.DotWrite("test_pa_out.dot");


    //AFAs created for on the fly testing
    std::vector<AFAStatePtr> afaRoots;
    std::vector<std::string> inputs = {"afa1"};
    //std::vector<std::string> inputs = {"afa21", "afa22"};

    for(auto inp : inputs){

        AFAut* afa = AFAut::MakeAFAutFromDot(inp+".dot");
        //PrintToDot(afa->mInit,inp+"_out.dot");
        afaRoots.push_back(afa->mInit);
    }


    std::cout<<"\nOn the fly algo starts"<<std::endl;

    std::string trace = MetaState::getUncoveredTrace(MetaState::generator, afaRoots);

    std::cout<<"Done"<<std::endl;
    std::cout<<"Uncovered Trace: "<<trace;

    return 0;
}















//// Print Vertices/Edges Set of Graph
//std::pair<vertex_iterator,vertex_iterator> vit = boost::vertices(graph);
//
//for (vit.first; vit.first != vit.second; ++vit.first)
//std::cout << graph[(*vit.first)].vertlabel << " "<<graph[(*vit.first)].xlabel<<" "<<graph[(*vit.first)].color<<" "<<graph[(*vit.first)].shape<<std::endl;
//
//std::pair<edge_iterator,edge_iterator> eit = boost::edges(graph);
//
//for (eit.first; eit.first != eit.second; ++eit.first)
//std::cout << graph[(*eit.first)].label << std::endl;

//// end

///// Trial Dynamic Properties code
//    boost::associative_property_map<std::map<Graph,VertexProp>> vertex_p;
//
//    typedef boost::property<vertex_t,VertexProp> NodeProperty;
//    typedef boost::property<edge_t,EdgeProp> EdgeProperty;
//
//    using IdMap = boost::property_map<Graph,boost::vertex_index_t>::const_type;
//
//    IdMap indexMap(get(boost::vertex_index,graph));
//
//    boost::vector_property_map<VertexProp,IdMap> props(boost::num_vertices(graph),indexMap);
//
//    dp.property("node_id",props);
//
//    dp.property("Vertex",get(vertex_t,graph));
//// end