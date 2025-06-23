#include "mistake.h"

Mistake::Mistake() {}
Mistake::Mistake(QString str)
{
    message = str;
}

Mistake::Mistake(QString str, int node1, int node2)
{
    message = str;
    nodeIds.first = node1;
    nodeIds.second = node2;
}
