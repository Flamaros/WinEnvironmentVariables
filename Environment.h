#pragma once

#include <QStringList>

// TODO Check if Windows variables are correctly updated if this application modify them throw the registry

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
