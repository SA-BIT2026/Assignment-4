/**
 * Andersen.cpp
 * @author kisslune
 */

#include "A4Header.h"
#include "Graphs/ConsG.h"

using namespace llvm;
using namespace std;

int main(int argc, char** argv)
{
    auto moduleNameVec =
            OptionBase::parseOptions(argc, argv, "Whole Program Points-to Analysis",
                                     "[options] <input-bitcode...>");

    SVF::LLVMModuleSet::buildSVFModule(moduleNameVec);

    SVF::SVFIRBuilder builder;
    auto pag = builder.build();
    auto consg = new SVF::ConstraintGraph(pag);
    consg->dump();
    pag->dump();

    Andersen andersen(consg);

    andersen.runPointerAnalysis();

    andersen.dumpResult();
    SVF::LLVMModuleSet::releaseLLVMModuleSet();
	return 0;
}


void Andersen::runPointerAnalysis()
{
    // Keep the constraints as plain integer edge lists. Solving introduces new
    // copy edges, and looking up a field object can add nodes to the graph, so
    // the graph's own edge sets are not safe to iterate while it changes.
    struct GepEdge
    {
        unsigned dst;
        const SVF::GepCGEdge *edge;
    };

    map<unsigned, set<unsigned>> copySucc;
    map<unsigned, set<unsigned>> loadSucc;
    map<unsigned, set<unsigned>> storePred;
    map<unsigned, vector<GepEdge>> gepSucc;

    for (const SVF::ConstraintEdge *edge : consg->getDirectCGEdges())
    {
        unsigned src = edge->getSrcID();
        unsigned dst = edge->getDstID();
        switch (edge->getEdgeKind())
        {
            case SVF::ConstraintEdge::Copy:
                copySucc[src].insert(dst);
                break;
            case SVF::ConstraintEdge::NormalGep:
            case SVF::ConstraintEdge::VariantGep:
                if (const SVF::GepCGEdge *gep = SVF::SVFUtil::dyn_cast<SVF::GepCGEdge>(edge))
                    gepSucc[src].push_back({dst, gep});
                break;
            default:
                break;
        }
    }

    for (const SVF::ConstraintEdge *edge : consg->getLoadCGEdges())
        loadSucc[edge->getSrcID()].insert(edge->getDstID());

    for (const SVF::ConstraintEdge *edge : consg->getStoreCGEdges())
        storePred[edge->getDstID()].insert(edge->getSrcID());

    // p = &o means o is in pts(p). These are the only constraints that hold
    // before anything has been propagated.
    WorkList<unsigned> workList;
    for (const SVF::ConstraintEdge *edge : consg->getAddrCGEdges())
        if (pts[edge->getDstID()].insert(edge->getSrcID()).second)
            workList.push(edge->getDstID());

    while (!workList.empty())
    {
        unsigned id = workList.pop();
        auto ptsIt = pts.find(id);
        if (ptsIt == pts.end())
            continue;
        // A node goes back on the list whenever its points-to set grows, so
        // every rule below is applied again with the full, updated set.
        set<unsigned> pointees = ptsIt->second;

        // q = *p and *p = q turn into copy edges once p points at o.
        for (unsigned o : pointees)
        {
            for (unsigned q : loadSucc[id])
                if (copySucc[o].insert(q).second)
                    workList.push(o);
            for (unsigned v : storePred[id])
                if (copySucc[v].insert(o).second)
                    workList.push(v);
        }

        // q = p: pts(q) |= pts(p)
        for (unsigned q : copySucc[id])
        {
            bool changed = false;
            for (unsigned o : pointees)
                if (pts[q].insert(o).second)
                    changed = true;
            if (changed)
                workList.push(q);
        }

        // q = &p->field: a constant offset selects that field of each object,
        // a variant offset collapses the object to its field-insensitive node.
        for (const GepEdge &gep : gepSucc[id])
        {
            bool changed = false;
            for (unsigned o : pointees)
            {
                unsigned field;
                if (consg->isBlkObjOrConstantObj(o))
                    field = o;
                else if (const SVF::NormalGepCGEdge *normal =
                        SVF::SVFUtil::dyn_cast<SVF::NormalGepCGEdge>(gep.edge))
                    field = consg->getGepObjVar(
                            o, normal->getAccessPath().getConstantStructFldIdx());
                else
                    field = consg->getGepObjVar(o, gep.edge);
                if (pts[gep.dst].insert(field).second)
                    changed = true;
            }
            if (changed)
                workList.push(gep.dst);
        }
    }
}
