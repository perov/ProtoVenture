
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

void DeleteBranch
(shared_ptr<Node>);

class VentureException__ForcedMHDecline : std::runtime_error {
public:
  VentureException__ForcedMHDecline();
};

void TouchNode(shared_ptr<Node> node, stack< shared_ptr<Node> >& touched_nodes, int proposal_unique_id);

void MakeMHProposal(int proposal_unique_id);

struct ProposalInfo : public boost::enable_shared_from_this<ProposalInfo> {
  size_t proposal_unique_id;
  bool request_to_terminate;
};

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
