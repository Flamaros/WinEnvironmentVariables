#pragma once

#include <QStringList>

class Environment
{
public:
    struct Variable
    {
        QString name;
        QString owner;
        QString value;
    };

public:
    Environment();

    QList<Variable> simpleVariables() const {return mSimpleVariables;}

private:
    QList<Variable> mSimpleVariables;
};
