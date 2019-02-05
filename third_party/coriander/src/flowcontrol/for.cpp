// Copyright Hugh Perkins 2016

// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at

//     http://www.apache.org/licenses/LICENSE-2.0

// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "flowcontrolinstructions.h"
#include "ir-to-opencl.h"

#include <iostream>
#include <sstream>

using namespace std;
using namespace llvm;

namespace cocl {
namespace flowcontrol {

int For::getNumChildren() const {
    return 2;
}
Block *For::getChild(int idx) {
    if(idx == 0) {
        return preBlock;
    } else if(idx == 1) {
        return body;
    }
    throw runtime_error("illegal request");
}
std::string For::generateCl(std::string indent, bool noLabel) {
    dumped = true;
    string gencode = "";
    gencode += indent + "for(\n";
    gencode += preBlock->generateCl(indent + "    ", true);
    gencode += indent + "    ; " + dumpOperand(condition) + ";) {\n";
    gencode += body->generateCl(indent + "    ");
    gencode += indent + "}\n";
    if(next != 0) {
        gencode += next->generateCl(indent);
    }
    return gencode;
}
void For::walk(std::function<void(Block *block)> fn) {
    fn(this);
    if(preBlock != 0) {
        preBlock->walk(fn);
    }
    if(body != 0) {
        body->walk(fn);
    }
    if(next != 0) {
        next->walk(fn);
    }
}
int For::numSuccessors() const {
    if(next != 0) {
        return 1;
    }
    return 0;
}
Block *For::getSuccessor(int idx) {
    if(idx > 0) {
        throw runtime_error("illegal request");
    }
    return next;
}
void For::replaceSuccessor(Block *oldChild, Block *newChild) {
    if(next == oldChild) {
        next = newChild;
        return;
    }
    throw runtime_error("couldnt find old child");
}
void For::replaceChildOrSuccessor(Block *oldChild, Block *newChild) {
    if(preBlock == oldChild) {
        preBlock = newChild;
        return;
    }
    if(body == oldChild) {
        body = newChild;
        return;
    }
    if(next == oldChild) {
        next = newChild;
        return;
    }
    throw runtime_error("couldnt find old child");
}
void For::dump(set<const Block *> &seen, string indent) const {
    seen.insert(this);
    cout << indent << "For " << this->id << gotoFreeString() << isExitString() << uncontainedJumpsString() << endl;
    cout << indent << "  Pre:" << endl;
    preBlock->dump(seen, indent + "    ");
    cout << indent << "  Body:" << endl;
    body->dump(seen, indent + "    ");
    if(next != 0) {
        if(seen.find(next) == seen.end()) {
            next->dump(seen, indent);
        } else {
            cout << "(*" << next->id << endl;
        }
    }
}

} // flowcontrol
} // cocl
