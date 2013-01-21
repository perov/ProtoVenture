
#ifndef VENTURE___MHPROPOSAL_H
#define VENTURE___MHPROPOSAL_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "XRPCore.h"
#include "Utilities.h"

struct MHProposalResults {
  MHProposalResults(real logscore_PNew) // Should be in the MHProposal.h?
    : logscore_PNew(logscore_PNew)
  {
  
  }

  real logscore_PNew;
};

bool VerifyOrderPattern(vector<size_t>&,
                        vector<size_t>&);

struct Node;

void DeleteNode(shared_ptr<Node>);

void DeleteBranch(shared_ptr<Node>, bool);

class VentureException__ForcedMHDecline : std::runtime_error {
public:
  VentureException__ForcedMHDecline();
};

void TouchNode(shared_ptr<Node> node, stack< shared_ptr<Node> >& touched_nodes, vector< shared_ptr<Node> >& touched_nodes2, int proposal_unique_id);

MHProposalResults MakeMHProposal(shared_ptr<NodeXRPApplication> principal_node, shared_ptr<VentureValue> proposing_value, shared_ptr< map<string, shared_ptr<VentureValue> > > random_database, bool forcing_not_collecting);

real GatherBranch__QNewToOld(shared_ptr<Node> first_node, size_t node_action);
real GatherBranch__PNew(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices);
real GatherBranch__POld(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices, bool if_old_variables);

struct ProposalInfo : public boost::enable_shared_from_this<ProposalInfo> {
  size_t proposal_unique_id;
  bool request_to_terminate;
};

enum MHDecision { MH_DECLINED, MH_APPROVED };

void Enumerate(shared_ptr<NodeXRPApplication> principal_node);

#endif
