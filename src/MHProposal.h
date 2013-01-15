
#ifndef VENTURE___MHPROPOSAL_H
#define VENTURE___MHPROPOSAL_H

#include "Header.h"
#include "VentureValues.h"
#include "VentureParser.h"
#include "XRPCore.h"
#include "Utilities.h"

bool VerifyOrderPattern(vector<size_t>&,
                        vector<size_t>&);

struct Node;

void DeleteNode(shared_ptr<Node>);

void DeleteBranch(shared_ptr<Node>);

class VentureException__ForcedMHDecline : std::runtime_error {
public:
  VentureException__ForcedMHDecline();
};

void TouchNode(shared_ptr<Node> node, stack< shared_ptr<Node> >& touched_nodes, vector< shared_ptr<Node> >& touched_nodes2, int proposal_unique_id);

void MakeMHProposal(int proposal_unique_id);

real GatherBranch__QNewToOld(shared_ptr<Node> first_node, size_t node_action);
real GatherBranch__PNew(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices);
real GatherBranch__POld(shared_ptr<Node> first_node, size_t node_action, set< shared_ptr<NodeXRPApplication> >& changing_random_choices, bool if_old_variables);

struct ProposalInfo : public boost::enable_shared_from_this<ProposalInfo> {
  size_t proposal_unique_id;
  bool request_to_terminate;
};

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
