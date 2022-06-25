#include "node.h"

bool Node::operator>(Node &other)
{
    if(this->frequence > other.frequence){
        return true;
    }else{
        return false;
    }
}
