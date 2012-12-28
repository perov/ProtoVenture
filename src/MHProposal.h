
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

void MakeMHProposal();

enum MHDecision { MH_DECLINED, MH_APPROVED };

#endif
